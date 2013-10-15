if [[ $PLATFORM_NAME == "Redhat" ]]; then
  display_step "Validating Yum Repositories"
  # If rhn_check comes back w/error, the system is not registered.
  if [ ! -f /etc/sysconfig/rhn/systemid ] ; then
    display_error "The server is not registered with the RedHat network. Please register your system with RHN using the rhn_register command and restart this script."
    exit 1
  else
    display "System is registered on RHN."
  fi

  # Make sure the RHN server-optional channel is registered
  if [[ `rhn-channel -l | grep -c 'server-optional'` -eq 1 ]] ; then
    display "System is registered to RHN channel server-optional."
  else
    display_error "The server is not registered to the server-optional channel. Please register the server-optional RHN channel restart this script."
    display_header "

You can register the server-optional channel by using the following command:

  rhn-channel --add --channel=<channel-name> --user=<rhn-username> --password=<rhn-password>

To find the <channel-name>, use the following command:

  rhn-channel -L -u <rhn-username> -p <rhn-password> | grep server-optional

"
    exit 1
  fi
fi
