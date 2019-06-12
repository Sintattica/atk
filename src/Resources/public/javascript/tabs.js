if (!window.ATK) {
    var ATK = {};
}

ATK.Tabs = {
    closedSections: [],

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
        // If we are called without a name, we check if the parent has a stored tab for our page
        // If so, then we go there, else we go to the first tab (most of the time the 'default' tab)
        if (!tab) {
            tab = ATK.Tabs.getCurrentTab();
            if (tab) {
                // However if for some reason this tab does not exist, we switch to the default tab
                if (!document.getElementById('tab_' + tab))
                    tab = tabs[0];
            }
            else {
                tab = tabs[0];
            }
        }

        // Then we store what tab we are going to visit in the parent
        ATK.Tabs.setCurrentTab(tab);

        var sectionItems = jQuery('div.section-item');
        sectionItems.each(function (index, el) {
            var $el = jQuery(el);
            var show = $el.attr('class').includes('section_'+tab);
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

        jQuery(tabs).each(function (index, label) {
            var $tab = jQuery('#tab_' + label);
            if (label === tab) {
                $tab.addClass('activetab active').removeClass('passivetab');
            } else {
                $tab.removeClass('activetab active').addClass('passivetab');
            }
        });
    },

    getCurrentTab: function () {
        var getUriParams = function (name) {
            return decodeURIComponent((new RegExp('[?|&]' + name + '=' + '([^&;]+?)(&|#|;|$)').exec(location.search) || [, ""])[1].replace(/\+/g, '%20')) || null;
        };
        if (getUriParams('stateful') == '1') {
            return 'ATK.Tabs.getTab(ATK.Tools.getCurrentNodetype(), ATK.Tools.getCurrentSelector())';
        }
        return '';
    },

    getTab: function (nodetype, selector) {
        ATK.Tabs.initTabArray(nodetype, selector);
        return parent.document.tab[nodetype][selector];
    },

    setCurrentTab: function (value) {
        ATK.Tabs.setTab(ATK.Tools.getCurrentNodetype(), ATK.Tools.getCurrentSelector(), value);

        for (var i = 0; i < document.forms.length; i++) {
            var form = document.forms[i];
            if (form.atktab != null) {
                form.atktab.value = value;
                form.atktab.defaultValue = value;
            } else {
                var input = document.createElement('input');
                input.setAttribute('type', 'hidden');
                input.setAttribute('name', 'atktab');
                input.setAttribute('value', value);
                input.defaultValue = value;
                form.appendChild(input);
            }
        }
    },

    setTab: function (nodetype, selector, value) {
        ATK.Tabs.initTabArray(nodetype, selector);
        parent.document.tab[nodetype][selector] = value;
    },

    /**
     * Makes sure we don't get any nasty JS errors by making sure
     * the arrays we use are always set before using them.
     */
    initTabArray: function (nodetype, selector) {
        if (!parent.document.tab) {
            parent.document.tab = [];
        }
        if (!parent.document.tab[nodetype]) {
            parent.document.tab[nodetype] = [];
        }
    }
};
