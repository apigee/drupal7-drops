core                                                           = 7.x
api                                                            = 2

; CONTRIB MODULES
; ------------------------------------------------------------------


; backup and migrate and fast 404 are deprecated on pantheon
; IMPORTANT:
; they should not be in the distro!!!!
;projects[backup_migrate][subdir]                              = "contrib"
;projects[fast_404][subdir]                                    = "contrib"
projects[acl][subdir]                                          = "contrib"
projects[addressfield][subdir]                                 = "contrib"
projects[admin_menu][subdir]                                   = "contrib"
projects[admin_views][subdir]                                  = "contrib"
projects[advanced_forum][subdir]                               = "contrib"
projects[amazons3][subdir]                                     = "contrib"
projects[apachesolr][subdir]                                   = "contrib"
projects[apachesolr_stats][subdir]                             = "contrib"
projects[autologout][subdir]                                   = "contrib"
projects[awssdk][subdir]                                       = "contrib"
projects[bean][subdir]                                         = "contrib"
projects[block_class][subdir]                                  = "contrib"
projects[breakpoints][subdir]                                  = "contrib"
projects[bugherd][subdir]                                      = "contrib"
projects[captcha][subdir]                                      = "contrib"
projects[cck_phone][subdir]                                    = "contrib"
projects[cdn][subdir]                                          = "contrib"
projects[chart][subdir]                                        = "contrib"
projects[ckeditor][subdir]                                     = "contrib"
projects[ckeditor_link][subdir]                                = "contrib"
projects[commerce][subdir]                                     = "contrib"
projects[commerce_custom_line_items][subdir]                   = "contrib"
projects[connector][subdir]                                    = "contrib"
projects[contentapi][subdir]                                   = "contrib"
projects[content_access][subdir]                               = "contrib"
projects[context][subdir]                                      = "contrib"
projects[context_condition_theme][subdir]                      = "contrib"
projects[ctools][subdir]                                       = "contrib"
projects[custom_breadcrumbs][subdir]                           = "contrib"
projects[date][subdir]                                         = "contrib"
projects[defaultcontent][subdir]                               = "contrib"
projects[delete_all][subdir]                                   = "contrib"
projects[devel][subdir]                                        = "contrib"
projects[diff][subdir]                                         = "contrib"
projects[download_file][subdir]                                = "contrib"
projects[ds][subdir]                                           = "contrib"
projects[eck][subdir]                                          = "contrib"
projects[entitycache][subdir]                                  = "contrib"
projects[entityreference][subdir]                              = "contrib"
; views plugins
; see https://drupal.org/node/2170193
projects[entityreference][patch][2170193]                      = "https://drupal.org/files/issues/entityreference-2170193-3-plugin-paths.patch"
projects[entity][subdir]                                       = "contrib"
projects[environment_indicator][subdir]                        = "contrib"
projects[facetapi][subdir]                                     = "contrib"
projects[faq][subdir]                                          = "contrib"
projects[features][subdir]                                     = "contrib"
projects[features_extra][subdir]                               = "contrib"
projects[field_group][subdir]                                  = "contrib"
projects[field_permissions][subdir]                            = "contrib"
projects[file_entity][subdir]                                  = "contrib"
projects[file_entity_link][subdir]                             = "contrib"
projects[flood_control][subdir]                                = "contrib"
projects[fontyourface][subdir]                                 = "contrib"
projects[footermap][subdir]                                    = "contrib"
projects[ftools][subdir]                                       = "contrib"
projects[genpass][subdir]                                      = "contrib"
projects[gist_filter][subdir]                                  = "contrib"
projects[gist_filter][version]                                 = "1.x-dev"
; see https://drupal.org/node/2114559
projects[gist_filter][patch][2114559]                          = "https://drupal.org/files/gist_filter-embedding_on_https_sites-2114559-1.patch"
; see https://drupal.org/node/1957754
projects[gist_filter][patch][1957754]                          = "https://drupal.org/files/regex-not-working-1957754-1.patch"
projects[github_connect][subdir]                               = "contrib"
; permissions
; see https://drupal.org/node/2150767
projects[github_connect][patch][2150767]                       = "https://drupal.org/files/issues/administer-github-connect-2150767-2.patch"
; unset $_GET['destination']
; see https://drupal.org/node/1895544
projects[github_connect][patch][1895544]                       = "https://drupal.org/files/issues/1895544-github-connect-return-user-5.patch"
; see https://drupal.org/node/2266675
projects[github_connect][patch][2266675]                       = "https://drupal.org/files/issues/github_connect-email-api-change-2266675-1.patch"
projects[google_analytics][subdir]                             = "contrib"
projects[google_analytics_reports][subdir]                     = "contrib"
projects[gravatar][subdir]                                     = "contrib"
projects[highcharts][subdir]                                   = "contrib"
projects[http_client][subdir]                                  = "contrib"
projects[i18n][subdir]                                         = "contrib"
projects[imagemagick][subdir]                                  = "contrib"
projects[import_html][subdir]                                  = "contrib"
projects[job_scheduler][subdir]                                = "contrib"
projects[jquerymobile][subdir]                                 = "contrib"
projects[jquery_colorpicker][subdir]                           = "contrib"
projects[jquery_update][subdir]                                = "contrib"
projects[jquery_update][version]                               = "2.3"
projects[l10n_update][subdir]                                  = "contrib"
projects[layout][subdir]                                       = "contrib"
projects[ldap][subdir]                                         = "contrib"
projects[legal][subdir]                                        = "contrib"
projects[libraries][subdir]                                    = "contrib"
projects[linkchecker][subdir]                                  = "contrib"
projects[link][subdir]                                         = "contrib"
projects[link_node][subdir]                                    = "contrib"
projects[linkit][subdir]                                       = "contrib"
projects[logintoboggan][subdir]                                = "contrib"
projects[mailsystem][subdir]                                   = "contrib"
projects[markdown][subdir]                                     = "contrib"
projects[mass_contact][subdir]                                 = "contrib"
projects[mediaelement][subdir]                                 = "contrib"
projects[media][subdir]                                        = "contrib"
projects[media][version]                                       = "2.0-alpha3"
; don't break views_view.tpl.php
; see https://drupal.org/node/2232703
projects[media][patch][2232703] = "https://drupal.org/files/issues/media-views-2232703-5.patch"
projects[media_youtube][subdir]                                = "contrib"
projects[menu_attributes][subdir]                              = "contrib"
projects[menu_trail_by_path][subdir]                           = "contrib"
projects[message][subdir]                                      = "contrib"
projects[metatags_quick][subdir]                               = "contrib"
projects[metatag][subdir]                                      = "contrib"
projects[me][subdir]                                           = "contrib"
projects[migrate][subdir]                                      = "contrib"
projects[mimemail][subdir]                                     = "contrib"
projects[module_filter][subdir]                                = "contrib"
projects[nagios][subdir]                                       = "contrib"
projects[navbar][subdir]                                       = "contrib"
projects[node_export][subdir]                                  = "contrib"
projects[nra][subdir]                                          = "contrib"
projects[oauthconnector][subdir]                               = "contrib"
projects[oauth][subdir]                                        = "contrib"
projects[page_title][subdir]                                   = "contrib"
projects[panelizer][subdir]                                    = "contrib"
projects[panels][subdir]                                       = "contrib"
projects[panels_everywhere][subdir]                            = "contrib"
projects[password_policy][subdir]                              = "contrib"
projects[pathauto][subdir]                                     = "contrib"
projects[permission_grid][subdir]                              = "contrib"
projects[prlp][subdir]                                         = "contrib"
projects[readonlymode][subdir]                                 = "contrib"
projects[recaptcha][subdir]                                    = "contrib"
projects[redirect][subdir]                                     = "contrib"
projects[redis][subdir]                                        = "contrib"
projects[responsive_preview][subdir]                           = "contrib"
projects[role_export][subdir]                                  = "contrib"
projects[remote_stream_wrapper][subdir]                        = "contrib"
projects[rules][subdir]                                        = "contrib"
projects[rules_forms][subdir]                                  = "contrib"
projects[rules_conditional][subdir]                            = "contrib"
projects[services][subdir]                                     = "contrib"
projects[securepages][subdir]                                  = "contrib"
projects[services_views][subdir]                               = "contrib"
projects[smtp][subdir]                                         = "contrib"
projects[special_menu_items][subdir]                           = "contrib"
projects[sps][subdir]                                          = "contrib"
projects[shield][subdir]                                       = "contrib"
projects[statsd][subdir]                                       = "contrib"
projects[strongarm][subdir]                                    = "contrib"
projects[syntaxhighlighter][subdir]                            = "contrib"
projects[taxonomy_access][subdir]                              = "contrib"
projects[token][subdir]                                        = "contrib"
projects[translation_overview][subdir]                         = "contrib"
projects[twitter][subdir]                                      = "contrib"
projects[util][subdir]                                         = "contrib"
projects[uuid][subdir]                                         = "contrib"
projects[variable][subdir]                                     = "contrib"
projects[views][subdir]                                        = "contrib"
projects[views_accordion][subdir]                              = "contrib"
projects[views_bulk_operations][subdir]                        = "contrib"
projects[views_rules][subdir]                                  = "contrib"
projects[views_slideshow][subdir]                              = "contrib"
projects[webform][subdir]                                      = "contrib"
projects[weight][subdir]                                       = "contrib"
projects[workbench][subdir]                                    = "contrib"
projects[workbench_moderation][subdir]                         = "contrib"
projects[workbench_moderation_notes][subdir]                   = "contrib"
projects[workflow][subdir]                                     = "contrib"
; xautoload is required by sps
projects[xautoload][subdir]                                    = "contrib"
projects[XHProf][subdir]                                       = "contrib"

