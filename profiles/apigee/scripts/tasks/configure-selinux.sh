# -----------------------------------------------------
# STEP: Configure SELinux policy if necessary
# -----------------------------------------------------
if [[ -f /usr/sbin/getsebool && `getsebool httpd_can_network_connect | cut -d " " -f3` = 'off' ]] ; then
  display_step "Setting SELinux policy for outgoing httpd network connections..."
  display "(This can take quite a few seconds; please be patient.)"
  setsebool -P httpd_can_network_connect 1
fi
