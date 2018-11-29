;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;     DO NOT DIRECTLY EDIT THIS FILE.    ;;;
;;;      Edit apigee.make.json instead     ;;;
;;; and then run scripts/make_makefile.php ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

core = 7.x
api = 2

; --- MODULES ---

; Soft dependency of content_access
projects[acl][type] = "module"
projects[acl][subdir] = "contrib"
projects[acl][version] = "1.1"

; Required by Monetization
projects[addressfield][type] = "module"
projects[addressfield][subdir] = "contrib"
projects[addressfield][version] = "1.2"

; Enabled during profile install.
projects[admin_menu][type] = "module"
projects[admin_menu][subdir] = "contrib"
projects[admin_menu][version] = "3.0-rc5"

; Enabled during profile install.
projects[admin_views][type] = "module"
projects[admin_views][subdir] = "contrib"
projects[admin_views][version] = "1.6"

; Enabled during profile install.
projects[adminimal_admin_menu][type] = "module"
projects[adminimal_admin_menu][subdir] = "contrib"
projects[adminimal_admin_menu][version] = "1.7"

; Enabled during profile install.
projects[advanced_forum][type] = "module"
projects[advanced_forum][subdir] = "contrib"
projects[advanced_forum][version] = "2.6"

projects[apachesolr][type] = "module"
projects[apachesolr][subdir] = "contrib"
projects[apachesolr][version] = "1.9"

projects[autologout][type] = "module"
projects[autologout][subdir] = "contrib"
projects[autologout][version] = "4.5"

; Enabled during profile install; required by devconnect_blog
projects[block_class][type] = "module"
projects[block_class][subdir] = "contrib"
projects[block_class][version] = "2.3"

; Enabled during profile install.
projects[bootstrap_modal_forms][type] = "module"
projects[bootstrap_modal_forms][subdir] = "contrib"
projects[bootstrap_modal_forms][download][branch] = "7.x-1.x"
projects[bootstrap_modal_forms][download][type] = "git"
projects[bootstrap_modal_forms][download][url] = "http://git.drupal.org/sandbox/bhasselbeck/2167991.git"
projects[bootstrap_modal_forms][directory_name] = "bootstrap_modal_forms"

; Deprecated
projects[bugherd][type] = "module"
projects[bugherd][subdir] = "contrib"
projects[bugherd][version] = "1.0-beta4"

projects[captcha][type] = "module"
projects[captcha][subdir] = "contrib"
projects[captcha][version] = "1.5"

; Deprecated
projects[cck_phone][type] = "module"
projects[cck_phone][subdir] = "contrib"
projects[cck_phone][version] = "1.x-dev"

; Enabled during profile install.
projects[ckeditor][type] = "module"
projects[ckeditor][subdir] = "contrib"
projects[ckeditor][version] = "1.18"

projects[ckeditor_link][type] = "module"
projects[ckeditor_link][subdir] = "contrib"
projects[ckeditor_link][version] = "2.4"

projects[commerce][type] = "module"
projects[commerce][subdir] = "contrib"
projects[commerce][version] = "1.14"

projects[commerce_custom_line_items][type] = "module"
projects[commerce_custom_line_items][subdir] = "contrib"
projects[commerce_custom_line_items][version] = "1.x-dev"

; Deprecated
projects[commerce_worldpay][type] = "module"
projects[commerce_worldpay][subdir] = "contrib"
projects[commerce_worldpay][version] = "1.0-alpha2"

projects[connector][type] = "module"
projects[connector][subdir] = "contrib"
projects[connector][version] = "1.0-beta2"

; Deprecated
projects[contentapi][type] = "module"
projects[contentapi][subdir] = "contrib"
projects[contentapi][version] = "1.0-alpha3"

projects[content_access][type] = "module"
projects[content_access][subdir] = "contrib"
projects[content_access][version] = "1.2-beta2"

; Enabled during profile install.
projects[context][type] = "module"
projects[context][subdir] = "contrib"
projects[context][version] = "3.7"

; Enabled during profile install.
projects[context_condition_theme][type] = "module"
projects[context_condition_theme][subdir] = "contrib"
projects[context_condition_theme][version] = "1.0"

; Enabled during profile install.
projects[ctools][type] = "module"
projects[ctools][subdir] = "contrib"
projects[ctools][version] = "1.13"

; Deprecated
projects[date][type] = "module"
projects[date][subdir] = "contrib"
projects[date][version] = "2.10"