projects[ckeditor_bootstrap][type]                             = module
projects[ckeditor_bootstrap][subdir]                           = "contrib"
projects[ckeditor_bootstrap][version]                          = 1.0-alpha1

projects[commerce_worldpay_business_gateway][subdir]           = "contrib"
projects[commerce_worldpay_business_gateway][type]             = module
projects[commerce_worldpay_business_gateway][download][type]   = git
projects[commerce_worldpay_business_gateway][download][branch] = "7.x-1.x"
projects[commerce_worldpay_business_gateway][download][url]    = http://git.drupal.org/sandbox/magicmyth/1433370.git
projects[commerce_worldpay_business_gateway][patch][1569386]   = https://drupal.org/files/DRAFT-Issue-1569386.patch

projects[bootstrap_modal_forms][type]                          = module
projects[bootstrap_modal_forms][subdir]                        = contrib
projects[bootstrap_modal_forms][download][type]                = 'git'
projects[bootstrap_modal_forms][download][url]                 = "http://git.drupal.org/sandbox/bhasselbeck/2167991.git"
projects[bootstrap_modal_forms][directory_name]                = bootstrap_modal_forms

; Assemble
projects[assemble][version]                                    = "1.0"
projects[assemble][type]                                       = "module"
projects[assemble][subdir]                                     = "contrib"
projects[assemble][download][type]                             = "git"
projects[assemble][download][revision]                         = "b89e60c"
projects[assemble][download][branch]                           = "7.x-1.x"

