if (!ATK) {
    var ATK = {};
}

ATK.TabbedPane = {
    showTab: function (paneAttrName, selectedPaneTabName) {
        let paneAttr = jQuery('#' + paneAttrName);
        let attrs = paneAttr.find('.tabbedPaneAttr');
        let paneTabs = paneAttr.find('.tabbedPaneTab');

        let input = jQuery('input[name="' + paneAttrName.substring(10) + '"]');
        input.val(selectedPaneTabName.substring(13));

        // show attributes of the selected pane tab
        attrs.each(function (index, attr) {
            let $attr = jQuery(attr);
            let id = $attr.attr('id');
            if (id !== null && id.substring(0, 3) === 'ar_') {
                return;
            }

            if ($attr.hasClass(selectedPaneTabName)) {
                $attr.show();
                ATK.Tools.enableSelect2($attr);
            } else {
                $attr.hide();
            }
        });

        // make paneTabs active or passive
        paneTabs.each(function (index, paneTab) {
            let $paneTab = jQuery(paneTab);
            let navLink = $paneTab.children('.nav-link');
            if ($paneTab.hasClass(selectedPaneTabName)) {
                navLink.addClass('active activetab').removeClass('passivetab');
            } else {
                navLink.addClass('passivetab').removeClass('active activetab');
            }
        });
    }
};
