# -----------------------------------------------------
# STEP: Make sure IUS Community location is available
# -----------------------------------------------------
display_step "Validating network is available"

# we don't know yet if wget is installed.
until curl -f -s -L -X HEAD -H "Connection: Close" http://apigee.com/about/ ; do
  display_header "
Could not access the network.
Please make sure this computer is properly connected to the internet.
"
  question "Press ENTER to try again..." DISCARD_ME StringOrBlank
done

