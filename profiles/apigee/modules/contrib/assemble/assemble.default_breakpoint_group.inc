<?php
/**
 * @file
 * assemble.default_breakpoint_group.inc
 */

/**
 * Implements hook_default_breakpoint_group().
 */
function assemble_default_breakpoint_group() {
  $export = array();

  // Breakpoints.
  $breakpoints = array();
  $breakpoints[] = 'custom.user.mobile';
  $breakpoints[] = 'custom.user.tablet';
  $breakpoints[] = 'custom.user.narrow';
  $breakpoints[] = 'breakpoints.module.navbar.wide';

  // Breakpoint group.
  $breakpoint_group = new stdClass();
  $breakpoint_group->disabled = FALSE; /* Edit this to true to make a default breakpoint_group disabled initially */
  $breakpoint_group->api_version = 1;
  $breakpoint_group->machine_name = 'letterbox';
  $breakpoint_group->name = 'DF Letterbox';
  $breakpoint_group->breakpoints = $breakpoints;
  $breakpoint_group->type = 'custom';
  $breakpoint_group->overridden = 0;
  $export['letterbox'] = $breakpoint_group;

  return $export;
}