projects[devel][type] = "module"
projects[devel][subdir] = "contrib"
projects[devel][version] = "1.5"

projects[diff][type] = "module"
projects[diff][subdir] = "contrib"
projects[diff][version] = "3.3"

projects[ds][type] = "module"
projects[ds][subdir] = "contrib"
projects[ds][version] = "2.15"

; Deprecated.
projects[eck][type] = "module"
projects[eck][subdir] = "contrib"
projects[eck][version] = "2.0-rc9"

projects[entityreference][type] = "module"
projects[entityreference][subdir] = "contrib"
projects[entityreference][version] = "1.5"
; see https://www.drupal.org/node/2170193
projects[entityreference][patch][2170193] = "https://www.drupal.org/files/issues/entityreference-2170193-3-plugin-paths.patch"

; Enabled during profile install.
projects[entity][type] = "module"
projects[entity][subdir] = "contrib"
projects[entity][version] = "1.9"

; Disabled on OPDK builds
projects[environment_indicator][type] = "module"
projects[environment_indicator][subdir] = "contrib"
projects[environment_indicator][version] = "2.9"

; Enabled during profile install.
projects[faq][type] = "module"
projects[faq][subdir] = "contrib"
projects[faq][version] = "1.1"
; see https://www.drupal.org/node/2646470
projects[faq][patch][2646470] = "https://www.drupal.org/files/issues/faq-2646470-php-warning.patch"

; Enabled during profile install.
projects[features][type] = "module"
projects[features][subdir] = "contrib"
projects[features][version] = "2.10"

; Deprecated
projects[features_extra][type] = "module"
projects[features_extra][subdir] = "contrib"
projects[features_extra][version] = "1.0"

; Enabled during profile install.
projects[field_group][type] = "module"
projects[field_group][subdir] = "contrib"
projects[field_group][version] = "1.6"

; Enabled during profile install.
projects[file_entity][type] = "module"
projects[file_entity][subdir] = "contrib"
projects[file_entity][version] = "2.16"

; Enabled during profile install.
projects[flood_control][type] = "module"
projects[flood_control][subdir] = "contrib"
projects[flood_control][version] = "1.0"

; Deprecated
projects[ftools][type] = "module"
projects[ftools][subdir] = "contrib"
projects[ftools][version] = "1.6"

; Deprecated
projects[gauth][type] = "module"
projects[gauth][subdir] = "contrib"
projects[gauth][version] = "1.9"

; Deprecated
projects[github_connect][type] = "module"
projects[github_connect][subdir] = "contrib"
projects[github_connect][version] = "1.1"
; see https://www.drupal.org/node/2150767
projects[github_connect][patch][2150767] = "https://www.drupal.org/files/issues/administer-github-connect-2150767-2.patch"
; see https://www.drupal.org/node/1895544
projects[github_connect][patch][1895544] = "https://www.drupal.org/files/issues/1895544-github-connect-return-user-5.patch"
; see https://www.drupal.org/node/2266675
projects[github_connect][patch][2266675] = "https://www.drupal.org/files/issues/github_connect-email-api-change-2266675-1.patch"
; see https://www.drupal.org/node/2292767
projects[github_connect][patch][2292767] = "https://www.drupal.org/files/issues/github_connect-2292767-openid-dependency.patch"

projects[google_analytics][type] = "module"
projects[google_analytics][subdir] = "contrib"
projects[google_analytics][version] = "2.4"

projects[gravatar][type] = "module"
projects[gravatar][subdir] = "contrib"
projects[gravatar][version] = "1.1"

; Deprecated
projects[highcharts][type] = "module"
projects[highcharts][subdir] = "contrib"
projects[highcharts][version] = "1.0-alpha6"
; see https://www.drupal.org/node/2831850
projects[highcharts][patch][2831850] = "https://www.drupal.org/files/issues/highcharts-2831850-1-use-cdn_0.patch"

projects[http_client][type] = "module"
projects[http_client][subdir] = "contrib"
projects[http_client][version] = "2.4"

; Deprecated
projects[i18n][type] = "module"
projects[i18n][subdir] = "contrib"
projects[i18n][version] = "1.22"

projects[jquery_update][type] = "module"
projects[jquery_update][subdir] = "contrib"
projects[jquery_update][version] = "3.0-alpha5"

; Required for OPDK
projects[ldap][type] = "module"
projects[ldap][subdir] = "contrib"
projects[ldap][version] = "2.3"

