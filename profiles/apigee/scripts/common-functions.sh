###############################################################################
# common-functions.sh - This script is sourced by networked and non-networked
# install scripts.
###############################################################################

export LOGFILE="/var/log/apigee-drupal-install.log"
# initialize empty logfile
echo -n "" > $LOGFILE 2>&1 || ( echo "Log file ${LOGFILE} is not writable. Cannot proceed."; exit 1 )

# Get the date of script running
export SCRIPT_RUNDATE="$(date '+%Y-%m-%d-%H.%M.%S')"
export SCRIPT_TEMP_DIR=${SCRIPT_PATH}/tmp
mkdir -p $SCRIPT_TEMP_DIR

export DRUPAL_WEBROOT="/var/www/html"
export HTTPD_CONF_DIR="/etc/httpd"
export HTTPD_VHOST_DIR_NAME="vhosts"


# Turn of case sensitive matching for our string compares
shopt -s nocasematch

function cleanup_noerror() {

  # Call function to remove tmp directory
  remove_tmp_dir

  display_header "

  GREAT SUCCESS! You're good to go.

  Install directory: ${DRUPAL_WEBROOT}
  Database connection settings: ${DRUPAL_WEBROOT}/sites/default/settings.php
  Database name: ${PORTAL_DB_NAME}
  Database user: ${PORTAL_DB_USERNAME}

  The actions of this installer are written to a log here:
    ${LOGFILE}
  If you need support during this installation, please include the logfile in
  your communication.
"

  exit 0
}


# Clean up function called if signal caught
function cleanup_error(){

  # Call function to remove tmp directory
  remove_tmp_dir

  display_error " ===> Exiting, ERROR!
 The actions of this installer are written to a log here: 
   ${LOGFILE}
 If you need support during this installation, please include the logfile in
 your communication.
 Here are the last few lines of the logfile for your convenience:"
  tail -n 20 $LOGFILE
  
  display_header ""
  exit 1

}

function cleanup_ctlc() {
  # Call function to remove tmp directory
  remove_tmp_dir
  exit 1
}

# Remove tmp directory when exiting
function remove_tmp_dir() {
  # Remove tmp directory
  if [[ -d $SCRIPT_TEMP_DIR ]]; then
    rm -rf $SCRIPT_TEMP_DIR
  fi
}

# Display a multiline string, because we can't rely on `echo` to do the right thing.
#
# Arguments:
# 1. Text to display.
display() {
  display_nonewline "${1?}\n"
}

# Display a multiline string without a trailing newline.
#
# Arguments:
# 1. Text to display.
display_nonewline() {
  printf -- "${1?}"
}

# Display a newline
display_newline() {
  display ''
}

display_header() {
  display_separator
  display "${1?}"
  display_separator
}

# Display an error message to STDERR, but do not exit.
#
# Arguments:
# 1. Message to display.
display_error() {
  printf "\e[31m !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n"
  printf "\e[31m ERROR: ${1?}\n\n"
  printf "\e[31m !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n"
  printf "\e[0m"
}

# Display a minor separator line.
display_separator() {
  display "*******************************************************************************"
}

# Invoke the exception_handler on CTRL-C or "set -e" errors.
register_exception_handler() {
  trap cleanup_error ERR
  trap cleanup_noerror TERM HUP
  trap cleanup_ctlc SIGINT
}

# Display a step in the installation process.
#
# Arguments:
# 1. Description of the step, e.g. "PERFORM INSTALLATION"
# 2. Display newline afterwards? Defaults to 'y'.
display_step() {
  t_display_step__description="${1?}"
  t_display_step__newline="${2:-"y"}"

  if [ -z "${DISPLAY_STEP__NUMBER:-""}" ]; then
    DISPLAY_STEP__NUMBER=1
  else
    DISPLAY_STEP__NUMBER=$(( 1 + ${DISPLAY_STEP__NUMBER?} ))
  fi

  display_newline
  display_separator
  display_newline
  display "STEP ${DISPLAY_STEP__NUMBER?}: ${t_display_step__description?}"

  if [ y = "${t_display_step__newline?}" ]; then
    display_newline
  fi
  display_separator
}

