<?php

function devconnect_partner_profile_form($form, &$form_state, $account) {

  $partner_id = _devconnect_partner_get_partner_uid($account->uid);
  if ($partner_id) {
    if ($partner_id != $account->uid) {
      drupal_access_denied();
    }
  }
  $entity = entity_load_single('developer', $account->mail);

  $form['partner_entity'] = array(
    '#type' => 'value',
    '#value' => $entity
  );

  $form['partner_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Partner Name'),
    '#required' => TRUE,
    '#maxlength' => 255,
    '#default_value' => (array_key_exists('partner_name', $entity->attributes) ? $entity->attributes['partner_name'] : '')
  );

  // TODO: add other fields as required here

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save')
    )
  );

  return $form;
}

function devconnect_partner_profile_form_validate($form, &$form_state) {
  $query = db_select('dc_dev', 'd');
  $query->innerJoin('dc_dev_attributes', 'a', 'd.developer_id = a.developer_id');
  // Searching for attributes which:
  //   a. Are not on the current user
  //   b. Have the name 'partner_name'
  //   c. Have the current form value for partner_name
  // If we find such a record, then we kick the form back as invalid.
  $uid = $query->fields('d', array('uid'))
    ->condition('a.name', 'partner_name')
    ->condition('a.value', $form_state['values']['partner_name'])
    ->condition('d.uid', $form_state['values']['partner_entity']->uid, '<>')
    ->execute()
    ->fetchField();
  if ($uid) {
    form_set_error('partner_name', t('This partner name is already taken; please choose another.'));
  }
}

function devconnect_partner_profile_form_submit($form, &$form_state) {
  $entity = $form_state['values']['partner_entity'];
  $entity->attributes['partner_id'] = $entity->developerId;
  $entity->attributes['partner_name'] = $form_state['values']['partner_name'];
  $entity->attributes['partner_perms'] = 'admin,view,create,edit,remove';
  entity_save('developer', $entity);
  drupal_set_message(t('Your changes have been saved.'));
}