projects[legal][type] = "module"
projects[legal][subdir] = "contrib"
projects[legal][version] = "1.10"

; Enabled during profile install.
projects[libraries][type] = "module"
projects[libraries][subdir] = "contrib"
projects[libraries][version] = "2.3"

projects[linkchecker][type] = "module"
projects[linkchecker][subdir] = "contrib"
projects[linkchecker][version] = "1.3"

; Enabled during profile install.
projects[link][type] = "module"
projects[link][subdir] = "contrib"
projects[link][version] = "1.4"

; Enabled during profile install.
projects[logintoboggan][type] = "module"
projects[logintoboggan][subdir] = "contrib"
projects[logintoboggan][version] = "1.5"

projects[mailsystem][type] = "module"
projects[mailsystem][subdir] = "contrib"
projects[mailsystem][version] = "2.34"

projects[markdown][type] = "module"
projects[markdown][subdir] = "contrib"
projects[markdown][version] = "1.5"

projects[mediaelement][type] = "module"
projects[mediaelement][subdir] = "contrib"
projects[mediaelement][version] = "1.2"

; Enabled during profile install.
projects[media][type] = "module"
projects[media][subdir] = "contrib"
projects[media][version] = "2.19"

; Enabled during profile install.
projects[media_ckeditor][type] = "module"
projects[media_ckeditor][subdir] = "contrib"
projects[media_ckeditor][version] = "2.5"

; Enabled during profile install.
projects[media_youtube][type] = "module"
projects[media_youtube][subdir] = "contrib"
projects[media_youtube][version] = "3.7"

; Enabled during profile install.
projects[menu_attributes][type] = "module"
projects[menu_attributes][subdir] = "contrib"
projects[menu_attributes][version] = "1.0"

projects[metatag][type] = "module"
projects[metatag][subdir] = "contrib"
projects[metatag][version] = "1.22"

; Enabled during profile install.
projects[me][type] = "module"
projects[me][subdir] = "contrib"
projects[me][version] = "1.4"

; Deprecated
projects[migrate][type] = "module"
projects[migrate][subdir] = "contrib"
projects[migrate][version] = "2.9"

projects[mimemail][type] = "module"
projects[mimemail][subdir] = "contrib"
projects[mimemail][version] = "1.1"

; Enabled on install.
projects[module_filter][type] = "module"
projects[module_filter][subdir] = "contrib"
projects[module_filter][version] = "2.1"

projects[nagios][type] = "module"
projects[nagios][subdir] = "contrib"
projects[nagios][version] = "1.3"

; Deprecated
projects[node_export][type] = "module"
projects[node_export][subdir] = "contrib"
projects[node_export][version] = "3.1"

projects[oauth][type] = "module"
projects[oauth][subdir] = "contrib"
projects[oauth][version] = "3.4"

projects[oauthconnector][type] = "module"
projects[oauthconnector][subdir] = "contrib"
projects[oauthconnector][version] = "1.0-beta2"

; Enabled on install.
projects[pathauto][type] = "module"
projects[pathauto][subdir] = "contrib"
projects[pathauto][version] = "1.3"

projects[readonlymode][type] = "module"
projects[readonlymode][subdir] = "contrib"
projects[readonlymode][version] = "1.2"

projects[recaptcha][type] = "module"
projects[recaptcha][subdir] = "contrib"
projects[recaptcha][version] = "2.2"

; Enabled on install.
projects[redirect][type] = "module"
projects[redirect][subdir] = "contrib"
projects[redirect][version] = "1.0-rc3"

projects[redis][type] = "module"
projects[redis][subdir] = "contrib"
projects[redis][version] = "3.17"

; Enabled on install.
projects[remote_stream_wrapper][type] = "module"
projects[remote_stream_wrapper][subdir] = "contrib"
projects[remote_stream_wrapper][version] = "1.0-rc1"

projects[rules][type] = "module"
projects[rules][subdir] = "contrib"
projects[rules][version] = "2.10"

projects[services][type] = "module"
projects[services][subdir] = "contrib"
projects[services][version] = "3.20"

projects[services_views][type] = "module"
projects[services_views][subdir] = "contrib"
projects[services_views][version] = "1.3"

; Required for OPDK
projects[siteminder][type] = "module"
projects[siteminder][subdir] = "contrib"
projects[siteminder][version] = "2.x-dev"

projects[smtp][type] = "module"
projects[smtp][subdir] = "contrib"
projects[smtp][version] = "1.7"