;projects[edit][version]                                        = "1.x-dev"
;projects[edit][type]                                           = "module"
;projects[edit][subdir]                                         = "contrib"
;projects[edit][download][type]                                 = "git"
;projects[edit][download][revision]                             = "cf62974"
;projects[edit][download][branch]                               = "7.x-1.x"
; Backport of Edit button for navbar
; http://drupal.org/node/1994256
;projects[edit][patch][1994256]                                 = "http://drupal.org/files/edit-navbar-button-1994256-15.patch"
; Edit Module fails for "pseudo" fields provided via Relationship or Appended
; Global Text in Views
; http://drupal.org/node/2015295
;projects[edit][patch][2015295]                                 = "http://drupal.org/files/edit-views-psuedo-fields-2015295-6.patch"

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

projects[apigee_responsive][type]                              = "theme"
projects[apigee_responsive][download][type]                    = 'git'
projects[apigee_responsive][download][url]                     = "http://git.drupal.org/sandbox/bhasselbeck/2168189.git"
projects[apigee_responsive][directory_name]                    = apigee_responsive

; development seed's admin theme
projects[tao][type]                                            = "theme"

projects[rubik][type]                                          = "theme"
projects[rubik][version]                                       = "4.x-dev"

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
projects[devconnect][download][branch]                         = "7.x-14.06"

; apigee SSO module
projects[apigee_sso][type]                                     = module
projects[apigee_sso][subdir]                                   = custom
projects[apigee_sso][download][type]                           = 'git'
projects[apigee_sso][download][url]                            = "git@github.com:apigee/apigee_drupal_sso.git"
projects[apigee_sso][download][branch]                         = "7.x-14.04"



; CKEditor Plugins
; ------------------------------------------------------------------
; These plugins will live in the libraries directory, but the main ckeditor instance will use them
; These also match D8 standards.