# Display a question, make the user answer it, and set a variable with their answer.
#
# Arguments:
# 1. Question text to display, e.g. "What's your favorite color?"
# 2. Name of the variable to export, e.g. "q_favorite_color"
# 3. Kind of question, e.g. "Yn" to show a 'Y/n' prompt that defaults to 'yes', "yN" to show a y/N prompt that defaults to 'no', "String" for a mandatory string response, "StringOrBlank" for an optional string response.
# 4. Default answer, optional. Supported for "String" questions.
question() {
  question_question="${1?}"
  question_name="${2?}"
  question_kind="${3?}"
  question_default="${4:-""}"

  question_message="?? ${question_question?} "
  case "${question_kind?}" in
    Yn)
      question_message="${question_message?}[Y/n] "
      ;;
    StringOrBlank)
      question_message="${question_message?}[Default: (blank)] "
      ;;
    String*)
      if [ ! -z "${question_default?}" ]; then
        question_message="${question_message?}[Default: ${question_default?}] "
      fi
      ;;
    Port)
      if [ ! -z "${question_default?}" ]; then
        question_message="${question_message?}[Default: ${question_default?}] "
      fi
      ;;
    *)
      display_failure "Invalid question kind: ${question_kind?}"
      ;;
  esac

  # Try to load the answer from an existing variable, e.g. given name "q" look at variable "$q".
  eval question_answered=\$"${question_name:-""}"
  question_defined=0
  question_success=n
  until [ y = "${question_success?}" ]; do
    echo "${question_message?}" || display 0
    display_nonewline " "

    read question_response

    case "${question_kind?}" in
      Yn)
        if [ -z "${question_response?}" -o y = "${question_response?}" -o Y = "${question_response?}" ]; then
          question_answer=y
          question_success=y
        elif [ n = "${question_response?}" -o N = "${question_response?}" ]; then
          question_answer=n
          question_success=y
        else
          display_error 'Answer must be either "y", "n" or <ENTER> for "y"'
        fi
        ;;
      String)
        if [ -z "${question_response?}" -a ! -z "${question_default?}" ]; then
          question_answer="${question_default?}"
          question_success=y
        elif [ ! -z ${question_response?} ]; then
          question_answer="${question_response?}"
          question_success=y
        else
          display_error 'Answer must be a string'
        fi
        ;;
      StringOrBlank)
        question_answer="${question_response?}"
        question_success=y
        ;;
      Port)
        if [ -z "${question_response?}" -a ! -z "${question_default?}" ]; then
          question_answer="${question_default?}"
          question_success=y
        else
          if [ ${question_response?} -gt 0 -a ${question_response?} -lt 65536 2>/dev/null ]; then
            question_answer="${question_response?}"
            question_success=y
          else
            display_error 'Answer must be a valid port number in the range 1-65535'
          fi
        fi
        ;;
      *)
        ;;
    esac

  done
  eval "${question_name?}='${question_answer?}'"
}

# Make sure script is run a root.
if [ "$(id -u)" != "0" ]; then
  display_error "This script must be run as root" 
  exit 1
fi
# Make sure we can check if system is RHEL or CENTOS
if [[ ! -f /etc/redhat-release ]] ; then
  display_error "The server does not have a /etc/redhat-release file; cannot determine OS type."
  exit 1
fi

# Variables set in Determine System Step
export PLATFORM_NAME=
export PLATFORM_MAJOR_VERSION=
export PLATFORM_ARCHITECTURE=

# Determine RHEL or CentOS
if [[ -f /etc/system-release && `cat /etc/system-release | cut -d " " -f1` == 'CentOS' ]] ; then
  PLATFORM_NAME="CentOS"
  PLATFORM_MAJOR_VERSION=`cat /etc/system-release | cut -d " " -f3 | cut -d. -f1`
elif [[ -f /etc/redhat-release && `cat /etc/redhat-release | cut -d " " -f1` == "Red" ]] ; then
  PLATFORM_NAME="Redhat"
  PLATFORM_MAJOR_VERSION=`cat /etc/redhat-release | cut -d " " -f7 | cut -d. -f1`
else
  display_error "The Server is not running Redhat or CentOS Linux. Only Redhat and CentOS Linux is supported at this time."
  exit 1
fi
PLATFORM_ARCHITECTURE=`uname -i`

# The following will disable apachesolr modules for OPDK builds
export OPDK_BUILD="yes"

export RPM_LOCAL_PATH=${SCRIPT_PATH}/bundle/devportal-repo


