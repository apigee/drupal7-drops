#!/bin/bash

# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

export SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

source ${SCRIPT_PATH}/common-functions.sh
source ${SCRIPT_PATH}/tasks/validate-network-availability.sh

WHEREAMI=`pwd`

mkdir -p ${WHEREAMI}/bundle/devportal-repo

source ${SCRIPT_PATH}/tasks/validate-rhn-repos.sh
source ${SCRIPT_PATH}/tasks/install-epel-repo.sh
source ${SCRIPT_PATH}/tasks/download-apigee-rpm.sh

rpm -q yum-downloadonly 2>&1 > /dev/null && yum_verb="update" || yum_verb="install"
display_step "Installing/updating yum-downloadonly"
yum $yum_verb -y yum-downloadonly >> $LOGFILE 2>&1

# The following list of RPMs was a diff of rpm -qa before and after installing
# all needed packages on a blank-slate CentOS system.
display_step "Downloading RPMs"
yum --downloadonly --downloaddir=${WHEREAMI}/bundle/devportal-repo -y install \
  apr apr-util apr-util-ldap autoconf automake cloog-ppl cpp freetype gcc \
  glibc glibc-common glibc-devel glibc-headers httpd httpd-tools kernel-headers \
  libgomp libjpeg-turbo libmcrypt libpng libX11 libX11-common libXau libxcb \
  libXpm libxslt mailcap make mpfr mysql mysql-libs mysql-server \
  pcre-devel perl perl-DBD-MySQL perl-DBI perl-libs perl-Module-Pluggable \
  perl-Pod-Escapes perl-Pod-Simple perl-version php php-cli php-common \
  php-devel php-gd php-mbstring php-mcrypt php-mysql php-pdo php-pear \
  php-pecl-apc php-xml pkgconfig ppl wget >> $LOGFILE 2>&1

display_step "Locally installing some needed tools"
yum install -y php php-pear createrepo >> $LOGFILE 2>&1

display_step "Creating repo"
createrepo ${WHEREAMI}/bundle/devportal-repo/ >> $LOGFILE 2>&1

source ${SCRIPT_PATH}/tasks/download-drush.sh

# Bundle up all drush- and pear-related files
display_step "Creating drush/pear tarball"
cd /
test -f /root/.pearrc || touch /root/.pearrc
tar cf ${WHEREAMI}/bundle/pear-files.tar /usr/bin/drush /usr/share/pear /root/.drush /root/.pearrc
cd ${WHEREAMI}

SYSTEM_ARCHITECTURE="$( echo "${PLATFORM_NAME}-${PLATFORM_MAJOR_VERSION}-${PLATFORM_ARCHITECTURE}" | tr '[A-Z]' '[a-z]' )"
echo -n $SYSTEM_ARCHITECTURE > bundle/system-architecture

tarball_name="apigee-drupal-install-bundle-${SYSTEM_ARCHITECTURE}.tbz"
display_step "Creating tarball"
tar --exclude='.git'  --exclude='.gitignore' --exclude='*.DS_Store' -cjf $tarball_name bundle >> $LOGFILE 2>&1
display "Created tarball at `pwd`/${tarball_name}"

if [[ $PLATFORM_NAME = "Redhat" ]] ; then
  display "The target system must be RHEL ${PLATFORM_MAJOR_VERSION} (${PLATFORM_ARCHITECTURE})."
  display "It must be registered on the Red Hat Network and registered to the 'server-optional' RHN channel."
else
  display "The target system must be CentOS ${PLATFORM_MAJOR_VERSION} (${PLATFORM_ARCHITECTURE})."
fi