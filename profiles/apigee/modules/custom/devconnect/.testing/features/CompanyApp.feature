Feature: Company App
  In order to gain access to Company-based apps
  developers should be able to
  see Company app details and analytics

  @api
  Background:
    Given that I am logged in with the "Authenticated User" role

  Scenario:
    Given I am on "/user/me/apps"
    Then I should see the link "Company Apps"

  Scenario:
    Given I am on "/user/me/apps"
    When I click "Company Apps"
    Then I should see
    ===
    ===


