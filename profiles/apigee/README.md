#Apigee Drupal's Distribution Install Profile

This is the first stab at an install profile and migration script from Apigee's internal Drupal hosting to pantheon. 

To use the drush command, drop it in ~/.drush for the user who will be executing the command.

`drush apigee-restore --file=apigee_hosting_db_backup.mysql`

`drush apigee-migrate --yes`