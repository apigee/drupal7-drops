<?php

abstract class DevconnectTestCase extends DrupalWebTestCase {

  /**
   * @var stdClass
   */
  protected $user;

  /**
   * {@inheritdoc}
   */
  public function setUp() {
    // Save this value from the old system.
    $credential_dir = variable_get('apigee_credential_dir');
    $this->profile = 'apigee_testing';
    parent::setUp();
    // Persist our cached value in the new database.
    variable_set('apigee_credential_dir', $credential_dir);

    $edit = [
      'name' => $this->randomName(),
      'mail' => 'simpletest-' . $this->randomName() . '@example.com',
      'pass' => user_password(),
      'status' => 1,
      'roles' => [],
    ];
    $edit['field_first_name'][LANGUAGE_NONE][0]['value'] = $this->randomName();
    $edit['field_last_name'][LANGUAGE_NONE][0]['value'] = $this->randomName();

    $account = drupal_anonymous_user();
    // If account->mail is unset, devconnect_user won't persist it to Edge.
    $account->mail = $edit['mail'];

    // Force devconnect_default_org_config() to throw away its caches, because
    // devconnect_cron() may have injected bogus values.
    drupal_static_reset('devconnect_default_org_config');

    // Create our user.
    $this->user = user_save($account, $edit);

    $this->user->pass_raw = $edit['pass'];
    $this->drupalLogin($this->user);
  }

  /**
   * {@inheritdoc}
   */
  protected function tearDown() {
    // Make sure remote API deletes this user.
    if (is_object($this->user)) {
      try {
        entity_delete('developer', $this->user->mail);
      }
      catch (Exception $e) {
        $this->verbose('Exception while deleting developer: ' . $e->getMessage());
      }
    }
    $this->user = NULL;
    parent::tearDown();
  }
}