; Actual CKEditor
libraries[ckeditor][destination]                               = "libraries"
libraries[ckeditor][directory_name]                            = "ckeditor"
libraries[ckeditor][download][type]                            = "get"
libraries[ckeditor][download][url]                             = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%204.3.2/ckeditor_4.3.2_full.zip"

libraries[about][destination]                                  = "libraries"
libraries[about][directory_name]                               = "ckeditor/plugins/about"
libraries[about][download][type]                               = "get"
libraries[about][download][url]                                = "http://download.ckeditor.com/about/releases/about_4.3.2.zip"

libraries[a11yhelp][destination]                               = "libraries"
libraries[a11yhelp][directory_name]                            = "ckeditor/plugins/a11yhelp"
libraries[a11yhelp][download][type]                            = "get"
libraries[a11yhelp][download][url]                             = "http://download.ckeditor.com/a11yhelp/releases/a11yhelp_4.3.2.zip"

libraries[basicstyles][destination]                            = "libraries"
libraries[basicstyles][directory_name]                         = "ckeditor/plugins/basicstyles"
libraries[basicstyles][download][type]                         = "get"
libraries[basicstyles][download][url]                          = "http://download.ckeditor.com/basicstyles/releases/basicstyles_4.3.2.zip"

libraries[blockquote][destination]                             = "libraries"
libraries[blockquote][directory_name]                          = "ckeditor/plugins/blockquote"
libraries[blockquote][download][type]                          = "get"
libraries[blockquote][download][url]                           = "http://download.ckeditor.com/blockquote/releases/blockquote_4.3.2.zip"

libraries[clipboard][destination]                              = "libraries"
libraries[clipboard][directory_name]                           = "ckeditor/plugins/clipboard"
libraries[clipboard][download][type]                           = "get"
libraries[clipboard][download][url]                            = "http://download.ckeditor.com/clipboard/releases/clipboard_4.3.2.zip"

libraries[contextmenu][destination]                            = "libraries"
libraries[contextmenu][directory_name]                         = "ckeditor/plugins/contextmenu"
libraries[contextmenu][download][type]                         = "get"
libraries[contextmenu][download][url]                          = "http://download.ckeditor.com/contextmenu/releases/contextmenu_4.3.2.zip"

libraries[toolbarswitch][destination]                          = "libraries"
libraries[toolbarswitch][directory_name]                       = "ckeditor/plugins/toolbarswitch"
libraries[toolbarswitch][download][type]                       = "get"
libraries[toolbarswitch][download][url]                        = "http://download.ckeditor.com/toolbarswitch/releases/toolbarswitch_4.3.2.zip"

libraries[elementspath][destination]                           = "libraries"
libraries[elementspath][directory_name]                        = "ckeditor/plugins/elementspath"
libraries[elementspath][download][type]                        = "get"
libraries[elementspath][download][url]                         = "http://download.ckeditor.com/elementspath/releases/elementspath_4.3.2.zip"

libraries[enterkey][destination]                               = "libraries"
libraries[enterkey][directory_name]                            = "ckeditor/plugins/enterkey"
libraries[enterkey][download][type]                            = "get"
libraries[enterkey][download][url]                             = "http://download.ckeditor.com/enterkey/releases/enterkey_4.3.2.zip"

libraries[entities][destination]                               = "libraries"
libraries[entities][directory_name]                            = "ckeditor/plugins/entities"
libraries[entities][download][type]                            = "get"
libraries[entities][download][url]                             = "http://download.ckeditor.com/entities/releases/entities_4.3.2.zip"

libraries[filebrowser][destination]                            = "libraries"
libraries[filebrowser][directory_name]                         = "ckeditor/plugins/filebrowser"
libraries[filebrowser][download][type]                         = "get"
libraries[filebrowser][download][url]                          = "http://download.ckeditor.com/filebrowser/releases/filebrowser_4.3.2.zip"

libraries[floatingspace][destination]                          = "libraries"
libraries[floatingspace][directory_name]                       = "ckeditor/plugins/floatingspace"
libraries[floatingspace][download][type]                       = "get"
libraries[floatingspace][download][url]                        = "http://download.ckeditor.com/floatingspace/releases/floatingspace_4.3.2.zip"

libraries[htmlwriter][destination]                             = "libraries"
libraries[htmlwriter][directory_name]                          = "ckeditor/plugins/htmlwriter"
libraries[htmlwriter][download][type]                          = "get"
libraries[htmlwriter][download][url]                           = "http://download.ckeditor.com/htmlwriter/releases/htmlwriter_4.3.2.zip"

