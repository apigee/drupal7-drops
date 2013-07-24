<?php

namespace Drupal\xautoload\Tests;

class ArithmeticsUnitTest extends \DrupalUnitTestCase {

  static function getInfo() {
    return array(
      'name' => 'X Autoload arithmetics test',
      'description' => 'This test class is only to prove that you can use xautoload for PSR-0 tests.',
      'group' => 'X Autoload',
    );
  }

  function testAddition() {
    $this->assert(5 + 2 == 7, '5 + 2 == 7');
    $this->assert(3 + 14 + 7 == 24, '3 + 14 + 7 == 24');
  }

  function testMultiplication() {
    $this->assert(5 * 3 == 15, '5 * 3 == 15');
    $this->assert(0.5 * 0.1 == 0.05, '0.5 * 0.1 == 0.05');
  }
}
