#!/bin/bash

echo "Today date is: ${DD}"
date +%D -s 2014-04-30

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
      echo "Invalid question kind: ${question_kind?}"
      ;;
  esac

  # Try to load the answer from an existing variable, e.g. given name "q" look at variable "$q".
  eval question_answered=\$"${question_name:-""}"
  question_defined=0
  question_success=n
  until [ y = "${question_success?}" ]; do
    echo "${question_message?}"

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
          echo 'Answer must be either "y", "n" or <ENTER> for "y"'
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
          echo 'Answer must be a string'
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
            echo 'Answer must be a valid port number in the range 1-65535'
          fi
        fi
        ;;
      *)
        ;;
    esac

  done
  eval "${question_name?}='${question_answer?}'"
}

# --------------------------------------------------------------------------------------------------
# STEP: BEGAN CREATOR SCRIPT
# --------------------------------------------------------------------------------------------------
MY_CURDIR=`pwd`

if [ "$(id -u)" != "0" ]; then
echo "This script must be run as root" 1>&2
  exit 1
fi

# Make sure we can check if system is RHEL or CENTOS
if [[ ! -f /etc/redhat-release ]] ; then
  echo "The Server does not have a /etc/redhat-release file, cannot determine OS type."
  exit 1
fi

# Determine RHEL or CentOS
if [[ -f /etc/system-release && `cat /etc/system-release | cut -d " " -f1` == 'CentOS' ]] ; then
  PLATFORM_NAME="CentOS"
  PLATFORM_MAJOR_VERSION=`cat /etc/system-release | cut -d " " -f3 | cut -d. -f1`
elif [[ -f /etc/redhat-release && `cat /etc/redhat-release | cut -d " " -f1` == "Red" ]] ; then
  PLATFORM_NAME="Redhat"
  PLATFORM_MAJOR_VERSION=`cat /etc/redhat-release | cut -d " " -f7 | cut -d. -f1`
else
  echo "The Server is not running Redhat or CentOS Linux."
  echo "Only Redhat and CentOS Linux are supported at this time."
  exit 1
fi
PLATFORM_ARCHITECTURE=`uname -i`

# If RHEL, check to make sure system is registered with RHN
if [[ $PLATFORM_NAME == "Redhat" ]]; then
  # Check if Redhat is Not Registered
  if [[ `subscription-manager list | grep "Status:\s*Not Subscribed"` ]] ; then
    echo "The server is not registered with the RedHat network."
    echo "Please register your system with RHN using the command and restart this script:"
    echo
    echo  "  subscription-manager register --username=my_username --password=my_password --auto-attach"
    echo
    exit 1
  fi

  # Make sure the RHN server-optional repo exist -- SCRIPT WILL INSTALL LATER
#  if [[ -z "$(yum repolist | grep 'rhel-6-server-optional-rpms \s*')" ]] ; then
#    echo "The server optional rpm has not beed added to the server."
#    echo "Add the server optional rpm using the following command:"
#    echo
#    echo "  yum-config-manager --enable rhel-6-server-optional-rpms"
#    echo
#    exit 1
#  fi
fi

# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

mkdir -p bundle/devportal-repo

# Install EPEL Repo -- "download" in case it's required on a clean installs
echo "=====> Installing EPEL Repo..."
epel_url=http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE}/epel-release-6-5.noarch.rpm
curl -O $epel_url || ( echo "cannot download from $epel_url"; exit 1)

echo "====> Executing rpm -ivh command .... "
rpm -ivh ./epel-release-*.rpm

echo "clean yum"
yum clean all
mv epel-release-*.rpm bundle/devportal-repo/

echo "=====> Installing yum-downloadonly..."
rpm -q yum-downloadonly 2>&1 > /dev/null && yum_verb="update" || yum_verb="install"
yum $yum_verb -y yum-downloadonly

# The following list of RPMs was a diff of rpm -qa before and after installing
# all needed packages on a blank-slate CentOS system.
echo "Downloading RPMs..."
yum --downloadonly --downloaddir=bundle/devportal-repo -y install yum-utils

