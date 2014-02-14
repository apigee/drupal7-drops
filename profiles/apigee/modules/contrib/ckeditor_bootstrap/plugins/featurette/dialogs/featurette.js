"use strict"

CKEDITOR.dialog.add( 'featurette', function( editor ) {
  var lang = editor.lang.featurette,
  commonLang = editor.lang.common;

	return {
		title: 'Edit Featurette',
		minWidth: 400,
		minHeight: 300,
		contents: [
			{
				id: 'info',
        label: lang.infoTab,
        accessKey: 'I',
				elements: [
					{
						type: 'vbox',
						padding: 0,
						children: [
							{
                type: 'hbox',
                widths: [ '280px', '110px' ],
                align: 'right',
                children: [
                  {
                    id: 'src',
                    type: 'text',
                    label: commonLang.url,
                    setup: function( widget ) {
                      this.setValue( widget.data.src );
                    },
                    commit: function( widget ) {
                      widget.setData( 'src', this.getValue() );
                    },
                    validate: CKEDITOR.dialog.validate.notEmpty( lang.urlMissing )
                  },
                  {
                    // Remark: button may be removed at the very bottom of
                    // the file, if browser config is not set.
                    type: 'button',
                    id: 'browse',
                    // v-align with the 'txtUrl' field.
                    // TODO: We need something better than a fixed size here.
                    style: 'display:inline-block;margin-top:16px;',
                    align: 'center',
                    label: editor.lang.common.browseServer,
                    hidden: true,
                    filebrowser: 'info:src'
                  }
                ]
              }
            ]
          },
          {
            id: 'alt',
            type: 'text',
            label: lang.alt,
            setup: function( widget ) {
              this.setValue( widget.data.alt );
            },
            commit: function( widget ) {
              widget.setData( 'alt', this.getValue() );
            }
          },
          {
            id: 'align',
            type: 'radio',
            label: lang.alignTitle,
            items: [
              [ editor.lang.common.alignLeft, 'left' ],
              [ editor.lang.common.alignRight, 'right' ],
            ],
            setup: function( widget ) {
              this.setValue( widget.data.align );
            },
            commit: function( widget ) {
              widget.setData( 'align', this.getValue() );
            }
          },
          {
						id: 'link',
						type: 'text',
						label: lang.linkTitle,
						style: 'width: 100%',
						'default': '',
						setup: function( widget ) {
              this.setValue( widget.data.link );
						},
						commit: function( widget ) {
              widget.setData( 'link', this.getValue() );
						}
					},
          {
						id: 'target',
						type: 'select',
						requiredContent: 'a[target]',
						label: editor.lang.common.target,
						'default': '',
						items: [
							[ editor.lang.common.notSet, '' ],
							[ editor.lang.common.targetNew, '_blank' ],
							[ editor.lang.common.targetTop, '_top' ],
							[ editor.lang.common.targetSelf, '_self' ],
							[ editor.lang.common.targetParent, '_parent' ]
							],
						setup: function( widget ) {
              this.setValue( widget.data.target );
						},
						commit: function( widget ) {
              widget.setData( 'target', this.getValue() );
						}
          },
        ]
      }
    ]
  };
} );