if [ ! -f /etc/yum.repos.d/devportal.repo ] ; then
  cat <<EOF > /etc/yum.repos.d/devportal.repo
[devportal]
name=Apigee Dev Portal Installation CDROM
baseurl=file://${BUNDLE_ROOT}/bundle/devportal-repo
enabled=0
EOF

  yum clean all >> $LOGFILE 2>&1
fi

