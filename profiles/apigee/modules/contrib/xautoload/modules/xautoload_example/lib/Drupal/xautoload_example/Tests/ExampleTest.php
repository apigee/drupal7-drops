<?php

namespace Drupal\xautoload_example\Tests;

class ExampleTest extends \DrupalUnitTestCase {

  static function getInfo() {
    return array(
      'name' => 'X Autoload example test',
      'description' => 'This test class is only to prove that disabled modules still have their tests working.',
      'group' => 'X Autoload Example',
    );
  }

  function testStringConcat() {
    $this->assert('aa' + 'bb' == 'aabb', "'aa' + 'bb' == 'aabb'");
  }
}
