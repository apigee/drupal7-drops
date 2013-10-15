# -----------------------------------------------------
# STEP: Set PHP's timezone
# -----------------------------------------------------
if [[ `egrep -c '^;date.timezone =$' /etc/php.ini` -eq 1 ]] ; then
  display_step "Setting up the timezone in php.ini"
  php_timezone=`egrep '^ZONE=' /etc/sysconfig/clock | cut -d '"' -f2`
  sed -i -e "s:^;date.timezone =:date.timezone = ${php_timezone}:g" /etc/php.ini
fi

