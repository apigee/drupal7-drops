Feature:
  In order to trigger actions to respond to changing events
  as an Administrator
  I need to create rules based on devconnect entity actions
  

  Scenario:
    Given that I am a user logged in with the "Administrator" role
    And I am on "/admin/config/workflow/rules/reaction/add"
    When i fill in the "name" field with value "new devconnect rule1"
    And I choose the "Devconnect App change" option
    And I press the "Save" button
    Then I should be on "/admin/config/workflow/rules/reaction/manage/rules_new_entity_rule/edit/1"
    And I should see "Events" text
    And I should see "Conditions" text
    And I should see "Actions" text
    And I should see the "Add Condition" button

  Scenario:
    Given that I am on "/admin/config/workflow/rules/reaction/manage/rules_new_entity_rule/edit/1"
    And I press the "Add Condition" button
    When I choose the "App Status Change" option
    #loaded via ajax
    Then I should see "Choose From Status"
    And I should see "select#app_status"
    And I should see the "Any" option selected
    And I should see "Choose To Status"
    And I should see the "Any" option selected
    # all of the status options should be in select#app_status

