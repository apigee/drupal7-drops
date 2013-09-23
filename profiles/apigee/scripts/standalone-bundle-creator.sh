#!/bin/bash

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
  # If rhn_check comes back w/error, the system is not registered.
  if [ ! -f /etc/sysconfig/rhn/systemid ] ; then
    echo "The server is not registered with the RedHat network."
    echo "Please register your system with RHN using the rhn_register command"
    echo "and restart this script."
    exit 1
  fi
  # Make sure the RHN server-optional channel is registered
  if [ `rhn-channel -l | grep -c 'server-optional'` -eq 0 ] ; then
    echo "The server is not registered to the server-optional channel. Please register"
    echo "the server-optional RHN channel restart this script."
    echo "You can register the server-optional channel by using the following command:"
    echo
    echo "  rhn-channel --add --channel=<channel-name> --user=<rhn-username> --password=<rhn-password>"
    echo
    echo "To find the <channel-name>, use the following command:"
    echo
    echo "  rhn-channel -L -u <rhn-username> -p <rhn-password> | grep server-optional"
    echo
    exit 1
  fi
fi

yum install -y wget

mkdir -p bundle/devportal-repo

# Install EPEL Repo if needed.
if [ `rpm -qa  | grep -c 'epel-release'` -eq 0 ] ; then
  echo "Installing EPEL Repo..."
  wget --quiet -r -A "epel-release-*.rpm" --level=1 --no-directories --no-parent http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE}
  rpm -ivh ./epel-release-*.rpm
  yum clean all
  mv epel-release-*.rpm bundle/devportal-repo/
fi

# Install IUS Community Repo if needed.
if [ `rpm -qa  | grep -c 'ius-release'` -eq 0 ] ; then
  echo "Installing IUS Community Repo..."
  wget --quiet -r -A "ius-release-*.rpm" --level=1 --no-directories --no-parent http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE}
  rpm -ivh ./ius-release-*.rpm
  yum clean all
  mv ius-release-*.rpm bundle/devportal-repo/
fi

rpm -q yum-downloadonly 2>&1 > /dev/null && yum_verb="update" || yum_verb="install"
yum $yum_verb -y yum-downloadonly

# The following list of RPMs was a diff of rpm -qa before and after installing
# all needed packages on a blank-slate CentOS system.
echo "Downloading RPMs"
yum --downloadonly --downloaddir=bundle/devportal-repo -y install \
  apr apr-util apr-util-ldap atk autoconf automake avahi-libs bzip2-devel \
  cairo cloog-ppl ConsoleKit ConsoleKit-libs cpp cups-libs dbus eggdbus \
  fontconfig fontconfig-devel freetype freetype-devel gcc GConf2 gd \
  ghostscript ghostscript-devel ghostscript-fonts glibc-devel glibc-headers \
  gnutls gtk2 hicolor-icon-theme httpd httpd-tools ImageMagick ImageMagick-devel \
  jasper-devel jasper-libs kernel-headers keyutils-libs-devel krb5-libs \
  lcms-devel lcms-libs libcom_err-devel libcroco libfontenc libgomp \
  libgsf libICE libICE-devel libIDL libjpeg-turbo libjpeg-turbo-devel libmcrypt \
  libpng libpng-devel librsvg2 libselinux-devel libsepol libSM libSM-devel \
  libtasn1 libthai libtidy libtiff libtiff-devel libtool-ltdl libwmf-lite \
  libX11 libX11-common libX11-devel libXau libXau-devel libxcb libxcb-devel \
  libXcomposite libXcursor libXdamage libXext libXext-devel libXfixes libXfont \
  libXft libXi libXinerama libXpm libXpm-devel libXrandr libXrender libxslt \
  libXt libXt-devel mailcap make mpfr mysql mysql-libs mysql-server \
  openssl-devel ORBit2 pango perl perl-DBD-MySQL perl-DBI perl-libs \
  perl-Module-Pluggable perl-Pod-Escapes perl-Pod-Simple perl-version \
  php54 php54-cli php54-common php54-devel php54-gd php54-mbstring php54-mcrypt \
  php54-mysql php54-pdo php54-pear php54-pecl-apc php54-pecl-imagick php54-process \
  php54-tidy php54-xml php54-xmlrpc pixman pkgconfig polkit ppl sgml-common \
  t1lib urw-fonts xorg-x11-font-utils xorg-x11-proto-devel zlib-devel

echo "Locally installing some needed tools"
yum install -y git php54 php54-pear createrepo

echo "Creating repo"
createrepo bundle/devportal-repo/

echo "Downloading Drupal install"
#FIXME: This fails because it's a private repo
git clone git://github.com/apigee/drupal7-drops.git bundle/drupal

echo "Downloading Drush and friends"
mkdir bundle/drush
pear channel-discover pear.drush.org
pear channel-update pear.drush.org
cd bundle/drush
pear download drush/drush
wget http://pear.drush.org/channel.xml

# Find current version of registry_rebuild
rr_current=`curl -s https://drupal.org/project/registry_rebuild | grep "ftp.drupal.org" | grep -v -- "-dev" | sed -E 's:.*(registry_rebuild-7.x-[0-9.]+.tar.gz).*:\1:g' | head -n 1`
wget http://ftp.drupal.org/files/projects/${rr_current}

cd ../..

echo "Creating tarball"
tar --exclude='.git'  --exclude='.gitignore' --exclude='*.DS_Store' -cjvf apigee-drupal-install-bundle.tbz bundle
echo "Created tarball at `pwd`/apigee-drupal-install-bundle.tbz"