libraries[horizontalrule][destination]                         = "libraries"
libraries[horizontalrule][directory_name]                      = "ckeditor/plugins/horizontalrule"
libraries[horizontalrule][download][type]                      = "get"
libraries[horizontalrule][download][url]                       = "http://download.ckeditor.com/horizontalrule/releases/horizontalrule_4.3.2.zip"

libraries[wysiwygarea][destination]                            = "libraries"
libraries[wysiwygarea][directory_name]                         = "ckeditor/plugins/wysiwygarea"
libraries[wysiwygarea][download][type]                         = "get"
libraries[wysiwygarea][download][url]                          = "http://download.ckeditor.com/wysiwygarea/releases/wysiwygarea_4.3.2.zip"

libraries[indent][destination]                                 = "libraries"
libraries[indent][directory_name]                              = "ckeditor/plugins/indent"
libraries[indent][download][type]                              = "get"
libraries[indent][download][url]                               = "http://download.ckeditor.com/indent/releases/indent_4.3.2.zip"

libraries[iframe][destination]                                 = "libraries"
libraries[iframe][directory_name]                              = "ckeditor/plugins/iframe"
libraries[iframe][download][type]                              = "get"
libraries[iframe][download][url]                               = "http://download.ckeditor.com/iframe/releases/iframe_4.3.2.zip"

libraries[image][destination]                                  = "libraries"
libraries[image][directory_name]                               = "ckeditor/plugins/image"
libraries[image][download][type]                               = "get"
libraries[image][download][url]                                = "http://download.ckeditor.com/image/releases/image_4.3.2.zip"

libraries[popup][destination]                                  = "libraries"
libraries[popup][directory_name]                               = "ckeditor/plugins/popup"
libraries[popup][download][type]                               = "get"
libraries[popup][download][url]                                = "http://download.ckeditor.com/popup/releases/popup_4.3.2.zip"

libraries[dialog][destination]                                 = "libraries"
libraries[dialog][directory_name]                              = "ckeditor/plugins/dialog"
libraries[dialog][download][type]                              = "get"
libraries[dialog][download][url]                               = "http://download.ckeditor.com/dialog/releases/dialog_4.3.2.zip"

libraries[panel][destination]                                  = "libraries"
libraries[panel][directory_name]                               = "ckeditor/plugins/panel"
libraries[panel][download][type]                               = "get"
libraries[panel][download][url]                                = "http://download.ckeditor.com/panel/releases/panel_4.3.2.zip"

libraries[menu][destination]                                   = "libraries"
libraries[menu][directory_name]                                = "ckeditor/plugins/menu"
libraries[menu][download][type]                                = "get"
libraries[menu][download][url]                                 = "http://download.ckeditor.com/menu/releases/menu_4.3.2.zip"

libraries[menubutton][destination]                             = "libraries"
libraries[menubutton][directory_name]                          = "ckeditor/plugins/menubutton"
libraries[menubutton][download][type]                          = "get"
libraries[menubutton][download][url]                           = "http://download.ckeditor.com/menubutton/releases/menubutton_4.3.2.zip"

libraries[menubutton][destination]                             = "libraries"
libraries[menubutton][directory_name]                          = "ckeditor/plugins/menubutton"
libraries[menubutton][download][type]                          = "get"
libraries[menubutton][download][url]                           = "http://download.ckeditor.com/menubutton/releases/menubutton_4.3.2.zip"

libraries[dialogui][destination]                               = "libraries"
libraries[dialogui][directory_name]                            = "ckeditor/plugins/dialogui"
libraries[dialogui][download][type]                            = "get"
libraries[dialogui][download][url]                             = "http://download.ckeditor.com/dialogui/releases/dialogui_4.3.2.zip"

libraries[floatpanel][destination]                             = "libraries"
libraries[floatpanel][directory_name]                          = "ckeditor/plugins/floatpanel"
libraries[floatpanel][download][type]                          = "get"
libraries[floatpanel][download][url]                           = "http://download.ckeditor.com/floatpanel/releases/floatpanel_4.3.2.zip"

libraries[fakeobjects][destination]                            = "libraries"
libraries[fakeobjects][directory_name]                         = "ckeditor/plugins/fakeobjects"
libraries[fakeobjects][download][type]                         = "get"
libraries[fakeobjects][download][url]                          = "http://download.ckeditor.com/fakeobjects/releases/fakeobjects_4.3.2.zip"

