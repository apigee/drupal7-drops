CKEDITOR.plugins.add('trifold', {
  lang: 'en',
  requires: 'widget,dialog',
  icons: 'trifold',
  init: function(editor) {
    CKEDITOR.dialog.add('trifold', this.path + 'dialogs/trifold.js');

    editor.widgets.add('trifold', {
      allowedContent:
              'div(!trifold,align-left,align-right,align-center){width};' +
              'div(!trifold-content); h2(!trifold-title)',
      requiredContent: 'div(trifold)',
      editables: {
        heading1: {
          selector: 'div.trifold > div:nth-child(1) h2',
          allowedContent: 'br strong em'
        },
        heading2: {
          selector: 'div.trifold > div:nth-child(2) h2',
          allowedContent: 'br strong em'
        },
        heading3: {
          selector: 'div.trifold > div:nth-child(3) h2',
          allowedContent: 'br strong em'
        },
        content1: 'div.trifold > div:nth-child(1) div.content',
        content2: 'div.trifold > div:nth-child(2) div.content',
        content3: 'div.trifold > div:nth-child(3) div.content',
      },
      parts: {
        image1: 'div.trifold > div:nth-child(1) img',
        image2: 'div.trifold > div:nth-child(2) img',
        image3: 'div.trifold > div:nth-child(3) img',
      },
      template:
              '<div class="row trifold text-center">' +
              '  <div class="col-md-4">' +
              '    <img class="img-circle" src="http://placehold.it/140x140" alt="140x140" style="height: 140px; width: 140px;">' +
              '    <h2>Heading 1</h2>' +
              '    <div class="content">' +
              '      <p>Donec sed odio dui. Etiam porta sem malesuada magna mollis euismod. Nullam id dolor id nibh ultricies vehicula ut id elit. Morbi leo risus, porta ac consectetur ac, vestibulum at eros. Praesent commodo cursus magna.</p>' +
              '      <p><a class="btn btn-default" href="#" role="button">View details »</a></p>' +
              '    </div>' +
              '  </div>' +
              '  <div class="col-md-4">' +
              '    <img class="img-circle" src="http://placehold.it/140x140" alt="140x140" style="height: 140px; width: 140px;">' +
              '    <h2>Heading 2</h2>' +
              '    <div class="content">' +
              '      <p>Duis mollis, est non commodo luctus, nisi erat porttitor ligula, eget lacinia odio sem nec elit. Cras mattis consectetur purus sit amet fermentum. Fusce dapibus, tellus ac cursus commodo, tortor mauris condimentum nibh.</p>' +
              '      <p><a class="btn btn-default" href="#" role="button">View details »</a></p>' +
              '    </div>' +
              '  </div>' +
              '  <div class="col-md-4">' +
              '    <img class="img-circle" src="http://placehold.it/140x140" alt="140x140" style="height: 140px; width: 140px;">' +
              '    <h2>Heading 3</h2>' +
              '    <div class="content">' +
              '      <p>Donec sed odio dui. Cras justo odio, dapibus ac facilisis in, egestas eget quam. Vestibulum id ligula porta felis euismod semper. Fusce dapibus, tellus ac cursus commodo, tortor mauris condimentum nibh, ut fermentum massa justo sit amet risus.</p>' +
              '      <p><a class="btn btn-default" href="#" role="button">View details »</a></p>' +
              '    </div>' +
              '  </div>' +
              '</div>',
      button: 'Create a trifold',
      dialog: 'trifold',
      upcast: function(element) {
        return element.name == 'div' && element.hasClass('trifold');
      },
      init: function() {
        var image1 = this.parts.image1;
        if (image1.hasClass('img-rounded'))
          this.setData('imageType', 'rounded');
        else if (image1.hasClass('img-circle'))
          this.setData('imageType', 'circle');
        else if (image1.hasClass('img-thumbnail'))
          this.setData('imageType', 'thumbnail');
        else
          this.setData('imageType', '');
        for (var i=1;i<=3;i++) {
          var image = this.parts['image' + i];
          this.setData('src' + i, image.getAttribute('src'));
          this.setData('alt' + i, image.getAttribute('alt'));
          this.setData('height' + i, image.getStyle('height'));
          this.setData('width' + i, image.getStyle('width'));
        }
      },
      data: function() {

        for (var i=1;i<=3;i++) {
          var image = this.parts['image' + i];
          image.removeClass('img-rounded');
          image.removeClass('img-circle');
          image.removeClass('img-thumbnail');
          if (this.data.imageType) {
            image.addClass('img-' + this.data.imageType);
          }
          image.setAttribute('src', this.data['src' + i]);
          // CKEditor is adding some stuff that causes the image not to be saved.
          image.removeAttribute('data-cke-saved-src');
          image.setAttribute('alt', this.data['alt' + i]);
          image.setStyle('height', this.data['height' + i]);
          image.setStyle('width', this.data['width' + i]);
        }
      }
    });
  }
});
