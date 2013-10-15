# -----------------------------------------------------
# STEP: Install required packages
# -----------------------------------------------------
display_step "Installing required packages"
if [[ $OPDK_STANDALONE -eq 1 ]]; then
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