libraries[scayt][destination]                                  = "libraries"
libraries[scayt][directory_name]                               = "ckeditor/plugins/scayt"
libraries[scayt][download][type]                               = "get"
libraries[scayt][download][url]                                = "http://download.ckeditor.com/scayt/releases/scayt_4.3.2.zip"

libraries[indentlist][destination]                             = "libraries"
libraries[indentlist][directory_name]                          = "ckeditor/plugins/indentlist"
libraries[indentlist][download][type]                          = "get"
libraries[indentlist][download][url]                           = "http://download.ckeditor.com/indentlist/releases/indentlist_4.3.2.zip"

libraries[list][destination]                                   = "libraries"
libraries[list][directory_name]                                = "ckeditor/plugins/list"
libraries[list][download][type]                                = "get"
libraries[list][download][url]                                 = "http://download.ckeditor.com/list/releases/list_4.3.2.zip"

libraries[magicline][destination]                              = "libraries"
libraries[magicline][directory_name]                           = "ckeditor/plugins/magicline"
libraries[magicline][download][type]                           = "get"
libraries[magicline][download][url]                            = "http://download.ckeditor.com/magicline/releases/magicline_4.3.2.zip"

libraries[lineutils][destination]                              = "libraries"
libraries[lineutils][directory_name]                           = "ckeditor/plugins/lineutils"
libraries[lineutils][download][type]                           = "get"
libraries[lineutils][download][url]                            = "http://download.ckeditor.com/lineutils/releases/lineutils_4.3.2.zip"

libraries[toolbarswitch][destination]                          = "libraries"
libraries[toolbarswitch][directory_name]                       = "ckeditor/plugins/toolbarswitch"
libraries[toolbarswitch][download][type]                       = "get"
libraries[toolbarswitch][download][url]                        = "http://download.ckeditor.com/toolbarswitch/releases/toolbarswitch_4.3.2.zip"

libraries[pastetext][destination]                              = "libraries"
libraries[pastetext][directory_name]                           = "ckeditor/plugins/pastetext"
libraries[pastetext][download][type]                           = "get"
libraries[pastetext][download][url]                            = "http://download.ckeditor.com/pastetext/releases/pastetext_4.3.2.zip"

libraries[pastefromword][destination]                          = "libraries"
libraries[pastefromword][directory_name]                       = "ckeditor/plugins/pastefromword"
libraries[pastefromword][download][type]                       = "get"
libraries[pastefromword][download][url]                        = "http://download.ckeditor.com/pastefromword/releases/pastefromword_4.3.2.zip"

libraries[removeformat][destination]                           = "libraries"
libraries[removeformat][directory_name]                        = "ckeditor/plugins/removeformat"
libraries[removeformat][download][type]                        = "get"
libraries[removeformat][download][url]                         = "http://download.ckeditor.com/removeformat/releases/removeformat_4.3.2.zip"

libraries[sourcearea][destination]                             = "libraries"
libraries[sourcearea][directory_name]                          = "ckeditor/plugins/sourcearea"
libraries[sourcearea][download][type]                          = "get"
libraries[sourcearea][download][url]                           = "http://download.ckeditor.com/sourcearea/releases/sourcearea_4.3.2.zip"

libraries[specialchar][destination]                            = "libraries"
libraries[specialchar][directory_name]                         = "ckeditor/plugins/specialchar"
libraries[specialchar][download][type]                         = "get"
libraries[specialchar][download][url]                          = "http://download.ckeditor.com/specialchar/releases/specialchar_4.3.2.zip"

libraries[stylescombo][destination]                            = "libraries"
libraries[stylescombo][directory_name]                         = "ckeditor/plugins/stylescombo"
libraries[stylescombo][download][type]                         = "get"
libraries[stylescombo][download][url]                          = "http://download.ckeditor.com/stylescombo/releases/stylescombo_4.3.2.zip"

libraries[richcombo][destination]                              = "libraries"
libraries[richcombo][directory_name]                           = "ckeditor/plugins/richcombo"
libraries[richcombo][download][type]                           = "get"
libraries[richcombo][download][url]                            = "http://download.ckeditor.com/richcombo/releases/richcombo_4.3.2.zip"

libraries[button][destination]                                 = "libraries"
libraries[button][directory_name]                              = "ckeditor/plugins/button"
libraries[button][download][type]                              = "get"
libraries[button][download][url]                               = "http://download.ckeditor.com/button/releases/button_4.3.2.zip"

