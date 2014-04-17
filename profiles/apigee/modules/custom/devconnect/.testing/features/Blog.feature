Feature: Blog
  In order to simplify bloging
  administrators should be able to
  create a blog entry easily
  
  #common to all scenarios
  Background:
    Given that I am logged in with the "Administrator" role


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
    Then I should be be on "/node/add/article?destination=blog"

  Scenario: 
    Given that I am on "node/add/article?destination=blog"
    When I fill in title with "Blog Test Article"
    And I fill in body with "lorem ipsum"
    And I press "submit"
    Then I should be on "/blog"
    And I should see "loren ipsum" in the "content" region
    And I should see "Blog Test Article" in the "content" region