#!/bin/bash

###############################################################################
# standalone-bundle-installer.sh - This script is used to install Dev Portal on
# either CentOS or Redhat Enterprise Linux 6.x without a network connection.
###############################################################################

if [ "$(id -u)" != "0" ]; then
echo "This script must be run as root" 1>&2
   exit 1
fi

# This configuration MUST be filled out.
export DEVCONNECT_ENDPOINT="https://api.enterprise.apigee.com/v1"
export DEVCONNECT_ORG="my-org"
export DEVCONNECT_AUTH="<username>:<password>"

if [[ $DEVCONNECT_AUTH = '<username>:<password>' ]] ; then
  echo "***************************************************************************"
  echo "Please edit standalone-bundle-installer.sh and set your Apigee organization"
  echo "parameters. These settings are found beginning at line 14."
  echo "When this is complete, re-run standalone-bundle-installer.sh."
  echo "***************************************************************************"
  exit 1;
fi

# Get directory this script is running in
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

# Create tmp directory
SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
SCRIPT_TEMP_DIR=${SCRIPT_PATH}/tmp
mkdir -p $SCRIPT_TEMP_DIR
DRUPAL_WEBROOT="$( cd -P ${SCRIPT_PATH}/../../.. && pwd )"
BUNDLE_ROOT="$( cd -P ${DRUPAL_WEBROOT}/.. && pwd )"

export LOGFILE="${SCRIPT_TEMP_DIR}/apigee-drupal-install.log"
export DRUPAL_WEBAPP="/var/www/html"

export HTTPD_CONF_DIR="/etc/httpd"
export HTTPD_VHOST_DIR_NAME="vhosts"

export PORTAL_DB_PASSWORD=`openssl rand -base64 12 | sed -e "s/[^0-9a-zA-Z]//g"`
export PORTAL_DB_RANDUSER=`openssl rand -base64 8 | sed -e "s/[^0-9a-zA-Z]//g"`
export PORTAL_DB_USERNAME="user-${PORTAL_DB_RANDUSER}"
export PORTAL_DB_NAME="devportal"
export PORTAL_DB_HOSTNAME="localhost"
export PORTAL_DB_PORT=3306

echo -n "" > $LOGFILE

echo "*************************************************************************"
echo " This script will install a default Apigee Portal here:"
echo " $DRUPAL_WEBAPP"
echo "*************************************************************************"

# Clean up function called if signal caught
function cleanup_error(){

  echo "===> Exiting, ERROR!"
  
  echo "***********************************************************************"
  echo " The actions of this installer are written to a log here:"
  echo "   ${LOGFILE}"
  echo " If you need support during this installation, "
  echo " please include the logfile in your communication."
  echo "***********************************************************************"
  echo ""
  exit
}

function cleanup_noerror() {
  echo "***********************************************************************"
  echo " GREAT SUCCESS! you're good to go. "
  echo "***********************************************************************"
  
  echo "***********************************************************************"
  echo " To create a drupal user, cd to the site folder and use drush as below"
  echo " cd ${DRUPAL_WEBAPP}/sites/default"
  echo " drush user-create newuser --mail='my@email.com' --password='p@ssw0rd' "
  echo " drush user-add-role 'Administrator' --name='newuser' "
  echo " drush user-unblock newuser "
  echo " "
  echo " The actions of this installer are written to a log here:"
  echo "    ${LOGFILE}"
  echo " If you need support during this installation, "
  echo " please include the logfile in your communication."
  echo "***********************************************************************"
  echo ""
  exit
}

# Set signal trap to call above cleanup function
trap cleanup_error ERR
trap clearup_noerror TERM HUP

cat <<EOF > /etc/yum.repos.d/devportal.repo
[devportal]
name=Apigee Dev Portal Installation CDROM
baseurl=file://${BUNDLE_ROOT}/devportal-repo
enabled=0
EOF

echo "*************************************************************************"
echo " Installing Packages from repo folder (this may take a few minutes)..."
echo "*************************************************************************"

yum clean all >> $LOGFILE 2>&1


# base RPMs install
echo -n "Installing Apache and MySQL"
for i in httpd mysql mysql-server; do
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install $i >> $LOGFILE 2>&1
  echo -n "."
done
echo " done."

echo -n "Installing PHP and its modules"
for i in php54 php54-devel php54-mysql php54-pdo php54-mcrypt php54-mbstring php54-process php54-tidy php54-xmlrpc php54-xml php54-gd php54-pear; do
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install $i >> $LOGFILE 2>&1
  echo -n "." 
done
echo " done."

echo -n "Installing PECL dependencies"
for i in gd gd-devel openssl openssl-devel ImageMagick ImageMagick-devel; do
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install $i >> $LOGFILE 2>&1
  echo -n "." 
done
echo " done."

echo -n "Installing development tools"
for i in gcc make; do
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install $i >> $LOGFILE 2>&1
  echo -n "." 
done
echo " done."

echo -n "Installing PECL extensions"
for i in php54-pecl-apc php54-pecl-imagick; do
  yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install $i >> $LOGFILE 2>&1
  echo -n "."   
