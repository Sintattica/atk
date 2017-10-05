// JQUERY ONLOAD
jQuery(function () {
    jQuery("form #action-buttons button").on("click", function () {
        jQuery("form #action-buttons button").removeAttr("clicked");
        jQuery(this).attr("clicked", "true");
    });

    // resize window fix smartmenu rendering
    setTimeout(function(){
        jQuery(window).trigger('resize');
    }, 10);
});


// JQUERY EXTENSIONS

jQuery.fn.visible = function () {
    return this.css('visibility', 'visible');
};

jQuery.fn.invisible = function () {
    return this.css('visibility', 'hidden');
};

jQuery.fn.visibilityToggle = function () {
    return this.css('visibility', function (i, visibility) {
        return (visibility === 'visible') ? 'hidden' : 'visible';
    });
};


// JQUERY AJAX

jQuery(document).ajaxStart(function () {
    jQuery('#atkbusy').visible();
});

jQuery(document).ajaxStop(function () {
    jQuery('#atkbusy').invisible();
});

jQuery(document).ajaxError(function (event, jqxhr, settings, thrownError) {
    if (typeof (console) === 'object' && console.debug) {
        console.debug('Exception in Ajax.Request:', thrownError);
    }
});


// SELECT2
jQuery.fn.select2.defaults.set('theme', 'bootstrap');
if (typeof(LANGUAGE) !== 'undefined') {
    jQuery.fn.select2.defaults.set('language', LANGUAGE);
}