; Required by Monetization
projects[special_menu_items][type] = "module"
projects[special_menu_items][subdir] = "contrib"
projects[special_menu_items][version] = "2.0"

projects[strongarm][type] = "module"
projects[strongarm][subdir] = "contrib"
projects[strongarm][version] = "2.0"

; Deprecated
projects[sumo][type] = "module"
projects[sumo][subdir] = "contrib"
projects[sumo][download][type] = "git"
projects[sumo][download][url] = "http://git.drupal.org/sandbox/daniel_j/2390985.git"
projects[sumo][directory_name] = "sumo"

projects[syntaxhighlighter][type] = "module"
projects[syntaxhighlighter][subdir] = "contrib"
projects[syntaxhighlighter][version] = "2.0"

projects[taxonomy_access][type] = "module"
projects[taxonomy_access][subdir] = "contrib"
projects[taxonomy_access][version] = "1.0"

; Enabled on install.
projects[token][type] = "module"
projects[token][subdir] = "contrib"
projects[token][version] = "1.7"

; Deprecated
projects[util][type] = "module"
projects[util][subdir] = "contrib"
projects[util][version] = "1.1"

; Enabled on install.
projects[uuid][type] = "module"
projects[uuid][subdir] = "contrib"
projects[uuid][version] = "1.1"

; Deprecated
projects[uuid_features][type] = "module"
projects[uuid_features][subdir] = "contrib"
projects[uuid_features][version] = "1.0-rc1"

; Deprecated
projects[variable][type] = "module"
projects[variable][subdir] = "contrib"
projects[variable][version] = "2.5"

; Enabled on install.
projects[views][type] = "module"
projects[views][subdir] = "contrib"
projects[views][version] = "3.18"

; Deprecated
projects[views_accordion][type] = "module"
projects[views_accordion][subdir] = "contrib"
projects[views_accordion][version] = "1.1"

projects[views_bulk_operations][type] = "module"
projects[views_bulk_operations][subdir] = "contrib"
projects[views_bulk_operations][version] = "3.4"

; Deprecated
projects[views_queue][type] = "module"
projects[views_queue][subdir] = "contrib"
projects[views_queue][version] = "1.x-dev"

projects[views_slideshow][type] = "module"
projects[views_slideshow][subdir] = "contrib"
projects[views_slideshow][version] = "3.9"

; NOT enabled on install.
projects[webform][type] = "module"
projects[webform][subdir] = "contrib"
projects[webform][version] = "4.16"

; Enabled during profile install. Prevents XSS in full HTML.
projects[wysiwyg_filter][type] = "module"
projects[wysiwyg_filter][subdir] = "contrib"
projects[wysiwyg_filter][version] = "1.6-rc9"

projects[xautoload][type] = "module"
projects[xautoload][subdir] = "contrib"
projects[xautoload][version] = "5.7"



; --- THEMES ---

; Enabled during profile install for admin screens.
projects[adminimal_theme][type] = "theme"
projects[adminimal_theme][version] = "1.24"

; Not enabled, but used by Apigee Responsive theme.
projects[bootstrap][type] = "theme"
projects[bootstrap][version] = "3.22"

; Deprecated in favor of Adminimal.
projects[rubik][type] = "theme"
projects[rubik][version] = "4.4"

projects[tao][type] = "theme"
projects[tao][version] = "3.1"



; --- LIBRARIES ---

libraries[ckeditor][destination] = "libraries"
libraries[ckeditor][directory_name] = "ckeditor"
libraries[ckeditor][download][type] = "file"
libraries[ckeditor][download][url] = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%204.3.2/ckeditor_4.3.2_full.zip"

libraries[about][destination] = "libraries"
libraries[about][directory_name] = "ckeditor/plugins/about"
libraries[about][download][type] = "file"
libraries[about][download][url] = "http://download.ckeditor.com/about/releases/about_4.3.2.zip"

libraries[a11yhelp][destination] = "libraries"
libraries[a11yhelp][directory_name] = "ckeditor/plugins/a11yhelp"
libraries[a11yhelp][download][type] = "file"
libraries[a11yhelp][download][url] = "http://download.ckeditor.com/a11yhelp/releases/a11yhelp_4.3.2.zip"

libraries[basicstyles][destination] = "libraries"
libraries[basicstyles][directory_name] = "ckeditor/plugins/basicstyles"
libraries[basicstyles][download][type] = "file"
libraries[basicstyles][download][url] = "http://download.ckeditor.com/basicstyles/releases/basicstyles_4.3.2.zip"

