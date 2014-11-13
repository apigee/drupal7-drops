;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; DO NOT DIRECTLY EDIT THIS FILE.        ;;;
;;; Edit apigee.make.json instead          ;;;
;;; and then run scripts/make_makefile.php ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

core = 7.x
api = 2

; --- MODULES ---

; Deprecated
projects[acl][type] = "module"
projects[acl][subdir] = "contrib"

; Required by Monetization
projects[addressfield][type] = "module"
projects[addressfield][subdir] = "contrib"

projects[admin_menu][type] = "module"
projects[admin_menu][subdir] = "contrib"

projects[admin_views][type] = "module"
projects[admin_views][subdir] = "contrib"

projects[advanced_forum][type] = "module"
projects[advanced_forum][subdir] = "contrib"

projects[apachesolr][type] = "module"
projects[apachesolr][subdir] = "contrib"

projects[apachesolr_stats][type] = "module"
projects[apachesolr_stats][subdir] = "contrib"

; Enabled during profile install
projects[autologout][type] = "module"
projects[autologout][subdir] = "contrib"

; Deprecated
projects[bean][type] = "module"
projects[bean][subdir] = "contrib"

projects[block_class][type] = "module"
projects[block_class][subdir] = "contrib"

projects[bootstrap_modal_forms][type] = "module"
projects[bootstrap_modal_forms][subdir] = "contrib"
projects[bootstrap_modal_forms][download][type] = "git"
projects[bootstrap_modal_forms][download][url] = "http://git.drupal.org/sandbox/bhasselbeck/2167991.git"
projects[bootstrap_modal_forms][directory_name] = "bootstrap_modal_forms"

projects[bugherd][type] = "module"
projects[bugherd][subdir] = "contrib"

projects[captcha][type] = "module"
projects[captcha][subdir] = "contrib"

; Deprecated?
projects[cck_phone][type] = "module"
projects[cck_phone][subdir] = "contrib"

projects[chart][type] = "module"
projects[chart][subdir] = "contrib"

projects[ckeditor][type] = "module"
projects[ckeditor][subdir] = "contrib"

projects[ckeditor_bootstrap][type] = "module"
projects[ckeditor_bootstrap][subdir] = "contrib"
projects[ckeditor_bootstrap][version] = "1.0-alpha1"

projects[ckeditor_link][type] = "module"
projects[ckeditor_link][subdir] = "contrib"

projects[commerce][type] = "module"
projects[commerce][subdir] = "contrib"

projects[commerce_custom_line_items][type] = "module"
projects[commerce_custom_line_items][subdir] = "contrib"

projects[commerce_worldpay_business_gateway][type] = "module"
projects[commerce_worldpay_business_gateway][subdir] = "contrib"
projects[commerce_worldpay_business_gateway][download][type] = "git"
projects[commerce_worldpay_business_gateway][download][url] = "http://git.drupal.org/sandbox/magicmyth/1433370.git"
projects[commerce_worldpay_business_gateway][download][branch] = "7.x-1.x"
projects[commerce_worldpay_business_gateway][directory_name] = "commerce_worldpay_business_gateway"
; see http://drupal.org/node/1569386
projects[commerce_worldpay_business_gateway][patch][1569386] = "https://drupal.org/files/DRAFT-Issue-1569386.patch"

projects[connector][type] = "module"
projects[connector][subdir] = "contrib"

; Deprecated
projects[contentapi][type] = "module"
projects[contentapi][subdir] = "contrib"

projects[content_access][type] = "module"
projects[content_access][subdir] = "contrib"

projects[context][type] = "module"
projects[context][subdir] = "contrib"

projects[context_condition_theme][type] = "module"
projects[context_condition_theme][subdir] = "contrib"

projects[ctools][type] = "module"
projects[ctools][subdir] = "contrib"

projects[date][type] = "module"
projects[date][subdir] = "contrib"

projects[devel][type] = "module"
projects[devel][subdir] = "contrib"

projects[diff][type] = "module"
projects[diff][subdir] = "contrib"

projects[ds][type] = "module"
projects[ds][subdir] = "contrib"

