Feature:
  In order to access multiple 4g orgs
  as a Developer
  I need to CRUD apps

  Background:
    Given that I am logged in as a user with the "Authenticated User" role


  Scenario:
    Given I am on  "/user/me/apps"
    Then I should see text "Currently no apps"
    And I should see the link "Create One"

  Scenario:
    Given I am on  "/user/me/apps"
    When I click "Create App"
    Then I should be on "/user/me/apps/add"
    And I should see a "Apigee Edge Org" element
    And I should see a "new_machine_name" element
    And I should see a "callback_url" element
    And I should see a "API Products" element
    And I should see a "Creat App" button

  Scenario:
    Given I am on "/user/me/apps/add"
    When I select the radio button "testorama-nonprod"
    And I fill in "App Name" with value "TEST"
    And I fill in "callback_url" with value "http://www.apigee.com"
    And I select the radio button labeled "Gold"
    And I click the "Create App" button
    Then I should be on "/user/me/apps"
    And I should see link "Test (textorama-nonprod)"

  Scenario:
    Given that I am on "/user/me/apps"
    When I click link "Test (testorama-nonprod)"
    Then I should see "Key"
    And I should see "Secret"
    And I should see "Test's Keys"
    And I should see link "Product"
    And I should see link "Details"
    And I should see link 'Edit "Test" App
    And I should see link 'Delete "Test" App

  Scenario:
    Given that I am on "/usr/me/apps"
    When I click the "#Test #Product" link
    Then I should see "Gold"

  Scenario:
    Given that I am on "/user/me/apps"
    When I click the "#Test #Detail" link
    Then I should see "Application Name"
    Then I should see "API Products"
    Then I should see "Callback URL"

  Scenario:
    Given that I am on "/user/me/apps"

  Scenario:
    Given that I do not have "Allow Changes to App Org value" permission
    When I go to "/apps"
    Then I should see "Access Denied"