libraries[blockquote][destination] = "libraries"
libraries[blockquote][directory_name] = "ckeditor/plugins/blockquote"
libraries[blockquote][download][type] = "file"
libraries[blockquote][download][url] = "http://download.ckeditor.com/blockquote/releases/blockquote_4.3.2.zip"

libraries[button][destination] = "libraries"
libraries[button][directory_name] = "ckeditor/plugins/button"
libraries[button][download][type] = "file"
libraries[button][download][url] = "http://download.ckeditor.com/button/releases/button_4.3.2.zip"

libraries[clipboard][destination] = "libraries"
libraries[clipboard][directory_name] = "ckeditor/plugins/clipboard"
libraries[clipboard][download][type] = "file"
libraries[clipboard][download][url] = "http://download.ckeditor.com/clipboard/releases/clipboard_4.3.2.zip"

libraries[contextmenu][destination] = "libraries"
libraries[contextmenu][directory_name] = "ckeditor/plugins/contextmenu"
libraries[contextmenu][download][type] = "file"
libraries[contextmenu][download][url] = "http://download.ckeditor.com/contextmenu/releases/contextmenu_4.3.2.zip"

libraries[dialog][destination] = "libraries"
libraries[dialog][directory_name] = "ckeditor/plugins/dialog"
libraries[dialog][download][type] = "file"
libraries[dialog][download][url] = "http://download.ckeditor.com/dialog/releases/dialog_4.3.2.zip"

libraries[dialogui][destination] = "libraries"
libraries[dialogui][directory_name] = "ckeditor/plugins/dialogui"
libraries[dialogui][download][type] = "file"
libraries[dialogui][download][url] = "http://download.ckeditor.com/dialogui/releases/dialogui_4.3.2.zip"

libraries[elementspath][destination] = "libraries"
libraries[elementspath][directory_name] = "ckeditor/plugins/elementspath"
libraries[elementspath][download][type] = "file"
libraries[elementspath][download][url] = "http://download.ckeditor.com/elementspath/releases/elementspath_4.3.2.zip"

libraries[enterkey][destination] = "libraries"
libraries[enterkey][directory_name] = "ckeditor/plugins/enterkey"
libraries[enterkey][download][type] = "file"
libraries[enterkey][download][url] = "http://download.ckeditor.com/enterkey/releases/enterkey_4.3.2.zip"

libraries[entities][destination] = "libraries"
libraries[entities][directory_name] = "ckeditor/plugins/entities"
libraries[entities][download][type] = "file"
libraries[entities][download][url] = "http://download.ckeditor.com/entities/releases/entities_4.3.2.zip"

libraries[fakeobjects][destination] = "libraries"
libraries[fakeobjects][directory_name] = "ckeditor/plugins/fakeobjects"
libraries[fakeobjects][download][type] = "file"
libraries[fakeobjects][download][url] = "http://download.ckeditor.com/fakeobjects/releases/fakeobjects_4.3.2.zip"

libraries[filebrowser][destination] = "libraries"
libraries[filebrowser][directory_name] = "ckeditor/plugins/filebrowser"
libraries[filebrowser][download][type] = "file"
libraries[filebrowser][download][url] = "http://download.ckeditor.com/filebrowser/releases/filebrowser_4.3.2.zip"

libraries[floatingspace][destination] = "libraries"
libraries[floatingspace][directory_name] = "ckeditor/plugins/floatingspace"
libraries[floatingspace][download][type] = "file"
libraries[floatingspace][download][url] = "http://download.ckeditor.com/floatingspace/releases/floatingspace_4.3.2.zip"

libraries[floatpanel][destination] = "libraries"
libraries[floatpanel][directory_name] = "ckeditor/plugins/floatpanel"
libraries[floatpanel][download][type] = "file"
libraries[floatpanel][download][url] = "http://download.ckeditor.com/floatpanel/releases/floatpanel_4.3.2.zip"

libraries[horizontalrule][destination] = "libraries"
libraries[horizontalrule][directory_name] = "ckeditor/plugins/horizontalrule"
libraries[horizontalrule][download][type] = "file"
libraries[horizontalrule][download][url] = "http://download.ckeditor.com/horizontalrule/releases/horizontalrule_4.3.2.zip"

