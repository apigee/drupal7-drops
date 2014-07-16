#!/bin/bash

# Polls all pantheon-hosted live sites, checking to see if SMTP module is
# enabled, and if so, if the module is actually "turned on" via the smtp_on
# Drupal variable, and configured with a host via the smtp_host Drupal variable.
# Sites with SMTP enabled and configured which are using Apigee's default SMTP
# username are then echoed to stdout.

# You are advised to make sure of the following:
# 1. You are logged in (try drush pauth)
# 2. You are on the "team" for all sites
# 3. You have a recent list of aliases (try drush paliases)

if [[ ! -f ~/.apigee-smtp-user || ! -s ~/.apigee-smtp-user ]] ; then
  echo "Please place the Apigee SMTP user name in ~/.apigee-smtp-user, then"
  echo "re-run this script."
  exit 1
fi

apigee_smtp_user=$( cat ~/.apigee-smtp-user )

# Log in to Pantheon
#drush pauth

# Pull latest list of aliases
#drush paliases

for site in $( drush sa | egrep '^@pantheon.*live$' | grep -v presales ); do
  # Is SMTP module enabled?
  has_smtp=$( drush ${site} --type=module --status=enabled --pipe --no-core pm-list | egrep -c '^smtp$' )
  smtp_on=0
  smtp_host=""
  smtp_user=""
  if [[ $has_smtp -gt 0 ]]; then
    # Is smtp_on var set to non-zero? If present it should be "0" or "1"
    smtp_on=$( drush ${site} vget --exact --format=json smtp_on 2>/dev/null | sed 's:"::g' )
    smtp_host=$( drush ${site} vget --exact --format=json smtp_host 2>/dev/null | sed 's:"::g' )
    smtp_user=$( drush ${site} vget --exact --format=json smtp_username 2>/dev/null | sed 's:"::g' )
    if [[ $smtp_on == '' ]] ; then
      # smtp_on var was never set.
      smtp_on=0;
    fi    
    has_smtp=$smtp_on

    if [[ $has_smtp -gt 0 ]] ; then
      # Is smtp_host var set to non-empty?
      has_smtp=$( echo $smtp_host | sed 's:"::g' | wc -c )
    fi
  fi
  
  if [[ $smtp_on -gt 0 && $smtp_user == $apigee_smtp_user ]] ; then
    echo $site | cut -d "." -f 2
  fi

done
