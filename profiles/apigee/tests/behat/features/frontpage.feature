Feature: Frontpage
  To have developers interested in your APIs
  As any user
  I should be an appealing homepage

  Background:
    Given I am on the homepage

  Scenario: Viewing content in a region
    Given I am on the homepage
    Then I should see "Forum Discussions" in the "content" region

  Scenario: User should be able to login
    Then I should see the link "Login"
    When I click "Login"
    Then I should get a "200" HTTP response

