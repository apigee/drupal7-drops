core                                                           = 7.x
api                                                            = 2

; CONTRIB MODULES
; ------------------------------------------------------------------


; backup and migrate and fast 404 are deprecated on pantheon
; IMPORTANT:
; they should not be in the distro!!!!
;projects[backup_migrate][subdir]            = "contrib"
;projects[fast_404][subdir]                  = "contrib"
projects[acl][subdir]                        = "contrib"
projects[addressfield][subdir]               = "contrib"
projects[admin_menu][subdir]                 = "contrib"
projects[admin_views][subdir]                = "contrib"
projects[advanced_forum][subdir]             = "contrib"
projects[amazons3][subdir]                   = "contrib"
projects[apachesolr][subdir]                 = "contrib"
projects[apachesolr_stats][subdir]           = "contrib"
projects[autologout][subdir]                 = "contrib"
projects[awssdk][subdir]                     = "contrib"
projects[bean][subdir]                       = "contrib"
projects[block_class][subdir]                = "contrib"
projects[breakpoints][subdir]                = "contrib"
projects[captcha][subdir]                    = "contrib"
projects[cck_phone][subdir]                  = "contrib"
projects[cdn][subdir]                        = "contrib"
projects[chart][subdir]                      = "contrib"
projects[ckeditor][subdir]                   = "contrib"
projects[ckeditor_link][subdir]              = "contrib"
projects[commerce][subdir]                   = "contrib"
projects[commerce_custom_line_items][subdir] = "contrib"
projects[connector][subdir]                  = "contrib"
projects[contentapi][subdir]                 = "contrib"
projects[content_access][subdir]             = "contrib"
projects[context][subdir]                    = "contrib"
projects[contextual][subdir]                 = "contrib"
projects[ctools][subdir]                     = "contrib"
projects[custom_breadcrumbs][subdir]         = "contrib"
projects[date][subdir]                       = "contrib"
projects[defaultcontent][subdir]             = "contrib"
projects[devel][subdir]                      = "contrib"
projects[diff][subdir]                       = "contrib"
projects[download_file][subdir]              = "contrib"
projects[ds][subdir]                         = "contrib"
projects[eck][subdir]                        = "contrib"
projects[entitycache][subdir]                = "contrib"
projects[entityreference][subdir]            = "contrib"
projects[entity][subdir]                     = "contrib"
projects[environment_indicator][subdir]      = "contrib"
projects[facetapi][subdir]                   = "contrib"
projects[faq][subdir]                        = "contrib"
projects[features][subdir]                   = "contrib"
projects[features_extra][subdir]             = "contrib"
projects[field_group][subdir]                = "contrib"
projects[field_permissions][subdir]          = "contrib"
projects[file_entity][subdir]                = "contrib"
projects[file_entity_link][subdir]           = "contrib"
projects[flood_control][subdir]              = "contrib"
projects[fontyourface][subdir]               = "contrib"
projects[footermap][subdir]                  = "contrib"
projects[ftools][subdir]                     = "contrib"
projects[genpass][subdir]                    = "contrib"
projects[gist_filter][subdir]                = "contrib"
projects[github_connect][subdir]             = "contrib"
; permissions
; see https://drupal.org/node/2150767
projects[github_connect][patch][2150767]     = "https://drupal.org/files/issues/administer-github-connect-2150767-2.patch"
; unset $_GET['destination']
; see https://drupal.org/node/1895544
projects[github_connect][patch][1895544]     = "https://drupal.org/files/issues/1895544-github-connect-return-user-5.patch"
projects[google_analytics][subdir]           = "contrib"
projects[google_analytics_reports][subdir]   = "contrib"
projects[gravatar][subdir]                   = "contrib"
projects[highcharts][subdir]                 = "contrib"
projects[http_client][subdir]                = "contrib"
projects[i18n][subdir]                       = "contrib"
projects[imagemagick][subdir]                = "contrib"
projects[import_html][subdir]                = "contrib"
projects[job_scheduler][subdir]              = "contrib"
projects[jquerymobile][subdir]               = "contrib"
projects[jquery_colorpicker][subdir]         = "contrib"
projects[jquery_update][subdir]              = "contrib"
projects[l10n_update][subdir]                = "contrib"
projects[layout][subdir]                     = "contrib"
projects[ldap][subdir]                       = "contrib"
projects[legal][subdir]                      = "contrib"
projects[libraries][subdir]                  = "contrib"
projects[linkchecker][subdir]                = "contrib"
projects[link][subdir]                       = "contrib"
projects[link_node][subdir]                  = "contrib"
projects[linkit][subdir]                     = "contrib"
projects[logintoboggan][subdir]              = "contrib"
projects[markdown][subdir]                   = "contrib"
projects[mass_contact][subdir]               = "contrib"
projects[mediaelement][subdir]               = "contrib"
projects[media][subdir]                      = "contrib"
projects[media_youtube][subdir]              = "contrib"
projects[menu_trail_by_path][subdir]         = "contrib"
projects[message][subdir]                    = "contrib"
projects[metatags_quick][subdir]             = "contrib"
projects[metatag][subdir]                    = "contrib"
projects[me][subdir]                         = "contrib"
projects[migrate][subdir]                    = "contrib"
projects[module_filter][subdir]              = "contrib"
projects[nagios][subdir]                     = "contrib"
projects[node_export][subdir]                = "contrib"
projects[nra][subdir]                        = "contrib"
projects[oauthconnector][subdir]             = "contrib"
projects[oauth][subdir]                      = "contrib"
projects[page_title][subdir]                 = "contrib"
projects[panelizer][subdir]                  = "contrib"
projects[panels][subdir]                     = "contrib"
projects[panels_everywhere][subdir]          = "contrib"
projects[password_policy][subdir]            = "contrib"
projects[pathauto][subdir]                   = "contrib"
projects[permission_grid][subdir]            = "contrib"
projects[prlp][subdir]                       = "contrib"
projects[readonlymode][subdir]               = "contrib"
projects[recaptcha][subdir]                  = "contrib"
projects[redirect][subdir]                   = "contrib"
projects[redis][subdir]                      = "contrib"
projects[responsive_preview][subdir]         = "contrib"
projects[role_export][subdir]                = "contrib"
projects[rules][subdir]                      = "contrib"
projects[rules_forms][subdir]                = "contrib"
projects[rules_conditional][subdir]          = "contrib"
projects[services][subdir]                   = "contrib"
projects[services_views][subdir]             = "contrib"
projects[smtp][subdir]                       = "contrib"
projects[special_menu_items][subdir]         = "contrib"
projects[sps][subdir]                        = "contrib"
projects[shield][subdir]                     = "contrib"
projects[statsd][subdir]                     = "contrib"
projects[strongarm][subdir]                  = "contrib"
projects[syntaxhighlighter][subdir]          = "contrib"
projects[taxonomy_access][subdir]            = "contrib"
projects[token][subdir]                      = "contrib"
projects[translation_overview][subdir]       = "contrib"
projects[twitter][subdir]                    = "contrib"
projects[util][subdir]                       = "contrib"
projects[uuid][subdir]                       = "contrib"
projects[variable][subdir]                   = "contrib"
projects[views][subdir]                      = "contrib"
projects[views_accordion][subdir]            = "contrib"
projects[views_bulk_operations][subdir]      = "contrib"
projects[views_rules][subdir]                = "contrib"
projects[views_slideshow][subdir]            = "contrib"
projects[webform][subdir]                    = "contrib"
projects[weight][subdir]                     = "contrib"
projects[workbench][subdir]                  = "contrib"
projects[workbench_moderation][subdir]       = "contrib"
projects[workbench_moderation_notes][subdir] = "contrib"
projects[workflow][subdir]                   = "contrib"
projects[xautoload][subdir]                  = "contrib"
projects[XHProf][subdir]                     = "contrib"