libraries[tab][destination]                                    = "libraries"
libraries[tab][directory_name]                                 = "ckeditor/plugins/tab"
libraries[tab][download][type]                                 = "get"
libraries[tab][download][url]                                  = "http://download.ckeditor.com/tab/releases/tab_4.3.2.zip"

libraries[table][destination]                                  = "libraries"
libraries[table][directory_name]                               = "ckeditor/plugins/table"
libraries[table][download][type]                               = "table"
libraries[table][download][url]                                = "http://download.ckeditor.com/table/releases/table_4.3.2.zip"

libraries[tabletools][destination]                             = "libraries"
libraries[tabletools][directory_name]                          = "ckeditor/plugins/tabletools"
libraries[tabletools][download][type]                          = "table"
libraries[tabletools][download][url]                           = "http://download.ckeditor.com/tabletools/releases/tabletools_4.3.2.zip"

libraries[undo][destination]                                   = "libraries"
libraries[undo][directory_name]                                = "ckeditor/plugins/undo"
libraries[undo][download][type]                                = "table"
libraries[undo][download][url]                                 = "http://download.ckeditor.com/undo/releases/undo_4.3.2.zip"

libraries[justify][destination]                                = "libraries"
libraries[justify][directory_name]                             = "ckeditor/plugins/justify"
libraries[justify][download][type]                             = "table"
libraries[justify][download][url]                              = "http://download.ckeditor.com/justify/releases/justify_4.3.2.zip"

libraries[showblocks][destination]                             = "libraries"
libraries[showblocks][directory_name]                          = "ckeditor/plugins/showblocks"
libraries[showblocks][download][type]                          = "table"
libraries[showblocks][download][url]                           = "http://download.ckeditor.com/showblocks/releases/showblocks_4.3.2.zip"

libraries[showborders][destination]                            = "libraries"
libraries[showborders][directory_name]                         = "ckeditor/plugins/showborders"
libraries[showborders][download][type]                         = "table"
libraries[showborders][download][url]                          = "http://download.ckeditor.com/showborders/releases/showborders_4.3.2.zip"

libraries[showborders][destination]                            = "libraries"
libraries[showborders][directory_name]                         = "ckeditor/plugins/showborders"
libraries[showborders][download][type]                         = "table"
libraries[showborders][download][url]                          = "http://download.ckeditor.com/showborders/releases/showborders_4.3.2.zip"

libraries[tableresize][destination]                            = "libraries"
libraries[tableresize][directory_name]                         = "ckeditor/plugins/tableresize"
libraries[tableresize][download][type]                         = "get"
libraries[tableresize][download][url]                          = "http://download.ckeditor.com/tableresize/releases/tableresize_4.2.3.zip"

libraries[sharedspace][destination]                            = "libraries"
libraries[sharedspace][directory_name]                         = "ckeditor/plugins/sharedspace"
libraries[sharedspace][download][type]                         = "get"
libraries[sharedspace][download][url]                          = "http://download.ckeditor.com/sharedspace/releases/sharedspace_4.3.2.zip"

libraries[sourcedialog][destination]                           = "libraries"
libraries[sourcedialog][directory_name]                        = "ckeditor/plugins/sourcedialog"
libraries[sourcedialog][download][type]                        = "get"
libraries[sourcedialog][download][url]                         = "http://download.ckeditor.com/sourcedialog/releases/sourcedialog_4.3.2.zip"

libraries[widget][destination]                                 = "libraries"
libraries[widget][directory_name]                              = "ckeditor/plugins/widget"
libraries[widget][download][type]                              = "get"
libraries[widget][download][url]                               = "http://download.ckeditor.com/widget/releases/widget_4.3.2.zip"

libraries[insertpre][destination]                              = "libraries"
libraries[insertpre][directory_name]                           = "ckeditor/plugins/insertpre"
libraries[insertpre][download][type]                           = "get"
libraries[insertpre][download][url]                            = "http://download.ckeditor.com/insertpre/releases/insertpre_1.1.zip"

libraries[apigee_ckeditor_skin][destination]                   = "libraries"
libraries[apigee_ckeditor_skin][directory_name]                = "ckeditor/skins/apigee"
libraries[apigee_ckeditor_skin][download][type]                = "git"
libraries[apigee_ckeditor_skin][download][url]                 = "git@github.com:apigee/drupal_ckeditor_skin.git"


; Libraries
; ------------------------------------------------------------------
; unfortunately, we can't link directly to a /download/latest
; for some and the URL's will need to be updated
; by hand when security updates are posted

