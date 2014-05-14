@api @permissions
Feature: Permissions
  As a site administrator
  I want to control who can do what with entity types, bundles and entities
  so users don't get themselves in trouble.

  Scenario: This is a set up step
    Given I am logged in as a user with the "administrator" role
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "Entity Type" with "Test 12587"
    And I fill in "Machine-readable name" with "test_12587"
    And I check "Title"
    And I press the "Save" button
    And I visit "/admin/structure/entity-type/test_12587/test_12587"
    And I click "Add Test 12587"
    And I fill in "Title" with "Entity 1239"
    And I press the "Save" button

  @entity-type
  Scenario: Only allowed users can access the entity type's overview page
    Given I am logged in as a user with the "Use the administration pages and help" permission
    And I visit "/admin/structure"
    Then I should not see the text "Entity types"
    
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure"
    And I click "Entity types"
    Then I should get a "200" HTTP response

  @entity-type
  Scenario: Only allowed users can add entity types from the overview page
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "Add entity type"

    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,Add Entity Types" permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    Then I should get a "200" HTTP response

  @entity-type
  Scenario: Only allowed users can delete entity types
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "delete"

    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,Delete Entity Types" permissions
    And I visit "/admin/structure/entity-type"
    And I click "delete" in the "Test 12587" row
    Then I should get a "200" HTTP response

  @bundle
  Scenario: Only allowed users can access the bundle's overview page
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the text "Test 12587"
    And I should not see the link "Test 12587"
    
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the link "Test 12587"
    When I click "Test 12587"
    Then I should get a "200" HTTP response

  @bundle
  Scenario: Only allowed users can add bundles from the overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/test_12587"
    Then I should not see the link "Add bundle"
    
    Given I am logged in as a user with the "View Bundle Lists,Add Bundles" permissions
    And I visit "/admin/structure/entity-type/test_12587"
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response

  @bundle
  Scenario: Only allowed users can delete bundles from the overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/test_12587"
    Then I should not see the link "delete"
    
    Given I am logged in as a user with the "View Bundle Lists,Delete Bundles" permissions
    And I visit "/admin/structure/entity-type/test_12587"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

  @entity
  Scenario: Only allowed users can access the entity's overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/test_12587"
    Then I should see the text "Test 12587"
    And I should not see the link "Test 12587"
    
    Given I am logged in as a user with the "View Bundle Lists,View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/test_12587"
    Then I should see the link "Test 12587"
    When I click "Test 12587"
    Then I should get a "200" HTTP response

  @entity
  Scenario: Only allowed users can add entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/test_12587/test_12587"
    Then I should not see the link "Add Test 12587"
    
    Given I am logged in as a user with the "View Entity Lists,Add Entities" permissions
    And I visit "/admin/structure/entity-type/test_12587/test_12587"
    Then I should see the link "Add Test 12587"
    When I click "Add Test 12587"
    Then I should get a "200" HTTP response

  @entity
  Scenario: Only allowed users can delete entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/test_12587/test_12587"
    Then I should not see the link "delete"
    
    Given I am logged in as a user with the "View Entity Lists,Delete Any Entity" permissions
    And I visit "/admin/structure/entity-type/test_12587/test_12587"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

  Scenario: This is a clean up step
    Given I am logged in as a user with the "administrator" role
    Given I visit "/admin/structure/entity-type"
    And I click "Test 12587"
    And I click "Delete"
    And I press the "Delete" button
