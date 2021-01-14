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
        element = $(element);


        // automatically determine if we need to expand or collapse
        if (expand == null) {
            expand = ATK.Tabs.closedSections.indexOf(element.id) >= 0;
        }

        $$('tr', 'div.atkSection', 'div.section-item').select(function (tr) {
            return $(tr).hasClassName(element.id);
        }).each(function (tr) {
            if (expand) {
                Element.show(tr);
                element.removeClassName('closedSection');
                element.addClassName('openedSection');
                ATK.Tabs.closedSections = ATK.Tabs.closedSections.without(element.id);
            } else {
                Element.hide(tr);
                element.removeClassName('openedSection');
                element.addClassName('closedSection');
                ATK.Tabs.closedSections.push(element.id);
            }
        });

        var param;
        if (expand) {
            param = 'opened';
        } else {
            param = 'closed';
        }

        new Ajax.Request(url, {
            method: 'get',
            parameters: 'atksectionstate=' + param
        });
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
            var show = ($el.hasClass('section_' + tab) && ATK.Tabs.closedSections.indexOf($el.attr('id')) < 0);

            if (show) {
                $el.show();
                ATK.Tools.enableSelect2($el);
            } else {
                $el.hide();
            }
        });

        jQuery(tabs).each(function (index, label) {
            var $tab = jQuery('#tab_' + label + "> a");
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