libraries[htmlwriter][destination] = "libraries"
libraries[htmlwriter][directory_name] = "ckeditor/plugins/htmlwriter"
libraries[htmlwriter][download][type] = "file"
libraries[htmlwriter][download][url] = "http://download.ckeditor.com/htmlwriter/releases/htmlwriter_4.3.2.zip"

libraries[iframe][destination] = "libraries"
libraries[iframe][directory_name] = "ckeditor/plugins/iframe"
libraries[iframe][download][type] = "file"
libraries[iframe][download][url] = "http://download.ckeditor.com/iframe/releases/iframe_4.3.2.zip"

libraries[image][destination] = "libraries"
libraries[image][directory_name] = "ckeditor/plugins/image"
libraries[image][download][type] = "file"
libraries[image][download][url] = "http://download.ckeditor.com/image/releases/image_4.3.2.zip"

libraries[indent][destination] = "libraries"
libraries[indent][directory_name] = "ckeditor/plugins/indent"
libraries[indent][download][type] = "file"
libraries[indent][download][url] = "http://download.ckeditor.com/indent/releases/indent_4.3.2.zip"

libraries[indentlist][destination] = "libraries"
libraries[indentlist][directory_name] = "ckeditor/plugins/indentlist"
libraries[indentlist][download][type] = "file"
libraries[indentlist][download][url] = "http://download.ckeditor.com/indentlist/releases/indentlist_4.3.2.zip"

libraries[justify][destination] = "libraries"
libraries[justify][directory_name] = "ckeditor/plugins/justify"
libraries[justify][download][type] = "file"
libraries[justify][download][url] = "http://download.ckeditor.com/justify/releases/justify_4.3.2.zip"

libraries[list][destination] = "libraries"
libraries[list][directory_name] = "ckeditor/plugins/list"
libraries[list][download][type] = "file"
libraries[list][download][url] = "http://download.ckeditor.com/list/releases/list_4.3.2.zip"

libraries[lineutils][destination] = "libraries"
libraries[lineutils][directory_name] = "ckeditor/plugins/lineutils"
libraries[lineutils][download][type] = "file"
libraries[lineutils][download][url] = "http://download.ckeditor.com/lineutils/releases/lineutils_4.3.2.zip"

libraries[magicline][destination] = "libraries"
libraries[magicline][directory_name] = "ckeditor/plugins/magicline"
libraries[magicline][download][type] = "file"
libraries[magicline][download][url] = "http://download.ckeditor.com/magicline/releases/magicline_4.3.2.zip"

libraries[menu][destination] = "libraries"
libraries[menu][directory_name] = "ckeditor/plugins/menu"
libraries[menu][download][type] = "file"
libraries[menu][download][url] = "http://download.ckeditor.com/menu/releases/menu_4.3.2.zip"

libraries[menubutton][destination] = "libraries"
libraries[menubutton][directory_name] = "ckeditor/plugins/menubutton"
libraries[menubutton][download][type] = "file"
libraries[menubutton][download][url] = "http://download.ckeditor.com/menubutton/releases/menubutton_4.3.2.zip"

libraries[panel][destination] = "libraries"
libraries[panel][directory_name] = "ckeditor/plugins/panel"
libraries[panel][download][type] = "file"
libraries[panel][download][url] = "http://download.ckeditor.com/panel/releases/panel_4.3.2.zip"

libraries[pastefromword][destination] = "libraries"
libraries[pastefromword][directory_name] = "ckeditor/plugins/pastefromword"
libraries[pastefromword][download][type] = "file"
libraries[pastefromword][download][url] = "http://download.ckeditor.com/pastefromword/releases/pastefromword_4.3.2.zip"

libraries[pastetext][destination] = "libraries"
libraries[pastetext][directory_name] = "ckeditor/plugins/pastetext"
libraries[pastetext][download][type] = "file"
libraries[pastetext][download][url] = "http://download.ckeditor.com/pastetext/releases/pastetext_4.3.2.zip"

libraries[popup][destination] = "libraries"
libraries[popup][directory_name] = "ckeditor/plugins/popup"
libraries[popup][download][type] = "file"
libraries[popup][download][url] = "http://download.ckeditor.com/popup/releases/popup_4.3.2.zip"

libraries[removeformat][destination] = "libraries"
libraries[removeformat][directory_name] = "ckeditor/plugins/removeformat"
libraries[removeformat][download][type] = "file"
libraries[removeformat][download][url] = "http://download.ckeditor.com/removeformat/releases/removeformat_4.3.2.zip"

