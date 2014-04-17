Feature:
  In order to access multiple 4g orgs
  as an Administrator
  I need to administer my orgs

  Background:
    Given that I am logged in as a user with the "Administrator" role

  Scenario:
    When I visit "/admin/config/devconnect"
    Then I should see an empty Table

  Scenario: Add Connection Button Should exist
    Given that I am on "/admin/config/devconnect/connections"
    Then I should see the "add an Apigee Edge connection"

  Scenario: Add connection form
    Given that I am on "/admin/config/devconnect/connections"
    When I click "Add an Apigee Edge Connection"
    Then I should be on "/admin/config/devconnect/connections/add"
    And I should see "Add an Apigee Edge Connection"
    And I should see a "org" element
    And I should see a "endpoint" element
    And I should see a "user" element
    And I should see a "password" element

  Scenario: Test the connection before moving on
    Given that I am on "/admin/config/devconnect/connections/add"
    And I see "https://api.enterprise.apigee.com/v1" in the "endpoint" field
    When I fill in "org" field with the value "testorama"
    And I fill in "user" with the value "dc_testorama@apigee.com"
    And I fill in "password" with the value "PASSWORD"
    And I click "Save Configuration"
    Then I should be on "/admin/config/devconnect/connections/add"
    And I should see "Invalid Password"

  Scenario:
    Given that I am on "/admin/config/devconnect/connections/add"
    And I see "https://api.enterprise.apigee.com/v1" in the "endpoint" field
    When I fill in "org" field with the value "testorama"
    And I fill in "user" with the value "dc_testorama@apigee.com"
    And I fill in "password" with the value "<put valid password here>"
    And I click "Save Configuration"
    Then I should be on "/admin/config/devconnect/connections"
    And I should see "New Connection Added"
    And I should see a table
      | Testorama ( testorama ) | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |

  Scenario:
    Given that I am on "/admin/config/devconnect"
    And I see the "active" link
    When I click "active" link
    Then I should be on "/admin/config/devconnect/connections"
    And I should see a table
      | testorama | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Disabled | Edit | Delete | Test | API Products | API Products |


  Scenario:
    Given that I am on "/admin/config/devconnect"
    And I see the "disbled" link
    When I click the "disabled" link
    Then I should be on "/admin/config/devconnect/connections"
    And I should see a table
      | testorama | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |

  Scenario:
    Given that I am on "/admin/config/devconnect/connections/add"
    And I see "https://api.enterprise.apigee.com/v1" in the "endpoint" field
    When I fill in "org" field with the value "testorama-prod"
    And I fill in "user" with the value "dc_testorama@apigee.com"
    And I fill in "password" with the value "<put valid password here>"
    And I click "Save Configuration"
    Then I should be on "/admin/config/devconnect/connections"
    And I should see "New Connection Added"
    And I should see a table
      | testorama      | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |
      | testorama-prod | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |

  Scenario:
    Given that I am on "/admin/config/devconnect/connections"
    And I see a table
      | testorama      | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |
      | testorama-prod | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |
    When I click "Edit"
    Then I should be on "/admin/config/devconnect/connections/edit/testorama"
    And I should see "Editing Apigee Edge connection 'testorama'"
    And I should see "testorama" in the "org" field
    And I should see "https://api.enterprise.apigee.com/v1" in the "endpoint" field
    And I should see "dc_testorama@apigee.com" in the "user" field

  Scenario:
    Given that I am on "/admin/config/devconnect/connections/edit/testorama"
    And I see "Editing Apigee Edge connection 'testorama'"
    When I change "org" field to value "testorama-nonprod"
    And I change "user" field to value "dc_devport+testoramanonprod@apigee.com"
    And I change "password" field to "<put valid password here>"
    And I click "Save Cofiguration"
    Then I should be on "/admin/config/devconnect/connections"
    And I should see table
      | testorama-nonprod | https://api.enterprise.apigee.com/v1 | dc_devport+testoramanonprod@apigee.com | Active | Edit | Delete | Test | API Products |
      | testorama-prod    | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com                | Active | Edit | Delete | Test | API Products |

  Scenario:
    Given that I am on "/admin/config/devconnect/connections"
    When I click "delete"
    Then I should be on "/admin/config/devconnect/connections/testorama-nonprod/delete"
    And I should see "Would you like to delete the Apigee Edge connection to 'testorama-nonprod'"
    And I should see "Delete this connection" button
    And I should see "Cancel" button

  Scenario:
    Given that I am on "/admin/config/devconnect/connections/testorama-nonprod/delete"
    When I click "Cancel" button
    Then I should be on ""/admin/config/devconnect/connections"
    And I should see table
      | testorama-nonprod | https://api.enterprise.apigee.com/v1 | dc_devport+testoramanonprod@apigee.com | Active | Edit | Delete | Test | API Products |
      | testorama-prod    | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com                | Active | Edit | Delete | Test | API Products |

  Scenario: Delete connection
    Given that I am on "/admin/config/devconnect/connections/testorama-nonprod/delete"
    When I click "Delete this connection" button
    Then I should be on ""/admin/config/devconnect/connections"
    And I should see table
      | testorama-prod    | https://api.enterprise.apigee.com/v1 | dc_testorama@apigee.com | Active | Edit | Delete | Test | API Products |


  Scenario: Test connection to Apigee Edge org
    Given that I am on "/admin/config/devconnect/connections"
    And I click "Test"
    Then I should see modal "test connection successful"

  Scenario: Show api product list
    Given that I am on "/admin/config/devconnect/connections"
    And I click "API Products"
    Then I should see a modal window with table
      | Display Name (machine name)       | Access              |
      ===========================================================
      | Display Name (worlds_best_api)    | Public              |
      | Display Name (manual-test)        | Public              |
      | Display Name  (airport_status)    | Private (not shown) |
      | Display Name (testcheggproduct)   | Private (not shown) |
      | Display Name (PremiumWeatherAPI)  | Private (not shown) |
      | Display Name (Gold)               | Private (not shown) |
      | Display Name (test-product)       | Hidden (Not shown)  |
    And I should see a modal "dismiss"

  Scenario:
    Given that I am a user logged in with "administer permissions" permission
    When I am on "/people/roles"
    Then I should see "Administrator"
    And I should see "App Admin"

  Scenario:
    Given that I am a user logged in with "Administrator" role
    When I am on "/people/privileges"
    Then I should see "Multiorg"
    And I should see "Allow Changes to App Org value" permission
    And I should see checkbox "Authenticated User" unchecked
    And I should see checkbox "App Admin" checked
    And I should see checkbox "Administrator" checked