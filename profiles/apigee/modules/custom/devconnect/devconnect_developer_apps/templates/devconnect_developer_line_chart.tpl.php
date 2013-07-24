<?php
/**
 * @file
 *    Template file for creating an SVG line chart for app performance.
 * @author
 *    djohnson
 */

/*
 * Expected variables:
 *
 * $x_translate (int)
 *    Describes horizontal offset of actual chart portion of the SVG
 * $y_translate (int)
 *    Describes vertical offset of actual chart portion of the SVG
 * $vertical_lines (array of objects)
 *    Describes the vertical guidelines describing the X axis
 * $horizontal_lines (array of objects)
 *    Describes the horizontal guidelines describing the Y axis
 * $axis_overhang (int)
 *    How many pixels the X and Y axes overshoot the zero mark
 * $x_axis (object)
 *    Describes the X axis line and its caption
 * $y_axis (object)
 *    Describes the Y axis line and its caption
 * $y_axis_title (object)
 *    Describes the caption (rotated 90 degrees) of the Y axis.
 * $data_lines (array of objects)
 *    Each item in this array describes a data line.
 */
$x_index = $y_index = 0;
// If the legend is to be shown (which should only happen when more than one
// line is displayed), $legend_allowance is how much extra vertical space
// should be allocated.
$legend_allowance = 0;

// Calculate SVG width.
$svg_width = ceil($x_translate + $actual_chart_width + 25);
$legend = '';

// Create chart legend if appropriate. This consists of a <g> group containing
// a rounded rectangle, which in turn encloses a colored line and a text
// element caption for each line. This will usually be something like "test"
// or "prod".
// Chart legend should be centered within the chart.
if (count($data_lines) > 1) {
  $legend_allowance += 35;
  $legend_width = 55 * count($data_lines);
  $legend_left = floor($actual_chart_width / 2) - floor($legend_width / 2) + $x_translate;

  $legend = '<g class="analytics-chart-legend">'
    . '<rect x="' . $legend_left . '" y="1" height="20" width="' . $legend_width . '" rx="3" ry="3" />';
  $curr_x = $legend_left + 10;
  foreach ($data_lines as $line) {
    $left = $curr_x;
    $right = $curr_x + 10;
    $points = "$left,9 $right,9 $right,13 $left,13"; // make an explicit rectangle for sharper edges
    $legend .= '<polyline style="fill:' . $line->color . '" points="' . $points . '" />'
      . '<text x="' . ($curr_x + 13) . '" y="13">' . check_plain($line->caption) . '</text>';
    $curr_x += 55;
  }
  $legend .= '</g>';
}

$svg_height = ceil($x_axis->y + $y_translate + 45 + $legend_allowance);
?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="<?php print $svg_width;?>px" height="<?php print $svg_height; ?>px">
  <g>
    <clipPath id="analytics-chart-clip-rect">
      <rect x="0" y="0" width="<?php print $actual_chart_width; ?>" height="<?php print $actual_chart_height; ?>" />
    </clipPath>
  </g>

  <?php print $legend; ?>

  <g transform="translate(<?php print $x_translate;?>,<?php print ($y_translate + $legend_allowance); ?>)">
    <!-- vertical lines and x-axis captions -->
    <?php foreach ($vertical_lines as $line): ?>
    <line class="analytics-chart-vertical" x1="<?php print $line->x;?>" x2="<?php print $line->x;?>" y1="<?php print $line->y1;?>" y2="<?php print $line->y2 + $axis_overhang;?>" />
    <?php if (!empty($line->caption)): ?>
      <text class="analytics-chart-x-caption<?php if ($x_index == 0) print ' first';?>" x="<?php print $line->x;?>" y="<?php print ($line->y2 + 13); ?>"><?php print check_plain($line->caption); ?></text>
      <?php endif; ?>
    <?php if (!empty($line->subcaption)): ?>
      <text class="analytics-chart-x-subcaption<?php if ($x_index == 0) print ' first';?>" x="<?php print $line->x;?>" y="<?php print ($line->y2 + 23); ?>"><?php print check_plain($line->subcaption); ?></text>
      <?php endif; ?>
    <?php $x_index++; ?>
    <?php endforeach; ?>

    <!-- horizontal lines and y-axis captions -->
    <?php foreach ($horizontal_lines as $line): ?>
    <line class="analytics-chart-horizontal" x1="<?php print ($line->x1 - $axis_overhang);?>" x2="<?php print $line->x2;?>" y1="<?php print $line->y;?>" y2="<?php print $line->y;?>" />
    <?php if (!empty($line->caption)): ?>
      <text class="analytics-chart-y-caption<?php if ($y_index == 0) print ' first';?>" x="<?php print ($line->x1 - $axis_overhang - 2);?>" y="<?php print ($line->y + 2); ?>"><?php print check_plain($line->caption); ?></text>
      <?php endif; ?>
    <?php $y_index++; ?>
    <?php endforeach; ?>

    <!-- data lines -->
    <?php foreach ($data_lines as $line): ?>
    <polyline clip-path="url(#analytics-chart-clip-rect)" class="analytics-chart-data-line" style="stroke:<?php print $line->color; ?>" points="<?php
      foreach ($line->points as $point) {
        print $point->x . ',' . $point->y . ' ';
      }
      ?>" />
    <?php endforeach; ?>

    <!-- axes -->
    <line class="analytics-chart-x-axis" x1="<?php print $x_axis->x1 - $axis_overhang; ?>" x2="<?php print $x_axis->x2; ?>" y1="<?php print $x_axis->y; ?>" y2="<?php print $x_axis->y; ?>" />
    <line class="analytics-chart-y-axis" x1="<?php print $y_axis->x; ?>" x2="<?php print $y_axis->x; ?>" y1="<?php print $y_axis->y1; ?>" y2="<?php print $y_axis->y2 + $axis_overhang; ?>" />

    <text class="analytics-chart-y-axis-title" transform="rotate(-90 <?php print $y_axis_title->x; ?>,<?php print $y_axis_title->y; ?>)" x="<?php print $y_axis_title->x; ?>" y="<?php print $y_axis_title->y?>"><?php print check_plain($y_axis_title->caption); ?></text>
    <text class="analytics-chart-x-axis-title" x="<?php print floor($actual_chart_width / 2); ?>" y="<?php print ($x_axis->y + 38); ?>">Time (UTC)</text>

  </g>
</svg>