; Required by devconnect_download
projects[eck][type] = "module"
projects[eck][subdir] = "contrib"

; Required by OPDK
projects[encrypt][type] = "module"
projects[encrypt][subdir] = "contrib"

projects[entityreference][type] = "module"
projects[entityreference][subdir] = "contrib"
; see http://drupal.org/node/2170193
projects[entityreference][patch][2170193] = "https://drupal.org/files/issues/entityreference-2170193-3-plugin-paths.patch"

projects[entity][type] = "module"
projects[entity][subdir] = "contrib"

; Disabled on OPDK builds
projects[environment_indicator][type] = "module"
projects[environment_indicator][subdir] = "contrib"

projects[faq][type] = "module"
projects[faq][subdir] = "contrib"

projects[features][type] = "module"
projects[features][subdir] = "contrib"

projects[features_extra][type] = "module"
projects[features_extra][subdir] = "contrib"

projects[field_group][type] = "module"
projects[field_group][subdir] = "contrib"

projects[file_entity][type] = "module"
projects[file_entity][subdir] = "contrib"

projects[flood_control][type] = "module"
projects[flood_control][subdir] = "contrib"

projects[ftools][type] = "module"
projects[ftools][subdir] = "contrib"

projects[gauth][type] = "module"
projects[gauth][subdir] = "contrib"

projects[github_connect][type] = "module"
projects[github_connect][subdir] = "contrib"
; see http://drupal.org/node/2150767
projects[github_connect][patch][2150767] = "https://drupal.org/files/issues/administer-github-connect-2150767-2.patch"
; see http://drupal.org/node/1895544
projects[github_connect][patch][1895544] = "https://drupal.org/files/issues/1895544-github-connect-return-user-5.patch"
; see http://drupal.org/node/2266675
projects[github_connect][patch][2266675] = "https://drupal.org/files/issues/github_connect-email-api-change-2266675-1.patch"
; see http://drupal.org/node/2292767
projects[github_connect][patch][2292767] = "https://drupal.org/files/issues/github_connect-2292767-openid-dependency.patch"

projects[google_analytics][type] = "module"
projects[google_analytics][subdir] = "contrib"

projects[gravatar][type] = "module"
projects[gravatar][subdir] = "contrib"

projects[highcharts][type] = "module"
projects[highcharts][subdir] = "contrib"

projects[http_client][type] = "module"
projects[http_client][subdir] = "contrib"

projects[i18n][type] = "module"
projects[i18n][subdir] = "contrib"

projects[jquery_colorpicker][type] = "module"
projects[jquery_colorpicker][subdir] = "contrib"

projects[jquery_update][type] = "module"
projects[jquery_update][subdir] = "contrib"
projects[jquery_update][version] = "2.3"

; Required for OPDK
projects[ldap][type] = "module"
projects[ldap][subdir] = "contrib"

projects[legal][type] = "module"
projects[legal][subdir] = "contrib"

projects[libraries][type] = "module"
projects[libraries][subdir] = "contrib"

projects[linkchecker][type] = "module"
projects[linkchecker][subdir] = "contrib"

projects[link][type] = "module"
projects[link][subdir] = "contrib"

projects[logintoboggan][type] = "module"
projects[logintoboggan][subdir] = "contrib"

projects[mailsystem][type] = "module"
projects[mailsystem][subdir] = "contrib"

projects[markdown][type] = "module"
projects[markdown][subdir] = "contrib"

projects[mediaelement][type] = "module"
projects[mediaelement][subdir] = "contrib"

projects[media][type] = "module"
projects[media][subdir] = "contrib"
projects[media][version] = "2.0-alpha3"
; see http://drupal.org/node/2232703
projects[media][patch][2232703] = "https://drupal.org/files/issues/media-views-2232703-5.patch"

projects[media_youtube][type] = "module"
projects[media_youtube][subdir] = "contrib"

projects[menu_attributes][type] = "module"
projects[menu_attributes][subdir] = "contrib"

projects[message][type] = "module"
projects[message][subdir] = "contrib"

