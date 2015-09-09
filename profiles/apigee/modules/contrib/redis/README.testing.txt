Redis module testing
====================

Unit tests won't cover the cache backend until the 8.x series.

This won't be fixed, by design. Drupal 8.x now ships a complete cache backend
unit tests suite which will be used when this module will be upgraded.

7.x testing
===========

7.x testing will keep it to the bare minimum and will test some minor bugs such
as admin UI or autoloading related bugs.

Cleanup environment
===================

php -f scripts/run-tests.sh -- --clean

Run all PhpRedis tests
======================

php -f scripts/run-tests.sh -- --verbose --color \
    --url "http://yoursite" \
    --class "Redis_Tests_Cache_PhpRedisFlushUnitTestCase,Redis_Tests_Cache_PhpRedisFixesUnitTestCase,Redis_Tests_Client_MockFactory,Redis_Tests_Lock_PhpRedisLockingUnitTestCase"

Run all Predis tests
======================

php -f scripts/run-tests.sh -- --verbose --color \
    --url "http://yoursite" \
    --class "Redis_Tests_Cache_PredisFlushUnitTestCase,Redis_Tests_Cache_PredisFixesUnitTestCase,Redis_Tests_Client_MockFactory,Redis_Tests_Lock_PredisLockingUnitTestCase"
