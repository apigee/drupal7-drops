# -----------------------------------------------------
# STEP: Install required packages
# -----------------------------------------------------
display_step "Installing required packages"
if [[ $OPDK_STANDALONE -eq 1 ]]; then
  # Hack. Force upgrade of these packages based on path rather than package
  # name. Can't figure out any other way. May need manual maintenance as
  # packages receive updates upstream.
  rpm -Uvh --replacepkgs ${RPM_LOCAL_PATH}/glibc-common*.rpm \
    ${RPM_LOCAL_PATH}/glibc-2*.rpm \
    ${RPM_LOCAL_PATH}/mysql-libs*.rpm
	
  yum install --disablerepo=\* --enablerepo=devportal -y --nogpgcheck \
    php php-gd php-mbstring php-pdo php-xml php-mysql php-devel php-mcrypt \
    php-pear gcc make httpd mysql pcre-devel php-pecl-apc crontabs wget
else
  yum install -y \
    php php-gd php-mbstring php-pdo php-xml php-mysql php-devel php-mcrypt \
    php-pear gcc make httpd mysql pcre-devel php-pecl-apc crontabs wget
fi

display_step "Making sure Apache runs at startup"
chkconfig --levels 35 httpd on >> $LOGFILE 2>&1
service httpd start
