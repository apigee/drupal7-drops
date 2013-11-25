# -----------------------------------------------------
# STEP: Upgrade PEAR
# -----------------------------------------------------
display_step "Upgrading PEAR"
pear channel-update pear.php.net
pear upgrade -f -a PEAR Console_Color Console_Table | tee $LOGFILE 2>&1

# -----------------------------------------------------
# STEP: Install Drush
# -----------------------------------------------------
display_step "Installing Drush"

# Install drush channel if needed
if [[ `pear list-channels  | grep -c 'pear.drush.org'` -eq 1 ]] ; then
  display "Pear channel pear.drush.org is already installed."
else
  pear config-set auto_discover 1 >> $LOGFILE 2>&1
  pear channel-discover pear.drush.org >> $LOGFILE 2>&1
  pear config-set preferred_state stable >> $LOGFILE 2>&1
fi

if [[ `pear info drush/drush | grep -c 'No information found'` -eq 0 ]] ; then
  display "Drush is already installed."
else
  pear install -a drush/drush-5.9.0 >> $LOGFILE 2>&1
fi

drush -y dl registry_rebuild >> $LOGFILE 2>&1

