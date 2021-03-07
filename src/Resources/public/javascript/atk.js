// JQUERY ONLOAD
jQuery(function () {
    jQuery("form #action-buttons button").on("click", function () {
        jQuery("form #action-buttons button").removeAttr("clicked");
        jQuery(this).attr("clicked", "true");
    });

    // resize window fix smartmenu rendering
    setTimeout(function () {
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
jQuery.fn.select2.defaults.set('theme', 'bootstrap4');
jQuery.fn.select2.defaults.set('containerCssClass', ':all:');
jQuery.fn.select2.defaults.set('width', null);

if (typeof (LANGUAGE) !== 'undefined') {
    jQuery.fn.select2.defaults.set('language', LANGUAGE);
}


/*
 * Admin LTE
 * In our case there can be a submenu that is open when we refresh a page (or
 * we click on another specified menu it should be displayed as open when the page loads).
 *
 * This script detects the menu element that has the active class.
 * and propagates the 'display block' property and 'menu-open' class
 * on the father elements so the menu is displayed as open on that specified element.
 */
jQuery(window).on("load", () => {

    const sidebar = document.querySelector('#menu-sidebar');
    const activeEl = sidebar.querySelector('.nav-link.active');

    // for sidebar menu entirely but not cover treeview
    jQuery('ul.nav-sidebar a').filter((index, el) => el === activeEl).addClass('active');

    // for sidebar menu and treeview
    jQuery('ul.nav-treeview a').filter((index, el) => el === activeEl)
        .parentsUntil(".nav-sidebar > .nav-treeview")
        .css({'display': 'block'})
        .addClass('menu-open')
        .prev('a').addClass('active');


    // ***AdminLTE bug ****:
    // ----------------------
    // i don't know why the TreeView plugin for the adminLTE puts a 'display:block' property
    // on every sibling sub-menu showing these former submenu elements even if their respective
    // menus are closed. This is a hack to fix this wrong behavior.
    const elements = document.querySelectorAll("ul.nav-treeview.menu-open > .nav-item:not(.menu-open) > ul");
    elements.forEach(el => el.style.display = 'none');

});