projects[metatag][type] = "module"
projects[metatag][subdir] = "contrib"

projects[me][type] = "module"
projects[me][subdir] = "contrib"

projects[migrate][type] = "module"
projects[migrate][subdir] = "contrib"

projects[mimemail][type] = "module"
projects[mimemail][subdir] = "contrib"

projects[module_filter][type] = "module"
projects[module_filter][subdir] = "contrib"

projects[nagios][type] = "module"
projects[nagios][subdir] = "contrib"

projects[navbar][type] = "module"
projects[navbar][subdir] = "contrib"

; Deprecated?
projects[node_export][type] = "module"
projects[node_export][subdir] = "contrib"

projects[oauth][type] = "module"
projects[oauth][subdir] = "contrib"

projects[oauthconnector][type] = "module"
projects[oauthconnector][subdir] = "contrib"

projects[pathauto][type] = "module"
projects[pathauto][subdir] = "contrib"

projects[readonlymode][type] = "module"
projects[readonlymode][subdir] = "contrib"

projects[recaptcha][type] = "module"
projects[recaptcha][subdir] = "contrib"

projects[redirect][type] = "module"
projects[redirect][subdir] = "contrib"

projects[redis][type] = "module"
projects[redis][subdir] = "contrib"
; see http://drupal.org/node/2369946

projects[remote_stream_wrapper][type] = "module"
projects[remote_stream_wrapper][subdir] = "contrib"

projects[rules][type] = "module"
projects[rules][subdir] = "contrib"

projects[services][type] = "module"
projects[services][subdir] = "contrib"
; see http://drupal.org/node/2369946
projects[services][patch][2369946] = "https://www.drupal.org/files/issues/services.spyc_make-2369946-2.patch"


projects[services_views][type] = "module"
projects[services_views][subdir] = "contrib"

; Required for OPDK
projects[siteminder][type] = "module"
projects[siteminder][subdir] = "contrib"

projects[smtp][type] = "module"
projects[smtp][subdir] = "contrib"

; Required by Monetization
projects[special_menu_items][type] = "module"
projects[special_menu_items][subdir] = "contrib"

projects[strongarm][type] = "module"
projects[strongarm][subdir] = "contrib"

projects[syntaxhighlighter][type] = "module"
projects[syntaxhighlighter][subdir] = "contrib"

projects[token][type] = "module"
projects[token][subdir] = "contrib"

projects[translation_overview][type] = "module"
projects[translation_overview][subdir] = "contrib"

projects[util][type] = "module"
projects[util][subdir] = "contrib"

projects[uuid][type] = "module"
projects[uuid][subdir] = "contrib"

; Deprecated
projects[uuid_features][type] = "module"
projects[uuid_features][subdir] = "contrib"
projects[uuid_features][version] = "1.x-dev"

projects[variable][type] = "module"
projects[variable][subdir] = "contrib"

projects[views][type] = "module"
projects[views][subdir] = "contrib"
; see http://drupal.org/node/1809958
projects[views][patch][1809958] = "http://drupal.org/files/issues/fix-views-missing-dom-id-1809958-7.patch"
; see http://drupal.org/node/1036962
projects[views][patch][1036962] = "http://drupal.org/files/views-fix-destination-link-for-ajax-1036962-29.patch"

projects[views_accordion][type] = "module"
projects[views_accordion][subdir] = "contrib"

projects[views_bulk_operations][type] = "module"
projects[views_bulk_operations][subdir] = "contrib"

projects[views_queue][type] = "module"
projects[views_queue][subdir] = "contrib"

projects[views_slideshow][type] = "module"
projects[views_slideshow][subdir] = "contrib"

projects[webform][type] = "module"
projects[webform][subdir] = "contrib"

; Prevents XSS in Full HTML
projects[wysiwyg_filter][type] = "module"
projects[wysiwyg_filter][subdir] = "contrib"

projects[xautoload][type] = "module"
projects[xautoload][subdir] = "contrib"



; --- THEMES ---

projects[bootstrap][type] = "theme"

projects[rubik][type] = "theme"
projects[rubik][version] = "4.1"

projects[tao][type] = "theme"



