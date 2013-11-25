if (!jQuery.fn.on && !jQuery.fn.off) {
  jQuery.fn.on = jQuery.fn.bind;
  jQuery.fn.off = jQuery.fn.unbind;
}
