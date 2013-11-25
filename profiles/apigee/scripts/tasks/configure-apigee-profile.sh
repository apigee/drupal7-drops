display_step "Drupal installation"

endpoint_connect_success=0
DEVCONNECT_ENDPOINT="https://api.enterprise.apigee.com/v1"
DEVCONNECT_ORG="my-org"
until [[ $endpoint_connect_success -eq 1 ]]; do
  question "What is the URI of the Apigee Management API Endpoint:" DEVCONNECT_ENDPOINT String $DEVCONNECT_ENDPOINT
  question "What is the Apigee Organization name:" DEVCONNECT_ORG String $DEVCONNECT_ORG
  question "What is the Username for the management API Endpoint?" DEVCONNECT_AUTH_USER String
  question "What is the Password for the management API Endpoint?" DEVCONNECT_AUTH_PASS String
  
  if [[ "${SKIP_CONNECTION_TEST}" = "Y" ]]; then
    endpoint_connect_success=1;	  
  else
    # k = allow insecure SSL, s = silent, f = fail silently on server error
    curl -k -s -f -X HEAD -u ${DEVCONNECT_AUTH_USER}:${DEVCONNECT_AUTH_PASS} "${DEVCONNECT_ENDPOINT}/o/${DEVCONNECT_ORG}" 2>&1 > /dev/null && endpoint_connect_success=1 || endpoint_connect_success=0
    if [[ $endpoint_connect_success -eq 0 ]]; then
      display "Could not connect to endpoint. Please check your parameters and try again."
    fi
  fi
done

## -----------------------------------------------------
## STEP: Run apigee profile installer
## -----------------------------------------------------
cd ${DRUPAL_WEBROOT}/sites/default

drush site-install apigee  apigee_install_api_endpoint.devconnect_org="${DEVCONNECT_ORG}" \
  apigee_install_api_endpoint.devconnect_endpoint="${DEVCONNECT_ENDPOINT}" \
  apigee_install_api_endpoint.devconnect_curlauth="${DEVCONNECT_AUTH_USER}:${DEVCONNECT_AUTH_PASS}"

cd $SCRIPT_PATH

