<?php


class xautoload_NamespaceHandler_checkNothing implements xautoload_NamespaceHandler_Interface {

  /**
   * Expect a class Aaa_Bbb_Ccc_Ddd to be in Aaa/Bbb/Ccc/Ddd.php,
   * but consider the PHP include_path setting.
   *
   * @param object $api
   *   The InjectedAPI object.
   * @param string $path_prefix_symbolic
   *   First part of the path, for instance "Aaa/Bbb/".
   * @param string $path_suffix
   *   Second part of the path, for instance "Ccc/Ddd.php".
   */
  function findFile($api, $path_prefix_symbolic, $path_suffix) {
    $path = $path_prefix_symbolic . $path_suffix;
    if ($api->suggestFile_checkNothing($path)) {
      return TRUE;
    }
  }
}