libraries[richcombo][destination] = "libraries"
libraries[richcombo][directory_name] = "ckeditor/plugins/richcombo"
libraries[richcombo][download][type] = "file"
libraries[richcombo][download][url] = "http://download.ckeditor.com/richcombo/releases/richcombo_4.3.2.zip"

libraries[scayt][destination] = "libraries"
libraries[scayt][directory_name] = "ckeditor/plugins/scayt"
libraries[scayt][download][type] = "file"
libraries[scayt][download][url] = "http://download.ckeditor.com/scayt/releases/scayt_4.3.2.zip"

libraries[sharedspace][destination] = "libraries"
libraries[sharedspace][directory_name] = "ckeditor/plugins/sharedspace"
libraries[sharedspace][download][type] = "file"
libraries[sharedspace][download][url] = "http://download.ckeditor.com/sharedspace/releases/sharedspace_4.3.2.zip"

libraries[showblocks][destination] = "libraries"
libraries[showblocks][directory_name] = "ckeditor/plugins/showblocks"
libraries[showblocks][download][type] = "file"
libraries[showblocks][download][url] = "http://download.ckeditor.com/showblocks/releases/showblocks_4.3.2.zip"

libraries[showborders][destination] = "libraries"
libraries[showborders][directory_name] = "ckeditor/plugins/showborders"
libraries[showborders][download][type] = "file"
libraries[showborders][download][url] = "http://download.ckeditor.com/showborders/releases/showborders_4.3.2.zip"

libraries[sourcearea][destination] = "libraries"
libraries[sourcearea][directory_name] = "ckeditor/plugins/sourcearea"
libraries[sourcearea][download][type] = "file"
libraries[sourcearea][download][url] = "http://download.ckeditor.com/sourcearea/releases/sourcearea_4.3.2.zip"

libraries[sourcedialog][destination] = "libraries"
libraries[sourcedialog][directory_name] = "ckeditor/plugins/sourcedialog"
libraries[sourcedialog][download][type] = "file"
libraries[sourcedialog][download][url] = "http://download.ckeditor.com/sourcedialog/releases/sourcedialog_4.3.2.zip"

libraries[specialchar][destination] = "libraries"
libraries[specialchar][directory_name] = "ckeditor/plugins/specialchar"
libraries[specialchar][download][type] = "file"
libraries[specialchar][download][url] = "http://download.ckeditor.com/specialchar/releases/specialchar_4.3.2.zip"

libraries[stylescombo][destination] = "libraries"
libraries[stylescombo][directory_name] = "ckeditor/plugins/stylescombo"
libraries[stylescombo][download][type] = "file"
libraries[stylescombo][download][url] = "http://download.ckeditor.com/stylescombo/releases/stylescombo_4.3.2.zip"

libraries[tab][destination] = "libraries"
libraries[tab][directory_name] = "ckeditor/plugins/tab"
libraries[tab][download][type] = "file"
libraries[tab][download][url] = "http://download.ckeditor.com/tab/releases/tab_4.3.2.zip"

libraries[table][destination] = "libraries"
libraries[table][directory_name] = "ckeditor/plugins/table"
libraries[table][download][type] = "file"
libraries[table][download][url] = "http://download.ckeditor.com/table/releases/table_4.3.2.zip"

libraries[tableresize][destination] = "libraries"
libraries[tableresize][directory_name] = "ckeditor/plugins/tableresize"
libraries[tableresize][download][type] = "file"
libraries[tableresize][download][url] = "http://download.ckeditor.com/tableresize/releases/tableresize_4.3.2.zip"

libraries[tabletools][destination] = "libraries"
libraries[tabletools][directory_name] = "ckeditor/plugins/tabletools"
libraries[tabletools][download][type] = "file"
libraries[tabletools][download][url] = "http://download.ckeditor.com/tabletools/releases/tabletools_4.3.2.zip"

libraries[toolbarswitch][destination] = "libraries"
libraries[toolbarswitch][directory_name] = "ckeditor/plugins/toolbarswitch"
libraries[toolbarswitch][download][type] = "file"
libraries[toolbarswitch][download][url] = "http://download.ckeditor.com/toolbarswitch/releases/toolbarswitch_4.3.2.zip"

libraries[undo][destination] = "libraries"
libraries[undo][directory_name] = "ckeditor/plugins/undo"
libraries[undo][download][type] = "file"
libraries[undo][download][url] = "http://download.ckeditor.com/undo/releases/undo_4.3.2.zip"