projects[commerce_worldpay_business_gateway][subdir]           = "contrib"
projects[commerce_worldpay_business_gateway][type]             = module
projects[commerce_worldpay_business_gateway][download][type]   = git
projects[commerce_worldpay_business_gateway][download][branch] = "7.x-1.x"
projects[commerce_worldpay_business_gateway][download][url]    = http://git.drupal.org/sandbox/magicmyth/1433370.git
projects[commerce_worldpay_business_gateway][patch][1569386]   = https://drupal.org/files/DRAFT-Issue-1569386.patch

; Assemble
projects[assemble][version]                                    = "1.0"
projects[assemble][type]                                       = "module"
projects[assemble][subdir]                                     = "contrib"
projects[assemble][download][type]                             = "git"
projects[assemble][download][revision]                         = "b89e60c"
projects[assemble][download][branch]                           = "7.x-1.x"

projects[edit][version]                                        = "1.x-dev"
projects[edit][type]                                           = "module"
projects[edit][subdir]                                         = "contrib"
projects[edit][download][type]                                 = "git"
projects[edit][download][revision]                             = "cf62974"
projects[edit][download][branch]                               = "7.x-1.x"
; Backport of Edit button for navbar
; http://drupal.org/node/1994256
projects[edit][patch][1994256]                                 = "http://drupal.org/files/edit-navbar-button-1994256-15.patch"
; Edit Module fails for "psudeo" fields provided via Relationship or Appended
; Global Text in Views
; http://drupal.org/node/2015295
projects[edit][patch][2015295]                                 = "http://drupal.org/files/edit-views-psuedo-fields-2015295-6.patch"

