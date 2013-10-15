# Install EPEL Repo if needed. This is required for php-mcrypt
if [[ `rpm -qa  | grep -c 'epel-release'` -eq 0 ]] ; then
  display_step "Installing EPEL Repo"
  epel_release_minor=5
  rpm_name="epel-release-${PLATFORM_MAJOR_VERSION}-${epel_release_minor}.noarch.rpm"
  epel_url="http://dl.iuscommunity.org/pub/ius/stable/${PLATFORM_NAME}/${PLATFORM_MAJOR_VERSION}/${PLATFORM_ARCHITECTURE}/${rpm_name}"
  curl -O $epel_url || ( display_error "Cannot download epel repo from ${epel_url}" ; exit 1 )
  mv $rpm_name ${SCRIPT_TEMP_DIR}/
  rpm -ivh ${SCRIPT_TEMP_DIR}/${rpm_name} >> $LOGFILE 2>&1
  yum clean all >> $LOGFILE 2>&1
fi
