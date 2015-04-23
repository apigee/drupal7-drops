<ul class="menu method-index">
<?php
  foreach ($models as $model) {
    print '<li class="model ' . ($model['active_trail'] ? ' active-trail' : '') . '">';
    print check_plain($model['name']);
    print '<ul class="methods">';
    foreach ($model['resources'] as $resource) {
      // could optionally print $resource['path'] here
      foreach ($resource['methods'] as $method) {
        print '<li class="method ' . strtolower($method['verb']) . ($method['active'] ? ' active-trail active' : '') . '">';
        print l($method['name'], 'node/' . $method['nid']);
        print '</li>';
      }
    }
    print '</ul>';
    print '</li>';
  }
?>
</ul>