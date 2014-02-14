<?php if ($visible) { ?>
  <div class="row hidden-xs">
    <nav role="navigation" class="navbar navbar-default">
      <div class="navbar-header">
        <a class="navbar-brand">Create</a>
      </div>
      <ul class="nav navbar-nav">
        <?php foreach($types as $type => $glyph) { ?>
          <div class="btn navbar-btn" style="margin-right:10px;">
            <span class="glyphicon <?php print $glyph; ?>"></span>
            <?php print $type; ?>
          </div>
        <?php } ?>
      </ul>
    </nav>
  </div>
  <div class="visible-xs">
    <div style="margin: 0 auto 10px;" class="well">
      <?php foreach($types as $type => $glyph) { ?>
        <div class="btn btn-primary btn-lg btn-block" style="margin-right:10px;">
          <span class="glyphicon <?php print $glyph; ?>"></span>
          Create <?php print $type; ?>
        </div>
      <?php } ?>
    </div>
  </div>
  <hr>
<?php } ?>