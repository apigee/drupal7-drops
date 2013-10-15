has_valid_settings="N";
db_on_localhost="N";
if [ -f /tmp/settings.php ]; then
  mv /tmp/settings.php /tmp/settings.php~
fi

# Set default values for portal DB
export PORTAL_DB_PASSWORD=`openssl rand -base64 12 | sed -e "s/[^0-9a-zA-Z]//g"`
export PORTAL_DB_RANDUSER=`openssl rand -base64 8 | sed -e "s/[^0-9a-zA-Z]//g"`
export PORTAL_DB_USERNAME="user-${PORTAL_DB_RANDUSER}"
export PORTAL_DB_NAME="devportal"
export PORTAL_DB_HOSTNAME="localhost"
export PORTAL_DB_PORT=3306

if [ -f ${DRUPAL_WEBROOT}/sites/default/settings.php ] ; then
  PORTAL_DB_NAME=`grep "'database'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_USERNAME=`grep "'username'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_PASSWORD=`grep "'password'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_HOSTNAME=`grep "'host'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  PORTAL_DB_PORT=`grep "'port'" ${DRUPAL_WEBROOT}/sites/default/settings.php | cut -d "'" -f4`
  if [[ -n $PORTAL_DB_NAME && -n $PORTAL_DB_USERNAME && -n $PORTAL_DB_PASSWORD && -n $PORTAL_DB_HOSTNAME && -n $PORTAL_DB_PORT ]] ; then
    mysql -u $PORTAL_DB_USERNAME -p${PORTAL_DB_PASSWORD} -h $PORTAL_DB_HOSTNAME -P $PORTAL_DB_PORT -D $PORTAL_DB_NAME -e 'SHOW TABLES' > /dev/null 2>&1 && has_valid_settings="y"
  fi
  if [[ $has_valid_settings = "Y" ]]; then
    question "A valid settings.php file was already found. Do you want to keep these settings?" has_valid_settings Yn
    if [[ $has_valid_settings = "Y" ]]; then
      # Make sure apigee-drupal-*.rpm doesn't trample on this
      cp ${DRUPAL_WEBROOT}/sites/default/settings.php /tmp
      if [[ $PORTAL_DB_HOSTNAME = 'localhost' || $PORTAL_DB_HOSTNAME = '127.0.0.1' ]]; then
        db_on_localhost="Y"
      fi
    fi
  fi
fi

if [[ $has_valid_settings == "Y" ]] ; then
  INSTALL_MYSQL_SERVER="N"
else
  # -----------------------------------------------------
  # STEP: Install MySQL Database
  # -----------------------------------------------------
  display_step "MySQL Server"
  # Would you like the MYSQL Server installed on this server?
  display_header "

The Dev Portal database can be installed in a local MySQL database or
connect to a remote server.  If you are installing the server on a remote
machine, you will need to supply the MySQL username, password, and the server
hostname later in this script.  Please set up the MySQL server on the database
server and have this information available before continuing.
"
  question "Would you like to install MySQL Server on this system?" INSTALL_MYSQL_SERVER Yn

  if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then
    display_step "Installing MySQL Server"
    if [[ $OPDK_STANDALONE -eq 1 ]]; then
      yum --disablerepo=\* --enablerepo=devportal -y --nogpgcheck install mysql-server
    else
      yum install -y mysql-server
    fi
  fi
fi

# -----------------------------------------------------
# STEP: Set MySQL to run at startup
# -----------------------------------------------------
if [[ $INSTALL_MYSQL_SERVER == "Y" || $db_on_localhost == "Y" ]]; then
  display_step "Making sure MySQL runs at startup"
  chkconfig --levels 35 mysqld on >> $LOGFILE 2>&1
  service mysqld start
fi

# -----------------------------------------------------
# STEP: Database Configuration
# -----------------------------------------------------
display_step "Database Configuration"

if [[ $INSTALL_MYSQL_SERVER == "Y" ]]; then

  question "What is the MySQL database name?" PORTAL_DB_NAME String $PORTAL_DB_NAME
  question "Enter a MySQL database username Dev Portal should connect with" PORTAL_DB_USERNAME String $PORTAL_DB_USERNAME
  question "Enter a MySQL password for the user ${PORTAL_DB_USERNAME}" PORTAL_DB_PASSWORD String $PORTAL_DB_PASSWORD

  # Make sure database is properly empty. Note that user-to-db associations
  # remain in effect as long as the new db has the same name as the old one.
  mysql -u root -e "DROP DATABASE IF EXISTS ${PORTAL_DB_NAME}";
  mysql -u root -e "CREATE DATABASE ${PORTAL_DB_NAME}";

  # Check to see if user exists
  IS_USER_CREATED=`mysql -u root --skip-column-names  -e "select count(*) from mysql.user where user='devportal' and host='localhost'"`

  if [[ $IS_USER_CREATED -eq 0 ]]; then
    display "Creating MySQL user ${PORTAL_DB_USERNAME}..."
    mysql -u root  -e "CREATE USER '${PORTAL_DB_USERNAME}'@'localhost' IDENTIFIED BY '${PORTAL_DB_PASSWORD}';"
    mysql -u root  -e "GRANT ALL ON ${PORTAL_DB_NAME}.* TO '${PORTAL_DB_USERNAME}'@'localhost';"
    mysql -u root  -e 'FLUSH PRIVILEGES;'
  else
    display "MySQL user ${PORTAL_DB_USERNAME} already exists, updating password."
    mysql -u root  -e "SET PASSWORD FOR '${PORTAL_DB_USERNAME}'@'localhost' = PASSWORD('${PORTAL_DB_PASSWORD}');"
  fi
elif [[ $has_valid_settings != 'y' ]]; then
  display_header "

  MySQL server has not been installed, so you will need to supply the connection information. The
  database user and database must already be created. The database should also be empty.

  Please make sure the database user has the following rights:
    SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, CREATE TEMPORARY TABLES, LOCK TABLES.

"

  # Keep looping until user can connect
  IS_VALID_MYSQL_USER=0
  while [[ $IS_VALID_MYSQL_USER -eq 0 ]]; do
    question "What is the MySQL database host?" PORTAL_DB_HOSTNAME String $PORTAL_DB_HOSTNAME
    question "What is the MySQL database port?" PORTAL_DB_PORT Port $PORTAL_DB_PORT
    question "What is the MySQL database name?" PORTAL_DB_NAME String $PORTAL_DB_NAME
    question "What is the MySQL database username Dev Portal should connect with?" PORTAL_DB_USERNAME String $PORTAL_DB_USERNAME
    question "Enter the MySQL password for the user ${PORTAL_DB_USERNAME}" PORTAL_DB_PASSWORD String $PORTAL_DB_PASSWORD

    # Turn off error handling
    trap '' ERR
    trap '' TERM HUP

    SQL_RESULT=`mysql -u ${PORTAL_DB_USERNAME} -p${PORTAL_DB_PASSWORD} -h ${PORTAL_DB_HOSTNAME} -P ${PORTAL_DB_PORT} --skip-column-names -e "SHOW DATABASES LIKE '${PORTAL_DB_NAME}'"` >> $LOGFILE 2>&1

    # Turn error handling back on
    register_exception_handler

    if [[ "${SQL_RESULT}" == "${PORTAL_DB_NAME}" ]]; then
      display "User $PORTAL_DB_ROOT_USER_NAME can connect to MySQL server."
      IS_VALID_MYSQL_USER=1
    else
      display_error "User $PORTAL_DB_ROOT_USER_NAME cannot connect to MySQL server."
      IS_VALID_MYSQL_USER=0
    fi
  done
fi
