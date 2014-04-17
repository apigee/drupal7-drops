Feature: Devconnect Blog
  In order to simplify bloging
  administrators should be able to
  create a blog entry easily

  @api
  Background:
    Given that I am a user logged in with the "Administrator" role


  Scenario:
    Given I am on the homepage
    Then I should see the link "Blog"

  Scenario:
    Given I am on the homepage
    When I click "Blog"
    Then I should be on "/blog"

  Scenario:
    Given that I am on  "/blog"
    Then I should see "Article"

  Scenario:
    Given I am on "/blog"
    When I press "Article"
    Then I should be be on "/node/add/article"

  Scenario:
    Given i am on "node/add/article"
    Then I should see the "node-article" form

  Scenario:
    Given that I am on the "node/add/article" page
    When I enter "Blog Test Article" in the title field
    And I enter "lorem ipsum" in the body text
    And I click the "submit" button
    Then I should go to the "blog" page
    And I should see "loren ipsum" on the page
    And I should see "Blog Test Article" on the page