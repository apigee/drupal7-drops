<?php

interface XHProfRunsInterface {
  /**
  * This function gets runs based on passed parameters, column data as key, value as the value. Values
  * are escaped automatically. You may also pass limit, order by, group by, or "where" to add those values,
  * all of which are used as is, no escaping.
  *
  * @param array $stats Criteria by which to select columns
  * @return resource
  */
  public function getRuns($stats, $limit = 50, $skip = 0);
  public function getCount();
  public function get_run($run_id, $type, &$run_desc);
  public function save_run($xhprof_data, $type, $run_id = null);
}

