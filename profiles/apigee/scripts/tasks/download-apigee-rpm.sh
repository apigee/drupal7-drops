if [[ ! -d $RPM_LOCAL_PATH ]] ; then
  mkdir -p $RPM_LOCAL_PATH
fi
RPM_FILENAME="$( cd $RPM_LOCAL_PATH; shopt -s nullglob; echo apigee-drupal-*.rpm )"
if [[ -z $RPM_FILENAME ]]; then
  downloaded_rpm=0
  display "You need to download the Apigee Drupal RPM. Please ask your Apigee salesperson"
  display "for the correct download URL. Supported protocols include http, https, ftp,"
  display "ftps, sftp, scp, and file. If you have the RPM for the devportal, please put"
  display "it in the following folder:"
  display "  ${RPM_LOCAL_PATH}/apigee-drupal-*.rpm"
  display "and then re-run this script."
  
  # curl handles all of the above protocols.
  while [[ $downloaded_rpm -eq 0 ]]; do
    question "Enter the download URL:" RPM_DOWNLOAD_URL String
    question "Enter username, if necessary:" RPM_DOWNLOAD_USER StringOrBlank
    if [[ ! -z $RPM_DOWNLOAD_USER ]] ; then
      question "Enter password:" RPM_DOWNLOAD_PASS String
    else
      RPM_DOWNLOAD_PASS=
    fi
    RPM_FILENAME="$( basename $RPM_DOWNLOAD_URL )"
    
    if [[ -z $RPM_DOWNLOAD_PASS ]] ; then
      curl -L -k -o ${RPM_LOCAL_PATH}/${RPM_FILENAME} $RPM_DOWNLOAD_URL && downloaded_rpm=1    
    else
      curl -L -k -u "${RPM_DOWNLOAD_USER}:${RPM_DOWNLOAD_PASS}" -o ${RPM_LOCAL_PATH}/${RPM_FILENAME} $RPM_DOWNLOAD_URL && downloaded_rpm=1
    fi
    if [[ $downloaded_rpm -eq 0 ]] ; then
      display "Sorry, the URL and/or credentials you gave were not correct; please try again."    	    
    fi
  done
fi

