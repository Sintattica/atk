if (!window.ATK) {
    var ATK = {};
}

ATK.Tabs = {
    closedSections: [],
    tabs : [],
    tabstateUrl: '',

    /**
     * Register an initially closed section.
     *
     * NOTE: this method does *not* close the section!
     */
    addClosedSection: function (section) {
        ATK.Tabs.closedSections.push(section);
    },

    /**
     * Toggle section visibility.
     */
    handleSectionToggle: function (element, expand, url) {

        // automatically determine if we need to expand or collapse
        if (expand == null) {
            expand = ATK.Tabs.closedSections.indexOf(element.id) >= 0;
        }

        $('div.section-item').filter(function (id, tr) {
            return $(tr).hasClass(element.id);
        }).each(function (id, tr) {
            if (expand) {
                $(tr).show();
            } else {
                $(tr).hide();
            }
        });

        var param;
        icon = $(document.getElementById("img_"+element.id));
        if (expand) {
            param = 'opened';
            icon.removeClass('fa-plus-square-o');
            icon.addClass('fa-minus-square-o');
            ATK.Tabs.closedSections = ATK.Tabs.closedSections.filter(id => id != element.id);
        } else {
            param = 'closed';
            icon.removeClass('fa-minus-square-o');
            icon.addClass('fa-plus-square-o');
            ATK.Tabs.closedSections.push(element.id);
        }

        $.get(url+'&atksectionstate='+param);
    },

    /**
     * Sets the current tab
     */
    showTab: function (tab) {
        // If we are called without a name, we go to the first tab (most of the time the 'default' tab)
        if (!tab) {
            tab = ATK.Tabs.tabs[0];
        }

        var sectionItems = jQuery('div.section-item');
        sectionItems.each(function (index, el) {
            var $el = jQuery(el);
            var show = $el.attr('class').includes('section_'+tab) || $el.attr('class').includes('alltabs');
            ATK.Tabs.closedSections.forEach(function(section) {
                if ($el.hasClass(section)) {
                    show = false;
                }
            });

            if (show) {
                $el.show();
                ATK.Tools.enableSelect2($el);
            } else {
                $el.hide();
            }
        });

        jQuery(ATK.Tabs.tabs).each(function (index, label) {
            var $tab = $(document.getElementById('tab_' + label));
            if (label === tab) {
                $tab.addClass('activetab active').removeClass('passivetab');
            } else {
                $tab.removeClass('activetab active').addClass('passivetab');
            }
        });

        $.get(ATK.Tabs.tabstateUrl + tab);
    }
};
