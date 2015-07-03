(function () {
    jQuery(document).ready(function () {

        var $all              = jQuery('*'), /* TODO: select only menu items ? */
            $submenuLinks     = jQuery('.dropdown-submenu > a'),
            isBootstrapEvent  = false,
            allBootsrapEvents = 'hide.bs.dropdown' +
                                ' hide.bs.collapse' +
                                ' hide.bs.modal' +
                                ' hide.bs.tooltip' +
                                ' hide.bs.popover' +
                                'hide.bs.submenu'
            ;

        /* init bootstrap-submenu */
        $submenuLinks.submenupicker();

        // Fix Twitter Bootstrap 3 dropdown menu disappears when used with prototype.js
        // (http://stackoverflow.com/questions/19139063/twitter-bootstrap-3-dropdown-menu-disappears-when-used-with-prototype-js)
        $all.on(allBootsrapEvents, function () {
            isBootstrapEvent = true;
        });
        var originalHide = Element.hide;
        Element.addMethods({
            hide: function (element) {
                if (isBootstrapEvent) {
                    isBootstrapEvent = false;
                    return element;
                }
                return originalHide(element);
            }
        });
    });
})();