# Add the server optional channel by using the commands:
yum -y install yum-utils
yum-config-manager --enable rhel-6-server-optional-rpms

# Grab Apigee RPM
RPM_FILENAME="$( cd bundle/devportal-repo; shopt -s nullglob; echo apigee-drupal-*.rpm )"
if [ -z $RPM_FILENAME ] ; then
  downloaded_rpm=0
  echo "You need to download the Apigee Drupal RPM. Please ask your Apigee salesperson"
  echo "for the correct download URL. Supported protocols include http, https, ftp,"
  echo "ftps, sftp, scp, and file. If you have the RPM for the devportal, please put"
  echo "it in the following folder:"
  echo "  ${RPM_LOCAL_PATH}/apigee-drupal-*.rpm"
  echo "and then re-run this script."

  # curl handles all of the above protocols.
  while [ $downloaded_rpm -eq 0 ]; do
    question "Enter the download URL:" RPM_DOWNLOAD_URL String
    question "Enter username, if necessary:" RPM_DOWNLOAD_USER StringOrBlank
    if [ ! -z $RPM_DOWNLOAD_USER ] ; then
      question "Enter password:" RPM_DOWNLOAD_PASS String
    else
      RPM_DOWNLOAD_PASS=
    fi
    RPM_FILENAME="$( basename $RPM_DOWNLOAD_URL )"

    if [ -z $RPM_DOWNLOAD_PASS ] ; then
      curl -L -k -o bundle/devportal-repo/${RPM_FILENAME} $RPM_DOWNLOAD_URL && downloaded_rpm=1
    else
      curl -L -k -u "${RPM_DOWNLOAD_USER}:${RPM_DOWNLOAD_PASS}" -o bundle/devportal-repo/${RPM_FILENAME} $RPM_DOWNLOAD_URL && downloaded_rpm=1
    fi
    if [ $downloaded_rpm -eq 0 ] ; then
      echo "Sorry, the URL and/or credentials you gave were not correct; please try again."
    fi
  done
fi

BUILD_ID=$(grep -Po '(?<=apigee-drupal-).*(?=.noarch.rpm)' < <(echo ${RPM_FILENAME}))

yum --downloadonly --downloaddir=bundle/devportal-repo -y install bundle/devportal-repo/$RPM_FILENAME
yum --downloadonly --downloaddir=bundle/devportal-repo -y install php-drush-drush

yum --downloadonly --downloaddir=bundle/devportal-repo -y install openssl

######## Download MySQL RPMs ########
yum --downloadonly --downloaddir=bundle/devportal-repo -y install mysql mysql-server mysql-libs


question "About to create repo - add aditional rpms to bundle directory now..." CONTINUE_INSTALL Yn

echo "Locally installing some needed tools"
yum install -y php createrepo

echo "Creating repo"
createrepo bundle/devportal-repo/

# create list of required rpms - pre-requisites
cd `pwd`/bundle/devportal-repo
for f in mysql-*
do
  echo "$f" >> required_rpms.txt
done
cd ${MY_CURDIR}

# move out MYSQL and Drush rpms
mkdir ${MY_CURDIR}/other_rpms
mv ${MY_CURDIR}/bundle/devportal-repo/mysql-* ${MY_CURDIR}/other_rpms
mv ${MY_CURDIR}/bundle/devportal-repo/php-drush-* ${MY_CURDIR}/other_rpms

echo "Creating MAIN tarball"
#tar --exclude='mysql-.*' --exclude='.git'  --exclude='.gitignore' --exclude='*.DS_Store' -cjvf apigee-drupal-bundle-"${PLATFORM_NAME}"-mysql.tbz bundle
tar --exclude='.git'  --exclude='.gitignore' --exclude='*.DS_Store' -cjvf apigee-drupal-bundle-"${PLATFORM_NAME}"-"${BUILD_ID}".tbz bundle
echo "Created MAIN tarball at `pwd`/apigee-drupal-bundle.${PLATFORM_NAME}-${BUILD_ID}.tbz"

yum install -y ftp