libraries[backbone][destination]                               = "libraries"
libraries[backbone][download][type]                            = "git"
libraries[backbone][download][url]                             = "git://github.com/jashkenas/backbone.git"

libraries[colorpicker][destination]                            = "libraries"
libraries[colorpicker][directory_name]                         = "colorpicker"
libraries[colorpicker][download][type]                         = 'get'
libraries[colorpicker][download][url]                          = 'http://www.eyecon.ro/colorpicker/colorpicker.zip'

libraries[highcharts][destination]                             = "libraries"
libraries[highcharts][directory_name]                          = "highcharts"
libraries[highcharts][download][type]                          = "get"
libraries[highcharts][download][url]                           = "http://code.highcharts.com/zips/Highcharts-3.0.7.zip"

libraries[jquery_cycle][destination]                           = "libraries"
libraries[jquery_cycle][directory_name]                        = "jquery.cycle"
libraries[jquery_cycle][download][type]                        = "get"
libraries[jquery_cycle][download][url]                         = "http://www.malsup.com/jquery/cycle/release/jquery.cycle.zip?v2.86"

libraries[jquery_selectlist][destination]                      = "libraries"
libraries[jquery_selectlist][directory_name]                   = "jquery.selectlist"
libraries[jquery_selectlist][download][type]                   = "get"
libraries[jquery_selectlist][download][url]                    = "http://odyniec.net/projects/selectlist/jquery.selectlist-0.6.1.zip"

libraries[json2][destination]                                  = "libraries"
libraries[json2][directory_name]                               = "json2"
libraries[json2][download][type]                               = "git"
libraries[json2][download][url]                                = "git://github.com/douglascrockford/JSON-js.git"

libraries[jsonpath][download][type]                            = "get"
libraries[jsonpath][download][url]                             = "https://jsonpath.googlecode.com/files/jsonpath-0.8.1.php"
libraries[jsonpath][destination]                               = "libraries"

libraries[maskmoney][download][type]                           = "get"
libraries[maskmoney][download][url]                            = "https://raw.github.com/plentz/jquery-maskmoney/master/src/jquery.maskMoney.js"
libraries[maskmoney][destination]                              = "libraries"

libraries[mediaelement][destination]                           = 'libraries'
libraries[mediaelement][directory_name]                        = 'mediaelement'
libraries[mediaelement][download][type]                        = "git"
libraries[mediaelement][download][url]                         = 'git://github.com/johndyer/mediaelement.git'

libraries[mgmt-api-php-sdk][destination]                       = 'libraries'
libraries[mgmt-api-php-sdk][directory_name]                    = 'mgmt-api-php-sdk'
libraries[mgmt-api-php-sdk][download][type]                    = 'git'
libraries[mgmt-api-php-sdk][download][url]                     = 'git@github.com:apigee/edge-php-sdk.git'

libraries[plupload][download][type]                            = "get"
libraries[plupload][download][url]                             = "https://github.com/moxiecode/plupload/archive/v2.0.0.zip"
libraries[plupload][destination]                               = "libraries"

libraries[respondjs][download][type]                           = "get"
libraries[respondjs][download][url]                            = "https://github.com/scottjehl/Respond/tarball/master"
libraries[respondjs][destination]                              = "libraries"

libraries[SolrPhpClient][destination]                          = "libraries"
libraries[SolrPhpClient][directory_name]                       = "SolrPhpClient"
libraries[SolrPhpClient][download][type]                       = "get"
libraries[SolrPhpClient][download][url]                        = "http://solr-php-client.googlecode.com/files/SolrPhpClient.r60.2011-05-04.zip"

libraries[syntaxhighlighter][destination]                      = 'libraries'
libraries[syntaxhighlighter][directory_name]                   = "syntaxhighlighter"
libraries[syntaxhighlighter][download][type]                   = 'get'
libraries[syntaxhighlighter][download][url]                    = 'http://alexgorbatchev.com/SyntaxHighlighter/download/download.php?sh_current'

libraries[timeago][download][type]                             = "get"
libraries[timeago][download][url]                              = "https://raw.github.com/rmm5t/jquery-timeago/v1.3.1/jquery.timeago.js"
libraries[timeago][destination]                                = "libraries"

libraries[underscore][download][type]                          = "get"
libraries[underscore][download][url]                           = "http://documentcloud.github.io/underscore/underscore-min.js"
libraries[underscore][destination]                             = "libraries"

