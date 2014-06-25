#!/bin/bash

# --------------------------------------------------------------------------------------------------
# Beg Checks
# --------------------------------------------------------------------------------------------------
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
  PLATFORM_MAJOR_RELEASE=`cat /etc/system-release | cut -d " " -f3 | cut -d. -f2`
  if [[ ! $PLATFORM_MAJOR_VERSION == 6 || ! $PLATFORM_MAJOR_RELEASE == 5 ]] ; then
    echo "The Server is not running CentOS Linux 6.5."
    exit 1
  fi
elif [[ -f /etc/redhat-release && `cat /etc/redhat-release | cut -d " " -f1` == "Red" ]] ; then
  PLATFORM_NAME="Redhat"
  PLATFORM_MAJOR_VERSION=`cat /etc/redhat-release | cut -d " " -f7 | cut -d. -f1`
  PLATFORM_MAJOR_RELEASE=`cat /etc/redhat-release | cut -d " " -f7 | cut -d. -f2`
  if [[ ! $PLATFORM_MAJOR_VERSION == 6 || $PLATFORM_MAJOR_RELEASE < 4 ]] ; then
    echo "The Server is not running Redhat 6.4 or 6.5."
    exit 1
  fi
else
  echo "The Server is not running Redhat or CentOS Linux."
  echo "Only Redhat and CentOS Linux are supported at this time."
  exit 1
fi

# Start logging into log file...
echo -n "" > $LOGFILE

# Determine if network connect exist
if ! curl --connect-timeout 5 -f -s -L -X HEAD -H "Connection: Close" http://apigee.com/about/ ; then
   echo "External network not available - installing rpms from local repository." >> $LOGFILE 2>&1
   echo "External network not available - installing rpms from local repository." 1>&2
   HAS_NETWORK=0
else
   echo "Network connection found - downloading rpms from network." >> $LOGFILE 2>&1
   echo "Network connection found - downloading rpms from network." 1>&2
   HAS_NETWORK=1
fi

## If RHEL, check to make sure system is registered with RHN
#if [[ $PLATFORM_NAME == "Redhat" ]]; then
#  # Check if Redhat is Not Registered
#  if [[ `subscription-manager list | grep "Status:\s*Not Subscribed"` ]] ; then
#    echo "The server is not registered with the RedHat network."
#    echo "Please register your system with RHN using the command and restart this script:"
#    echo
#    echo  "  subscription-manager register --username=my_username --password=my_password --auto-attach"
#    echo
#    exit 1
#  fi
#
##  # Make sure the RHN server-optional repo exist -- SCRIPT WILL INSTALL LATER
##  if [[ -z "$(yum repolist | grep 'rhel-6-server-optional-rpms \s*')" ]] ; then
##    echo "The server optional rpm has not beed added to the server."
##    echo "Add the server optional rpm using the following commands:"
##    echo
##    echo "  yum install yum-utils"
##    echo "  yum-config-manager --enable rhel-6-server-optional-rpms"
##    echo
##    exit 1
##  fi
#fi

