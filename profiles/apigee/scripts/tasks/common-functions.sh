#!/bin/bash

###############################################################################################
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
        if [ -z "${question_response?}" ] ; then
          display_error 'Answer must be either "y" or "n"'
        elif [ q = "${question_response?}" -o Q = "${question_response?}" ]; then
          display_nonewline " "
          exit 1
        elif [ y = "${question_response?}" -o Y = "${question_response?}" ]; then
          question_answer=y
          question_success=y
        elif [ n = "${question_response?}" -o N = "${question_response?}" ]; then
          question_answer=n
          question_success=y
        else
          display_error 'Answer must be either "y" or "n"'
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

# Turn of case sensitive matching for our string compares
shopt -s nocasematch