projects[timeago][version]                                     = "2.x-dev"
projects[timeago][type]                                        = "module"
projects[timeago][subdir]                                      = "contrib"
projects[timeago][download][type]                              = "git"
projects[timeago][download][url]                               = "http://git.drupal.org/project/timeago.git"
projects[timeago][download][branch]                            = "7.x-2.x"
projects[timeago][download][revision]                          = "768ea66"
; Provide a dedicated date type
; http://drupal.org/node/1427226
projects[timeago][patch][1427226]                              = "http://drupal.org/files/1427226-timeago-date-type.patch"

projects[lingotek][version]                                    = "4.x-dev"
projects[lingotek][type]                                       = "module"
projects[lingotek][subdir]                                     = "contrib"
projects[lingotek][download][type]                             = "git"
projects[lingotek][download][revision]                         = "abdc289"
projects[lingotek][download][branch]                           = "7.x-4.x"
; Update Lingotek navbar.css to reflect changes to navbar.base.css
; http://drupal.org/node/2054903
projects[lingotek][patch][2054903]                             = "http://drupal.org/files/navbar-css-changes-2054903.patch"

projects[revision_scheduler][version]                          = "1.x-dev"
projects[revision_scheduler][type]                             = "module"
projects[revision_scheduler][subdir]                           = "contrib"
projects[revision_scheduler][download][type]                   = "git"
projects[revision_scheduler][download][revision]               = "ab04410"
projects[revision_scheduler][download][branch]                 = "7.x-1.x"

projects[collections][version]                                 = "1.x-dev"
projects[collections][type]                                    = "module"
projects[collections][subdir]                                  = "contrib"
projects[collections][download][type]                          = "git"
projects[collections][download][revision]                      = "b4e8212"
projects[collections][download][branch]                        = "7.x-1.x"

; Curate
projects[curate][version]                                      = "1.0"
projects[curate][type]                                         = "module"
projects[curate][subdir]                                       = "contrib"
projects[curate][download][type]                               = "git"
projects[curate][download][revision]                           = "679deb0"
projects[curate][download][branch]                             = "7.x-1.x"


projects[iib][version]                                         = "1.x-dev"
projects[iib][type]                                            = "module"
projects[iib][subdir]                                          = "contrib"
projects[iib][download][type]                                  = "git"
projects[iib][download][revision]                              = "17a55eb"
projects[iib][download][branch]                                = "7.x-1.x"
; UX Improvements
; http://drupal.org/node/1737036
projects[iib][patch][1737036]                                  = "http://drupal.org/files/iib-entity-css-1737036-18.patch"

projects[file_entity_link][subdir]                             = "contrib"
projects[file_entity_link][version]                            = "1.0-alpha3"

projects[uuid_features][subdir]                                = "contrib"
projects[uuid_features][version]                               = "1.x-dev"



; THEMES
; ------------------------------------------------------------------
; apigee base theme
projects[apigee_base][type]                                    = "theme"
projects[apigee_base][download][type]                          = 'git'
projects[apigee_base][download][url]                           = "git@github.com:apigee/apigee_drupal_base_theme.git"
projects[apigee_base][download][branch]                        = "devconnect"

; apigee devconnect theme
projects[apigee_devconnect][type]                              = "theme"
projects[apigee_devconnect][download][type]                    = 'git'
projects[apigee_devconnect][download][url]                     = "git@github.com:apigee/apigee_drupal_devconnect_theme.git"
projects[apigee_devconnect][download][branch]                  = "7.x-4.24"

; development seed's admin theme
projects[tao][type]                                            = "theme"
projects[rubik][type]                                          = "theme"
projects[bootstrap][type]                                      = "theme"


; CUSTOM MODULES
; ------------------------------------------------------------------

;d8cmi
projects[d8cmi][type]                                          = module
projects[d8cmi][subdir]                                        = custom
projects[d8cmi][download][type]                                = 'git'
projects[d8cmi][download][url]                                 = "http://git.drupal.org/sandbox/daniel_j/2122587.git"
projects[d8cmi][directory_name]                                = d8cmi

;devconnect
projects[devconnect][type]                                     = module
projects[devconnect][subdir]                                   = custom
projects[devconnect][download][type]                           = 'git'
projects[devconnect][download][url]                            = "git@github.com:apigee/devconnect.git"
projects[devconnect][download][branch]                         = "7.x-4.24"