libraries[widget][destination] = "libraries"
libraries[widget][directory_name] = "ckeditor/plugins/widget"
libraries[widget][download][type] = "file"
libraries[widget][download][url] = "http://download.ckeditor.com/widget/releases/widget_4.3.2.zip"

libraries[wysiwygarea][destination] = "libraries"
libraries[wysiwygarea][directory_name] = "ckeditor/plugins/wysiwygarea"
libraries[wysiwygarea][download][type] = "file"
libraries[wysiwygarea][download][url] = "http://download.ckeditor.com/wysiwygarea/releases/wysiwygarea_4.3.2.zip"

libraries[insertpre][destination] = "libraries"
libraries[insertpre][directory_name] = "ckeditor/plugins/insertpre"
libraries[insertpre][download][type] = "file"
libraries[insertpre][download][url] = "http://download.ckeditor.com/insertpre/releases/insertpre_1.1.zip"

; Used by gauth. Cannot update to 2.x because it depends on PHP >= 5.4
libraries[google-api-php-client][destination] = "libraries"
libraries[google-api-php-client][directory_name] = "google-api-php-client"
libraries[google-api-php-client][download][type] = "file"
libraries[google-api-php-client][download][url] = "https://github.com/google/google-api-php-client/archive/1.1.7.tar.gz"

; Used by views_slideshow
libraries[jquery_cycle][destination] = "libraries"
libraries[jquery_cycle][directory_name] = "jquery.cycle"
libraries[jquery_cycle][download][type] = "file"
libraries[jquery_cycle][download][url] = "http://www.malsup.com/jquery/cycle/release/jquery.cycle.zip?v2.86"

; Used by devconnect_developer_apps
libraries[jquery_selectlist][destination] = "libraries"
libraries[jquery_selectlist][directory_name] = "jquery.selectlist"
libraries[jquery_selectlist][download][type] = "file"
libraries[jquery_selectlist][download][url] = "http://odyniec.net/projects/selectlist/jquery.selectlist-0.6.1.zip"

; Used by devconnect_developer_apps
libraries[datetimepicker][destination] = "libraries"
libraries[datetimepicker][directory_name] = "datetimepicker"
libraries[datetimepicker][download][type] = "file"
libraries[datetimepicker][download][url] = "https://github.com/xdan/datetimepicker/archive/2.5.4.tar.gz"

; Used by devconnect_monetization
libraries[maskmoney][destination] = "libraries"
libraries[maskmoney][directory_name] = "maskmoney"
libraries[maskmoney][download][type] = "file"
libraries[maskmoney][download][url] = "https://raw.githubusercontent.com/plentz/jquery-maskmoney/master/src/jquery.maskMoney.js"

; Used by mediaelement
libraries[mediaelement][destination] = "libraries"
libraries[mediaelement][directory_name] = "mediaelement"
libraries[mediaelement][download][type] = "file"
libraries[mediaelement][download][url] = "https://github.com/johndyer/mediaelement/archive/2.23.5.tar.gz"

; Used by devconnect
libraries[mgmt-api-php-sdk][destination] = "libraries"
libraries[mgmt-api-php-sdk][directory_name] = "mgmt-api-php-sdk"
libraries[mgmt-api-php-sdk][download][type] = "file"
libraries[mgmt-api-php-sdk][download][url] = "https://github.com/apigee/edge-php-sdk/archive/1.1.19.tar.gz"

; Used by media, file_entity
libraries[plupload][destination] = "libraries"
libraries[plupload][directory_name] = "plupload"
libraries[plupload][download][type] = "file"
libraries[plupload][download][url] = "https://github.com/moxiecode/plupload/archive/v2.2.1.zip"

; Used by services, devconnect, smartdocs
libraries[spyc][destination] = "libraries"
libraries[spyc][directory_name] = "spyc"
libraries[spyc][download][type] = "file"
libraries[spyc][download][url] = "https://raw.githubusercontent.com/mustangostang/spyc/master/Spyc.php"

; Used by syntaxhighlighter
libraries[syntaxhighlighter][destination] = "libraries"
libraries[syntaxhighlighter][directory_name] = "syntaxhighlighter-2.x"
libraries[syntaxhighlighter][download][type] = "file"
libraries[syntaxhighlighter][download][url] = "https://github.com/syntaxhighlighter/syntaxhighlighter/archive/2.1.364.tar.gz"
