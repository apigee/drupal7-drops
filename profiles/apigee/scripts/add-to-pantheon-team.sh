#!/bin/bash

if [[ $# -lt 1 ]] ; then
  echo "Usage: ${0} <email>"
  exit 1
fi

apigee_uuid=$( drush porgs | grep Apigee | sed -E 's: +: :g' | cut -d " " -f4 )
my_email=$1

site_uuids=$( drush porg-sites $apigee_uuid | tail -n +2 | sed -E 's: +: :g' | cut -d " " -f 4 )

for site_uuid in $site_uuids ; do
  has_me=$( drush psite-team $site_uuid | grep -ci $my_email )
  site_name=$( drush psite-name $site_uuid )
  if [[ $has_me -eq 0 ]] ; then
    echo -n "Adding ${my_email} to site ${site_name}... "
    drush psite-team-add $site_uuid $my_email 
  else
    echo "${my_email} is already a team member of site ${site_name}"
  fi
done
