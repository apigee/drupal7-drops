<h4 class="parent collapsed"><?php print check_plain($display_name); ?></h4>
<?php
if (!empty($description) && $description != 'description') {
  print '<p>' . check_plain($description) . '</p>';
}
?>