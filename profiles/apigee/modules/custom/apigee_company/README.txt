
This module allows a monetization developer to have subscriptions under a personal or
company account. For example, if a developer want to have a subscription to certain APIs
for personal use, and other subscriptions to APIs for a company project, they can purchase
these subscriptions separately.

Also, if you purchase a subscription in a company context, you can allow other developers
to create APIs under that subscription.  This is a useful feature to allow one person on
a project to pay for a subscription and then allow all developers on that project use the
subscription instead of each developer paying for their own app access individually.

Learn more: http://docs.apigee.com/monetization/content/companies-developers-self-service

Installation
------------

Prerequisite: You must have an Apigee Monetization enabled organization.

Install the module as you would any Drupal module. However, this module depends
on some Edge API calls that are only available to Apigee Monetization enabled
organizations.  If your org is not monetization enabled, this feature will
not work.

The Apigee Responsive theme has the company switcher available, but it may be hidden
due to the fixed top navigation bar.  To fix this, click on Appearance in the admin
bar, then click on "settings" on the Apigee Responsive theme. Click on "Components" on
left hand side, then click "Navbar" in Components section. Change "Navbar Position" to
"Static Top".

If you are using your own theme, you can add the company switcher block to anywhere on your
theme by going to Structure > Blocks on the admin bar, then put the "Switch Company" block
in a region on your page.

Troubleshooting
---------------
Problem:
The company features are not working properly.

Solution:
Make sure you have a Apigee Monetization enabled organization.