; --- LIBRARIES ---

; Used by navbar et. al.
libraries[backbone][destination] = "libraries"
libraries[backbone][directory_name] = "backbone"
libraries[backbone][download][type] = "git"
libraries[backbone][download][url] = "git://github.com/jashkenas/backbone.git"

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

; Used by jquery_colorpicker
libraries[colorpicker][destination] = "libraries"
libraries[colorpicker][directory_name] = "colorpicker"
libraries[colorpicker][download][type] = "file"
libraries[colorpicker][download][url] = "http://www.eyecon.ro/colorpicker/colorpicker.zip"

; Used by gauth
libraries[google-api-php-client][destination] = "libraries"
libraries[google-api-php-client][directory_name] = "google-api-php-client"
libraries[google-api-php-client][download][type] = "file"
libraries[google-api-php-client][download][url] = "https://google-api-php-client.googlecode.com/files/google-api-php-client-0.6.0.tar.gz"

; Used by devconnect_developer_apps
libraries[highcharts][destination] = "libraries"
libraries[highcharts][directory_name] = "highcharts"
libraries[highcharts][download][type] = "file"
libraries[highcharts][download][url] = "http://code.highcharts.com/zips/Highcharts-3.0.7.zip"

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

; Used by views_slideshow
libraries[json2][destination] = "libraries"
libraries[json2][directory_name] = "json2"
libraries[json2][download][type] = "git"
libraries[json2][download][url] = "git://github.com/douglascrockford/JSON-js.git"

; Used by devconnect_monetization
libraries[maskmoney][destination] = "libraries"
libraries[maskmoney][directory_name] = "maskmoney"
libraries[maskmoney][download][type] = "file"
libraries[maskmoney][download][url] = "https://raw.github.com/plentz/jquery-maskmoney/master/src/jquery.maskMoney.js"

; Used by mediaelement
libraries[mediaelement][destination] = "libraries"
libraries[mediaelement][directory_name] = "mediaelement"
libraries[mediaelement][download][type] = "git"
libraries[mediaelement][download][url] = "git://github.com/johndyer/mediaelement.git"

; Used by devconnect
libraries[mgmt-api-php-sdk][destination] = "libraries"
libraries[mgmt-api-php-sdk][directory_name] = "mgmt-api-php-sdk"
libraries[mgmt-api-php-sdk][download][type] = "git"
libraries[mgmt-api-php-sdk][download][url] = "git://github.com/apigee/edge-php-sdk.git"

; Used by navbar
libraries[modernizr][destination] = "libraries"
libraries[modernizr][directory_name] = "modernizr"
libraries[modernizr][download][type] = "file"
libraries[modernizr][download][url] = "http://modernizr.com/downloads/modernizr-2.8.3.js"
libraries[modernizr][filename] = "modernizr.js"

; Used by media, file_entity
libraries[plupload][destination] = "libraries"
libraries[plupload][directory_name] = "plupload"
libraries[plupload][download][type] = "file"
libraries[plupload][download][url] = "https://github.com/moxiecode/plupload/archive/v2.0.0.zip"

; Used by apachesolr
libraries[SolrPhpClient][destination] = "libraries"
libraries[SolrPhpClient][directory_name] = "SolrPhpClient"
libraries[SolrPhpClient][download][type] = "file"
libraries[SolrPhpClient][download][url] = "http://solr-php-client.googlecode.com/files/SolrPhpClient.r60.2011-05-04.zip"

; Used by syntaxhighlighter
libraries[syntaxhighlighter][destination] = "libraries"
libraries[syntaxhighlighter][directory_name] = "syntaxhighlighter"
libraries[syntaxhighlighter][download][type] = "file"
libraries[syntaxhighlighter][download][url] = "http://alexgorbatchev.com/SyntaxHighlighter/download/download.php?sh_current"

; Used by navbar
libraries[underscore][destination] = "libraries"
libraries[underscore][directory_name] = "underscore"
libraries[underscore][download][type] = "file"
libraries[underscore][download][url] = "http://documentcloud.github.io/underscore/underscore-min.js"

