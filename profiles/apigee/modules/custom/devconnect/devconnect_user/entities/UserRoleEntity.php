<?php

namespace Drupal\devconnect_user;
use \Drupal\devconnect\ArrayEntity;

class UserRoleEntity extends ArrayEntity {
  /**
   * @var string
   */
  public $name;
  /**
   * @var array
   */
  public $users;
}
