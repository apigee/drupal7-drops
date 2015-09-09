<?php

/**
 * Bugfixes made over time test class.
 */
abstract class Redis_Tests_Path_PathUnitTestCase extends Redis_Tests_AbstractUnitTestCase
{
    /**
     * @var Cache bin identifier
     */
    static private $id = 1;

    /**
     * Get cache backend
     *
     * @return Redis_Path_HashLookupInterface
     */
    final protected function getBackend($name = null)
    {
        if (null === $name) {
            // This is needed to avoid conflict between tests, each test
            // seems to use the same Redis namespace and conflicts are
            // possible.
            $name = 'cache' . (self::$id++);
        }

        $className = Redis_Client::getClass(Redis_Client::REDIS_IMPL_PATH);
        $hashLookup = new $className(Redis_Client::getClient(), 'path', Redis_Client::getDefaultPrefix('path'));

        return $hashLookup;
    }

    /**
     * Tests basic functionnality
     */
    public function testPathLookup()
    {
        $backend = $this->getBackend();

        $source = $backend->lookupSource('node-1-fr', 'fr');
        $this->assertIdentical(null, $source);
        $alias = $backend->lookupAlias('node/1', 'fr');
        $this->assertIdentical(null, $source);

        $backend->saveAlias('node/1', 'node-1-fr', 'fr');
        $source = $backend->lookupSource('node-1-fr', 'fr');
        $source = $backend->lookupSource('node-1-fr', 'fr');
        $this->assertIdentical('node/1', $source);
        $alias = $backend->lookupAlias('node/1', 'fr');
        $this->assertIdentical('node-1-fr', $alias);

        // Delete and ensure it does not exist anymore.
        $backend->deleteAlias('node/1', 'node-1-fr', 'fr');
        $source = $backend->lookupSource('node-1-fr', 'fr');
        $this->assertIdentical(null, $source);
        $alias = $backend->lookupAlias('node/1', 'fr');
        $this->assertIdentical(null, $source);

        // Set more than one aliases and ensure order at loading.
        $backend->saveAlias('node/1', 'node-1-fr-1', 'fr');
        $backend->saveAlias('node/1', 'node-1-fr-2', 'fr');
        $backend->saveAlias('node/1', 'node-1-fr-3', 'fr');
        $alias = $backend->lookupAlias('node/1', 'fr');
        $this->assertIdentical('node-1-fr-3', $alias);

        // Add another alias to test the delete language feature.
        // Also add some other languages aliases.
        $backend->saveAlias('node/1', 'node-1');
        $backend->saveAlias('node/2', 'node-2-en', 'en');
        $backend->saveAlias('node/3', 'node-3-ca', 'ca');

        // Ok, delete fr and tests every other are still there.
        $backend->deleteLanguage('fr');
        $alias = $backend->lookupAlias('node/1');
        $this->assertIdentical('node-1', $alias);
        $alias = $backend->lookupAlias('node/2', 'en');
        $this->assertIdentical('node-2-en', $alias);
        $alias = $backend->lookupAlias('node/3', 'ca');
        $this->assertIdentical('node-3-ca', $alias);

        // Now create back a few entries in some langage and
        // ensure fallback to no language also works.
        $backend->saveAlias('node/4', 'node-4');
        $backend->saveAlias('node/4', 'node-4-es', 'es');
        $alias = $backend->lookupAlias('node/4');
        $this->assertIdentical('node-4', $alias);
        $alias = $backend->lookupAlias('node/4', 'es');
        $this->assertIdentical('node-4-es', $alias);
        $alias = $backend->lookupAlias('node/4', 'fr');
        $this->assertIdentical('node-4', $alias);
    }
}