done
echo " done."

echo "Making sure MySQL and Apache run at startup"
chkconfig --levels 35 mysqld on >> $LOGFILE 2>&1
chkconfig --levels 35 httpd on >> $LOGFILE 2>&1

echo "Installing Drush"
pear channel-add drush/channel.xml >> $LOGFILE 2>&1
pear install "drush/drush-5.8.0.tgz" >> $LOGFILE 2>&1

echo "Copying the webapp folder into /var/www/html ... "
cp -R ${DRUPAL_WEBROOT}/{.*,*} $DRUPAL_WEBAPP >> $LOGFILE 2>&1
chown -R apache:apache $DRUPAL_WEBAPP >> $LOGFILE 2>&1
chmod -R 775 $DRUPAL_WEBAPP >> $LOGFILE 2>&1

HTTPD_VHOST_DIR_PATH=${HTTPD_CONF_DIR}/${HTTPD_VHOST_DIR_NAME}

# Create the vhost conf directory to add virtualhosts to
if [ ! -d $HTTPD_VHOST_DIR_PATH ] ; then
  display "Virtual Host config directory already exists: ${HTTPD_VHOST_DIR_PATH}"
  mkdir $HTTPD_VHOST_DIR_PATH
fi

IS_VHOST_HTTPD_INCLUDE_INSTALLED=`cat /etc/httpd/conf/httpd.conf | grep "^Include" | grep -c $HTTPD_VHOST_DIR_NAME`
# Add an include directive to httpd.conf for the vhost dir
if [ $IS_VHOST_HTTPD_INCLUDE_INSTALLED -eq 0 ]; then
  display "Adding an Include directive for ${HTTPD_VHOST_DIR_NAME}/*.conf virtual host files to httpd.conf"
  cp ${HTTPD_CONF_DIR}/httpd.conf ${HTTPD_CONF_DIR}/httpd.conf.orig
  echo "# Include ${HTTPD_VHOST_DIR_NAME}/*.conf for Dev Portal virtual hosts" >> ${HTTPD_CONF_DIR}/httpd.conf
  echo "Include ${HTTPD_VHOST_DIR_NAME}/*.conf" >> ${HTTPD_CONF_DIR}/httpd.conf
fi

echo "Installing VHOST config file for Dev Portal at: ${HTTPD_VHOST_DIR_PATH}"
cp ${SCRIPT_PATH}/resources/devportal.conf ${HTTPD_VHOST_DIR_PATH}
sed -i -e "s:_DEVPORTAL_INSTALL_DIR_:${DRUPAL_WEBAPP}:g" ${HTTPD_VHOST_DIR_PATH}/devportal.conf

echo "Starting Apache"
service httpd start >> $LOGFILE 2>&1
echo "Starting MySQL"
service mysqld start >> $LOGFILE 2>&1

echo "Creating empty database"
mysql -u root -e "CREATE DATABASE ${PORTAL_DB_NAME};"
mysql -u root -e "GRANT ALL ON ${PORTAL_DB_NAME}.* to '${PORTAL_DB_USERNAME}'@'localhost' IDENTIFIED BY '${PORTAL_DB_PASSWORD}';"
mysql -u root -e "FLUSH PRIVILEGES;"

display_step "Creating Dev Portal DB ${DRUPAL_WEBAPP}/sites/default/settings.php file..."

mkdir --mode=755 -p ${DRUPAL_WEBAPP}/sites/default/{public,tmp,private}

# Create drupal site config
(
echo '<?php'
echo '$databases = array('
echo "  'default' => array ("
echo "    'default' => array ("
echo "	    'database' => '${PORTAL_DB_NAME}',"
echo "	    'username' => '${PORTAL_DB_USERNAME}',"
echo "	    'password' => '${PORTAL_DB_PASSWORD}',"
echo "	    'host' => '${PORTAL_DB_HOSTNAME}',"
echo "	    'port' => '${PORTAL_DB_PORT}',"
echo "	    'driver' => 'mysql',"
echo "	    'prefix' => '',"
echo "	  ),"
echo "  ),"
echo ");"
echo '$update_free_access = FALSE;'
echo '$drupal_hash_salt = "";'
echo "ini_set('session.gc_probability', 1);"
echo "ini_set('session.gc_divisor', 100);"
echo "ini_set('session.gc_maxlifetime', 200000);"
echo "ini_set('session.cookie_lifetime', 2000000);"
) > "${DRUPAL_WEBROOT}/sites/default/settings.php"

echo "Installing Drush extensions"
mkdir -p ~/.drush
cd ~/.drush
tar xzf ${BUNDLE_ROOT}/drush/registry_rebuild*.tar.gz

display_step "Installing Drupal portal"
cd ${DRUPAL_WEBAPP}/sites/default

drush site-install apigee  apigee_install_api_endpoint.devconnect_org="${DEVCONNECT_ORG}" \
    apigee_install_api_endpoint.devconnect_endpoint="${DEVCONNECT_ENDPOINT}" \
    apigee_install_api_endpoint.devconnect_curlauth="${DEVCONNECT_AUTH}"

cd $SCRIPT_PATH

