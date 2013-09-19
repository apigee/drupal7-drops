<?php

class MongodbXHProfRuns implements XHProfRunsInterface {

  private $dir = '';

  private function gen_run_id($type) {
    return uniqid();
  }

  private function file_name($run_id, $type) {
    $file = "$run_id.$type";

    if (!empty($this->dir)) {
      $file = $this->dir . "/" . $file;
    }
    return $file;
  }

  public function __construct($dir = null) {
  }

  /**
  * This function gets runs based on passed parameters, column data as key, value as the value. Values
  * are escaped automatically. You may also pass limit, order by, group by, or "where" to add those values,
  * all of which are used as is, no escaping.
  *
  * @param array $stats Criteria by which to select columns
  * @return resource
  */
  public function getRuns($stats, $limit = 50, $skip = 0) {
    $collection = mongodb_collection('xhprof');
    $sort = isset($_GET['order']) ? $_GET['order'] : 'date';
    $sort_direction_str = isset($_GET['sort']) ? $_GET['sort'] : 'desc';
    $sort_direction = array(
      'asc' => 1,
      'desc' => -1,
    );

    $cursor = $collection
    ->find(array())
    ->limit($limit)
    ->skip($skip)
    ->sort(array(strtolower($sort) => $sort_direction[$sort_direction_str]));
    ;
    $runs = array();
    foreach ($cursor as $id => $run) {
      $run['run_id'] = $id;
      $runs[] = $run;
    }

    return $runs;
  }

  public function getCount() {
    $collection = mongodb_collection('xhprof');
    $count = $collection->count();
    return $count;
  }

  public function get_run($run_id, $type, &$run_desc) {
    $collection = mongodb_collection('xhprof');
    $run_desc = "XHProf Run (Namespace=$type)";

    $run = $collection->findOne(array('_id' => (string)$run_id));
    return $run['run_data'];
  }

  public function save_run($xhprof_data, $type, $run_id = null) {
    if ($run_id === null) {
      $run_id = $this->gen_run_id($type);
    }

    $mongo_id = $run_id;

    module_load_include('module', 'mongodb');
    $collection = mongodb_collection('xhprof');

    $entry = array();
    $entry['_id'] = (string)$mongo_id;
    $entry['run_data'] = $xhprof_data;
    $entry['get'] = serialize($_GET);
    $entry['cookie'] = serialize($_COOKIE);
    $entry['date'] = $_SERVER['REQUEST_TIME'];
    $entry['pmu'] = isset($xhprof_data['main()']['pmu']) ? $xhprof_data['main()']['pmu'] : '';
    $entry['wt']  = isset($xhprof_data['main()']['wt'])  ? $xhprof_data['main()']['wt']  : '';
    $entry['cpu'] = isset($xhprof_data['main()']['cpu']) ? $xhprof_data['main()']['cpu'] : '';



    $entry['path'] = isset($_SERVER['REQUEST_URI']) ? $_SERVER['REQUEST_URI'] : $_SERVER['PHP_SELF'];
    $entry['servername'] = isset($_SERVER['SERVER_NAME']) ? $_SERVER['SERVER_NAME'] : '';

    $collection->save($entry);

    return $run_id;
  }
}