; apigee SSO module
projects[apigee_sso][type]                                     = module
projects[apigee_sso][subdir]                                   = custom
projects[apigee_sso][download][type]                           = 'git'
projects[apigee_sso][download][url]                            = "git@github.com:apigee/apigee_drupal_sso.git"
;projects[apigee_sso][download][branch]                         = "master"

; Libraries
; ------------------------------------------------------------------
; unfortunately, we can't link directly to a /download/latest
; for some and the URL's will need to be updated
; by hand when security updates are posted


libraries[backbone][download][type]          = "get"
libraries[backbone][download][url]           = "http://documentcloud.github.io/backbone/backbone-min.js"
libraries[backbone][destination]             = "libraries"

libraries[ckeditor][destination]             = "libraries"
libraries[ckeditor][directory_name]          = "ckeditor"
libraries[ckeditor][download][type]          = "get"
libraries[ckeditor][download][url]           = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%204.2.1/ckeditor_4.2.1_standard.zip"

libraries[colorpicker][destination]          = "libraries"
libraries[colorpicker][directory_name]       = "colorpicker"
libraries[colorpicker][download][type]       = 'get'
libraries[colorpicker][download][url]        = 'http://www.eyecon.ro/colorpicker/colorpicker.zip'

libraries[highcharts][destination]           = "libraries"
libraries[highcharts][directory_name]        = "highcharts"
libraries[highcharts][download][type]        = "get"
libraries[highcharts][download][url]         = "http://code.highcharts.com/zips/Highcharts-3.0.4.zip"

libraries[jquery_cycle][destination]         = "libraries"
libraries[jquery_cycle][directory_name]      = "jquery.cycle"
libraries[jquery_cycle][download][type]      = "get"
libraries[jquery_cycle][download][url]       = "http://www.malsup.com/jquery/cycle/release/jquery.cycle.zip?v2.86"

libraries[jquery_selectlist][destination]    = "libraries"
libraries[jquery_selectlist][directory_name] = "jquery.selectlist"
libraries[jquery_selectlist][download][type] = "get"
libraries[jquery_selectlist][download][url]  = "http://odyniec.net/projects/selectlist/jquery.selectlist-0.5.1.zip"

libraries[json2][destination]                = "libraries"
libraries[json2][directory_name]             = "json2"
libraries[json2][download][type]             = "git"
libraries[json2][download][url]              = "git://github.com/douglascrockford/JSON-js.git"

libraries[jsonpath][download][type]          = "get"
libraries[jsonpath][download][url]           = "https://jsonpath.googlecode.com/files/jsonpath-0.8.1.php"
libraries[jsonpath][destination]             = "libraries"

libraries[maskmoney][download][type]         = "get"
libraries[maskmoney][download][url]          = "https://raw.github.com/plentz/jquery-maskmoney/master/src/jquery.maskMoney.js"
libraries[maskmoney][destination]            = "libraries"

libraries[mediaelement][destination]         = 'libraries'
libraries[mediaelement][directory_name]      = 'mediaelement'
libraries[mediaelement][download][type]      = "git"
libraries[mediaelement][download][url]       = 'git://github.com/johndyer/mediaelement.git'

libraries[plupload][download][type]          = "get"
libraries[plupload][download][url]           = "https://github.com/moxiecode/plupload/archive/v2.0.0.zip"
libraries[plupload][destination]             = "libraries"

libraries[respondjs][download][type]         = "get"
libraries[respondjs][download][url]          = "https://github.com/scottjehl/Respond/tarball/master"
libraries[respondjs][destination]            = "libraries"

libraries[SolrPhpClient][destination]        = "libraries"
libraries[SolrPhpClient][directory_name]     = "SolrPhpClient"
libraries[SolrPhpClient][download][type]     = "get"
libraries[SolrPhpClient][download][url]      = "http://solr-php-client.googlecode.com/files/SolrPhpClient.r60.2011-05-04.zip"

libraries[syntaxhighlighter][destination]    = 'libraries'
libraries[syntaxhighlighter][directory_name] = "syntaxhighlighter"
libraries[syntaxhighlighter][download][type] = 'get'
libraries[syntaxhighlighter][download][url]  = 'http://alexgorbatchev.com/SyntaxHighlighter/download/download.php?sh_current'

libraries[timeago][download][type]           = "get"
libraries[timeago][download][url]            = "https://raw.github.com/rmm5t/jquery-timeago/v1.3.0/jquery.timeago.js"
libraries[timeago][destination]              = "libraries"

libraries[underscore][download][type]        = "get"
libraries[underscore][download][url]         = "http://documentcloud.github.io/underscore/underscore-min.js"
libraries[underscore][destination]           = "libraries"



