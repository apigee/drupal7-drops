// Note: This automatic widget to dialog window binding (the fact that every field is set up from the widget
// and is committed to the widget) is only possible when the dialog is opened by the Widgets System
// (i.e. the widgetDef.dialog property is set).
// When you are opening the dialog window by yourself, you need to take care of this by yourself too.

CKEDITOR.dialog.add( 'trifold', function( editor ) {
  var maxImages = 3;
    
  var lang = editor.lang.trifold,
      commonLang = editor.lang.common;
      
  var dialog = {
		title: 'Edit Trifold',
		minWidth: 600,
		minHeight: 300,
		contents: [
			{
				id: 'info',
        label: lang.infoTab,
        accessKey: 'I',
				elements: [
					{
						id: 'imageType',
						type: 'select',
						label: 'Image Type',
						items: [
							[ 'Square', '' ],
							[ 'Rounded', 'rounded' ],
							[ 'Circle', 'circle' ],
							[ 'Thumbnail', 'thumbnail' ]
						],
						// When setting up this field, set its value to the "align" value from widget data.
						// Note: Align values used in the widget need to be the same as those defined in the "items" array above.
						setup: function( widget ) {
							this.setValue( widget.data.imageType );
						},
						// When committing (saving) this field, set its value to the widget data.
						commit: function( widget ) {
							widget.setData( 'imageType', this.getValue() );
						}
					}
				]
			}
		]
	};
  
  for (var ii = 1; ii <= maxImages; ii++) {
   (function (i) { // Closure the i variable so that it is available in callbacks
      // Add slide count
      dialog.contents[0].elements[0].items.push([i + ' ' + (i == 1 ? lang.slideSingular : lang.slidePlural), i]);
      // Add tab for each slide
      dialog.contents[i] = {
        id: 'image' + i,
        label: lang.image + ' ' + i,
        accessKey: i,
        elements: [
          {
            type: 'vbox',
            padding: 0,
            children: [
              {
                type: 'hbox',
                widths: ['380px', '220px'],
                align: 'right',
                children: [
                  {
                    id: 'src' + i,
                    type: 'text',
                    label: lang.imageTitle,
                    setup: function(widget) {
                      this.setValue(widget.data['src' + i]);
                    },
                    commit: function(widget) {
                      widget.setData('src' + i, this.getValue());
                    },
                    validate: CKEDITOR.dialog.validate.notEmpty(lang.urlMissing)
                  },
                  {
                    type: 'button',
                    id: 'browse',
                    style: 'display:inline-block;margin-top:16px;',
                    align: 'center',
                    label: commonLang.browseServer,
                    hidden: true,
                    filebrowser: 'image' + i + ':src' + i
                  }
                ]
              }
            ]
          },
          {
            id: 'alt' + i,
            type: 'text',
            label: lang.alt,
            setup: function(widget) {
              this.setValue(widget.data['alt' + i]);
            },
            commit: function(widget) {
              widget.setData('alt' + i, this.getValue());
            }
          },
          {
            id: 'height' + i,
            type: 'text',
            label: editor.lang.common.height,
            setup: function(widget) {
              this.setValue(widget.data['height' + i]);
            },
            commit: function(widget) {
              widget.setData('height' + i, this.getValue());
            }
          },
          {
            id: 'width' + i,
            type: 'text',
            label: editor.lang.common.width,
            setup: function(widget) {
              this.setValue(widget.data['width' + i]);
            },
            commit: function(widget) {
              widget.setData('width' + i, this.getValue());
            }
          },
        ],
      };
    }(ii));
  }
  
  return dialog;
} );