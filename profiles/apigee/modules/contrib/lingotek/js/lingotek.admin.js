/**
 * @file
 * Custom javascript.
 */

(function ($) {

Drupal.behaviors.lingotekAdminForm = {
  attach: function (context) {

    $('td:first-child .form-checkbox', context).click( function() {
      isChecked = $(this).attr('checked');
      $(this).parents('tr').find('.form-checkbox').each( function() {
        $(this).attr('checked', isChecked);
      })
    });
    
    $('.field.form-checkbox', context).click( function() {
      row = $(this).parents('tr')
      if($(this).attr('checked')) {
        row.find('td:first-child .form-checkbox').each( function() {
          $(this).attr('checked', true);
        })
      } else {
        count = 0;
        row.find('.field.form-checkbox').each( function() {
          count += $(this).attr('checked') ? 1 : 0;
        })
        if(count == 0) {
          row.find('td:first-child .form-checkbox').attr('checked',false);
        }
      }
    });

    $('fieldset.lingotek-account', context).drupalSetSummary(function (context) {
      return Drupal.t($('#account_summary').val() + ' / ' + $('#connection_summary').val());
    });
    
//    $('fieldset.lingotek-connection-status', context).drupalSetSummary(function (context) {
//      return Drupal.t();
//    });
    
    $('fieldset.lingotek-translate-content', context).drupalSetSummary(function (context) {
      $list = [];
      total = 0;
      $('#edit-node-translation input').each(function( index ) {
        if($(this).attr('id').substring(0, 9) == 'edit-type') {
          if($(this).attr('checked') == '1') {
            $list.push($(this).val());
          }
          total++;
        }
      });
      if($list.length == 0) {
        return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
      } else {
        return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.length + '/' + total + ' ' + Drupal.t('content types');
      }
    });
    
    $('fieldset.lingotek-translate-comments', context).drupalSetSummary(function (context) {
      $list = [];
      total = 0;
      $('#edit-lingotek-translate-comments-node-types input').each(function( index ) {
        if($(this).attr('checked') == '1') {
          $list.push($(this).val());
        }
        total++;
      });
      if($list.length == 0) {
        return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
      } else {
        return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.length + '/' + total + ' ' + Drupal.t('content types');
      }
    });

    $('fieldset.lingotek-translate-configuration', context).drupalSetSummary(function (context) {
      $list = [];
      $('#edit-additional-translation input').each(function( index ) {
        if($(this).attr('checked') == '1') {
          name = $(this).attr('name');
          name = name.substring(name.lastIndexOf('_') + 1, name.length);
          $list.push(name);
        }
      });
      if($list.length == 0) {
        return '<span style="color:red;">' + Drupal.t('Disabled') + '</span>';
      } else if($list.length == 5) {
        return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: all';
      } else {
        return '<span style="color:green;">' + Drupal.t('Enabled') + '</span>: ' + $list.join(', ');
      }
    });
    
    $('fieldset.lingotek-preferences', context).drupalSetSummary(function (context) {
      $list = [];
      $('#edit-region').each(function( index ) {
        if($(this).attr('checked') == '1') {
          $list.push($(this).val());
        }
      });
      return Drupal.t($list.join(', '));
    });
  }
};

})(jQuery);
