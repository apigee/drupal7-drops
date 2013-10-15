#!/bin/bash

###############################################################################
# networked-install.sh - This script is used to install Dev Portal on either
# CentOS or Redhat Enterprise Linux 6.x.
###############################################################################


# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

export SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

source ${SCRIPT_PATH}/common-functions.sh

display_header "

 This script will install a default Apigee Portal on this server.

"
question "Press ENTER to continue..." DISCARD_ME StringOrBlank

source ${SCRIPT_PATH}/tasks/download-apigee-rpm.sh

register_exception_handler

source ${SCRIPT_PATH}/tasks/validate-network-availability.sh

# If RHEL, check to make sure system is registered with RHN
if [[ $PLATFORM_NAME == "Redhat" ]]; then
  display_step "Validating Yum Repositories"
  source ${SCRIPT_PATH}/tasks/validate-rhn-repos.sh
fi

source ${SCRIPT_PATH}/tasks/install-epel-repo.sh
source ${SCRIPT_PATH}/tasks/install-required-packages.sh
source ${SCRIPT_PATH}/tasks/set-php-timezone.sh
source ${SCRIPT_PATH}/tasks/install-mysql.sh
source ${SCRIPT_PATH}/tasks/download-drush.sh
source ${SCRIPT_PATH}/tasks/install-apigee-rpm.sh
source ${SCRIPT_PATH}/tasks/configure-selinux.sh
source ${SCRIPT_PATH}/tasks/configure-vhost.sh

## -----------------------------------------------------
## STEP: Run apigee profile installer
## -----------------------------------------------------
display_step "Dev Portal Drupal configuration"

question "Would you like to run the install/configure process for your portal now?
(If you have already installed and configured a portal for this database server,
choose No.)" INSTALL_PROFILE Yn

if [[ $INSTALL_PROFILE = "Y" ]] ; then
  source ${SCRIPT_PATH}/tasks/configure-apigee-profile.sh
fi

cleanup_noerror
