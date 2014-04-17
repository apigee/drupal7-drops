Feature:
  In order to move an application between orgs
  as an App Administrator
  I need to administer my apps

  Background:
    Given that I am logged in with the role "App Admin"

  Scenario: apps view w/bulk operations
    Given that I am on "/apps"
    Then I should see "Application List"
    And I shoudl see table
      |     | App Name  | Apigee Edge Org   | Status |
      | [ ] | app1      | testorama         | Active |
      | [ ] | app2      | testorama-nonprod | Active |
      | [ ] | app3      | testorama-prod    | Active |
    And I should see "operations"
    And I should see "edit-operation" element
    And I should see "execute" button


  Scenario: push operation on app listing
    Given that I am on "/apps"
    When I check "#app1"
    And I choose select "push to new org" option
    And I press "execute" button
    Then I should be on "/apps"
    And I should see "You have selected the following item"
    And I should see "app1"
    And I should see "Confirm" button

  Scenario: push app to new org dialog
    Given I am on "/apps"
    And I see "You have selected the following item"
    When I press "confirm" button
    Then I should be on "/apps"
    And I should see table
      |     | App Name  | Apigee Edge Org   | Status |
      | [ ] | app1      | testorama         | Active |
      | [ ] | app1      | testorama-nonprod | Active |
      | [ ] | app2      | testorama-nonprod | Active |
      | [ ] | app3      | testorama-prod    | Active |

  Scenario: disable operation on app listing
    Given that I am on "/apps"
    When I check "#app1"
    And I choose select "disable app and keys" option
    And I press "execute" button
    Then I should be on "/apps"
    And I should see "You have selected the following item"
    And I should see "app1"
    And I should see "Confirm" button


  Scenario: push app to new org dialog
    Given I am on "/apps"
    And I see "You have selected the following item"
    When I press "confirm" button
    Then I should be on "/apps"
    And I should see table
      |     | App Name  | Apigee Edge Org   | Status   |
      | [ ] | app1      | testorama         | Disabled |
      | [ ] | app1      | testorama-nonprod | Active   |
      | [ ] | app2      | testorama-nonprod | Active   |
      | [ ] | app3      | testorama-prod    | Active   |

