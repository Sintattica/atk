if (!ATK) {
    var ATK = {};
}

ATK.TabbedPane = {
    showTab: function (paneName, tabName) {
        var pane = jQuery('#' + paneName);
        var attrs = pane.find('.tabbedPaneAttr');
        var tabs = pane.find('.tabbedPaneTab');

        var input = jQuery('input[name="'+paneName.substring(10)+'"]');
        input.val(tabName.substring(13));

        // show attribute of the current tab
        attrs.each(function (index, attr) {
            var $attr = jQuery(attr);
            var id = $attr.attr('id');
            if (id !== null && id.substring(0, 3) === 'ar_') {
                return;
            }

            if ($attr.hasClass(tabName)) {
                $attr.show();
                ATK.Tools.enableSelect2($attr);
            } else {
                $attr.hide();
            }
        });

        // make tabs active or passive
        tabs.each(function (index, tab) {
            var $tab = jQuery(tab);
            if ($tab.hasClass(tabName)) {
                $tab.addClass('activetab');
                $tab.addClass('active');
                $tab.removeClass('passivetab');
            } else {
                $tab.addClass('passivetab');
                $tab.removeClass('activetab');
                $tab.removeClass('active');
            }
        });
    }
};
