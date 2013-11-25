#!/bin/bash
if [[ $HAVE_COMMON_FUNCTIONS -ne 1 ]]; then
# Get directory this script is running in
  SOURCE="${BASH_SOURCE[0]}"
  while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
  done
  export SCRIPT_PATH="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  source ${SCRIPT_PATH}/../common-functions.sh
fi

CURRENT_COMMONS_LOGGING_RELEASE="1.1.3"
COMMONS_LOGGING_FILENAME="commons-logging-${CURRENT_COMMONS_LOGGING_RELEASE}-bin.tar.gz"

CURRENT_SLF4J_RELEASE="1.7.5"
SLF4J_FILENAME="slf4j-${CURRENT_SLF4J_RELEASE}.tar.gz"

CURRENT_SOLR_RELEASE="4.5.1"
SOLR_FILENAME="solr-${CURRENT_SOLR_RELEASE}.tgz"

DRUPAL_APACHESOLR_RELEASE="7.x-1.5"
DRUPAL_APACHESOLR_FILENAME="apachesolr-${DRUPAL_APACHESOLR_RELEASE}.tar.gz"

display_step "Downloading needed resources"
mkdir /tmp/solr-tarballs
cd /tmp/solr-tarballs
curl -L -O http://apache.osuosl.org/commons/logging/binaries/${COMMONS_LOGGING_FILENAME}
curl -L -O http://www.slf4j.org/dist/${SLF4J_FILENAME}
curl -L -O http://apache.osuosl.org/lucene/solr/${CURRENT_SOLR_RELEASE}/${SOLR_FILENAME}
curl -L -O http://ftp.drupal.org/files/projects/${DRUPAL_APACHESOLR_FILENAME}
cd $SCRIPT_PATH

display_step "Installing Tomcat"
yum -y install tomcat6 tomcat6-webapps tomcat6-admin-webapps
chkconfig tomcat6 on
# Not sure if the next line is actually needed, but it does no harm.
sed -i -e 's:<tomcat-users>:<tomcat-users><role rolename="manager" /><role rolename="admin" /><user username="SolrUser" password="SolrPass" roles="manager,admin" />:g' /etc/tomcat6/tomcat-users.xml
service tomcat6 start

# yum install jakarta-commons-logging does not work. Must install from tarball.
display_step "Installing Apache Commons Logging"
cd /tmp
tar xzf solr-tarballs/$COMMONS_LOGGING_FILENAME
cp commons-logging-${CURRENT_COMMONS_LOGGING_RELEASE}/commons-logging-*.jar /usr/share/tomcat6/lib
#rm -r commons-logging-${CURRENT_COMMONS_LOGGING_RELEASE}
cd $SCRIPT_PATH

# yum install slf4j does not work. Must install from tarball.
display_step "Installing Simple Logging Facade for Java"
tar xzf solr-tarballs/$SLF4J_FILENAME
cp slf4j-${CURRENT_SLF4J_RELEASE}/slf4j-*.jar /usr/share/tomcat6/lib
#rm -r slf4j-${CURRENT_SLF4J_RELEASE}
cd $SCRIPT_PATH

display_step "Installing and configuring Apache Solr"
cd /tmp
tar xzf solr-tarballs/$SOLR_FILENAME
tar xzf solr-tarballs/$DRUPAL_APACHESOLR_FILENAME
cp solr-${CURRENT_SOLR_RELEASE}/dist/solr-${CURRENT_SOLR_RELEASE}.war /usr/share/tomcat6/webapps/solr.war
mkdir /home/solr
cp -R solr-${CURRENT_SOLR_RELEASE}/example/solr/* /home/solr
# Pull in Drupal schema configuration
cp apachesolr/solr-conf/solr-4.x/* /home/solr/collection1/conf
#rm -r solr-${CURRENT_SOLR_RELEASE} apachesolr
cd $SCRIPT_PATH

chown -R tomcat /home/solr
# Need to restart tomcat to get it to unpack the war
service tomcat6 restart

# Now mangle a file that the war unpacked
sed -i -e 's:<filter>:<env-entry><env-entry-name>solr/home</env-entry-name><env-entry-value>/home/solr</env-entry-value><env-entry-type>java.lang.String></env-entry-type></env-entry><filter>:g' /usr/share/tomcat6/webapps/solr/WEB-INF/web.xml
service tomcat6 restart

# Verify that we're golden
curl http://localhost:8080/solr/ > /dev/null 2>&1 || ( echo "Apache Solr failed to respond."; exit )

# Perform Drupal config if appropriate
if [[ -f ${DRUPAL_WEBROOT}/sites/default/settings.php ]]; then
  display_step "Configuring Drupal to use apachesolr"
  cd ${DRUPAL_WEBROOT}/sites/default
  drush pm-list --status=enabled --package="Search Toolkit,Pantheon" > /tmp/drupal-search-packages
  if [[ `grep -c '\(pantheon_apachesolr\)' /tmp/drupal-search-packages` -eq 1 ]]; then
    drush --yes dis pantheon_apachesolr
  fi
  if [[ `grep -c '\(apachesolr\)' /tmp/drupal-search-packages` -eq 0 ]]; then
    drush --yes en apachesolr
  fi
  if [[ `grep -c '\(apachesolr_search\)' /tmp/drupal-search-packages` -eq 0 ]]; then
    drush --yes en apachesolr_search
  fi  
  
  drush solr-set-env-url http://localhost:8080/solr
  display_step "Reindexing all Drupal content"
  drush solr-delete-index
  drush solr-index
fi

rm -rf /tmp/solr-tarballs
