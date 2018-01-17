if (!window.ATK) {
    var ATK = {};
}

ATK.DataGrid = {
    grids: {},
    /**
     * Registers a data grid.
     */
    register: function (name, baseUrl, embedded) {
        ATK.DataGrid.grids[name] = {
            name: name, baseUrl: baseUrl, embedded: embedded,
            locked: false, updateCompletedListeners: []
        };
        ATK.DataGrid.updateScroller(name);
    },
    /**
     * Add update completed listener.
     */
    addUpdateCompletedListener: function (name, listener) {
        ATK.DataGrid.get(name).updateCompletedListeners.push(listener);
    },
    /**
     * Returns the information for the grid with the given name.
     */
    get: function (name) {
        return ATK.DataGrid.grids[name];
    },
    /**
     * Returns the container for the grid with the given name.
     */
    getContainer: function (name) {
        return jQuery('#' + name + '_container');
    },
    /**
     * Returns the form for the grid with the given name.
     */
    getForm: function (name) {
        return ATK.DataGrid.getContainer(name).closest('form');
    },
    /**
     * Returns the grid form elements.
     */
    getElements: function (name) {
        return ATK.DataGrid.getContainer(name).find(':input');
    },
    /**
     * Updates/refreshes the data grid with the given name.
     *
     * All current parameter values will be applied, except the ones overriden
     * by the overrides.
     */
    update: function (name, plainOverrides, jsOverrides, jsCallback) {
        var grid = ATK.DataGrid.get(name);

        // prevent multiple updates to the same grid at once
        if (grid.locked) {
            return;
        }
        grid.locked = true;

        // show that grid is updating
        ATK.DataGrid.getContainer(name).fadeTo(0, 0.5);

        // overrides
        var overrides = jQuery.extend(plainOverrides, jsOverrides);
        if (jsCallback !== null) {
            overrides = jQuery.extend(overrides, jsCallback(name));
        }

        // convert overrides to query components
        var queryComponents = [];

        jQuery.each(overrides, function (k, v) {
            var key = 'atkdg_AE_' + grid.name + '_AE_' + k;
            var queryComponent;

            if (jQuery.isArray(v) && v.length > 0) {
                for (var i = 0; i < v.length; i++) {
                    queryComponent = encodeURIComponent(key) + '=' + encodeURIComponent(v[i]);
                    queryComponents.push(queryComponent);
                }
            } else {
                queryComponent = encodeURIComponent(key) + '=' + encodeURIComponent(v);
                queryComponents.push(queryComponent);
            }
        });


        if (grid.embedded) {
            var elements = ATK.DataGrid.getForm(grid.name).find(':input');
            elements.each(function(index, el){
                var $el = jQuery(el);
                var name = $el.attr('name');
                if (name && name.substring(0, 3) !== 'atk') {
                    var queryComponent = $el.serialize();
                    if (queryComponent) {
                        queryComponents.push(queryComponent);
                    }
                }
            });
        }

        queryComponents.push('atkdatagrid=' + encodeURIComponent(name));

        jQuery.post(grid.baseUrl, queryComponents.join('&'), function(data){
            ATK.DataGrid.getContainer(name).html(data);
            ATK.DataGrid.updateCompleted(name);
        });
    },
    /**
     * After update of the grid has successfully completed.
     */
    updateCompleted: function (name) {
        ATK.DataGrid.getContainer(name).fadeTo(0, 1);

        jQuery.each(ATK.DataGrid.get(name).updateCompletedListeners, function (listener) {
            listener.defer(name);
        });

        ATK.DataGrid.get(name).locked = false;
        ATK.DataGrid.updateScroller(name);
    },
    /**
     * Extracts fields from the datagrid form with the given needle in
     * the name and returns them as overrides for use in the update method.
     *
     * It would be better to be able to check for a certain prefix,
     * unfortunately not all atksearch* / atkcolcmd fields necessarily
     * have the needle at the start of the name. So for backwards
     * compatibility we search the entire string for the needle. Luckily
     * the strings we are searching for are pretty unique within a form.
     */
    extractOverrides: function (name, needle) {
        var overrides = {};

        ATK.DataGrid.getElements(name).each(function (index, el) {
            var $el = jQuery(el);
            var name = $el.attr('name');
            var v;
            if (name !== undefined && name.indexOf(needle) >= 0) {
                v = $el.val() === null ? [] : $el.val();
                overrides[name] = v;
            }
        });

        return overrides;
    },
    /**
     * Extracts the search fields from the datagrid form and returns them
     * as overrides for use in the update method.
     */
    extractSearchOverrides: function (name) {
        return ATK.DataGrid.extractOverrides(name, 'atksearch');
    },
    /**
     * Extracts the extended sort fields from the datagrid form and returns them
     * as overrides for use in the update method.
     */
    extractExtendedSortOverrides: function (name) {
        return ATK.DataGrid.extractOverrides(name, 'atkcolcmd');
    },
    /**
     * Save a datagrid which is in edit mode.
     */
    save: function (name, url) {
        var prefix = 'atkdatagriddata_AE_';
        var elements = ATK.DataGrid.getElements(name);
        var queryComponents = [];

        elements.each(function(index, el){
            var $el = jQuery(el);
            var name = $el.attr('name');
            if(name && name.substring(0, prefix.length) === prefix){
                var queryComponent = $el.serialize();
                if (queryComponent) {
                    queryComponents.push(queryComponent);
                }
            }
        });

        jQuery.ajax(url, {
            data: queryComponents.join('&'),
            success: ATK.DataGrid.update(name, {atkgridedit: 0}, {}, null)
        });
    },
    updateScroller: function (name) {
        var container = jQuery(ATK.DataGrid.getContainer(name));
        var recordListScroller = container.find('.recordListScroller');
        if (recordListScroller.length) {
            var recordListContainer = container.find('.recordListContainer');
            var scroller = recordListScroller.find('.scroller');
            var scrollWidth = recordListContainer.get(0).scrollWidth;
            var clientWidth = recordListContainer.get(0).clientWidth;
            var datagridList = container.find('.datagrid-list');

            scroller.width(scrollWidth);
            if (scrollWidth <= clientWidth) {
                recordListScroller.hide();
                datagridList.css('margin-top', 0);
            } else {
                recordListScroller.show();
                datagridList.css('margin-top', '-15px');

                var scrollTimeoutId;
                recordListScroller.scroll(function () {
                    recordListContainer.scrollLeft(recordListScroller.scrollLeft())
                });
                recordListContainer.scroll(function () {
                    clearTimeout(scrollTimeoutId);
                    scrollTimeoutId = setTimeout(function () {
                        recordListScroller.scrollLeft(recordListContainer.scrollLeft());
                    }, 50);
                });
            }
        }
    },
    updateAllScrollers: function () {
        jQuery.each(ATK.DataGrid.grids, function(key){
            ATK.DataGrid.updateScroller(key);
        });
    }
};

jQuery(window).on('resize', ATK.Tools.debounce(ATK.DataGrid.updateAllScrollers, 100));

jQuery(function () {
    jQuery(document).on('keypress', '.atkdatagrid-container .recordListSearch input[type="text"]', function (e) {
        var bt;
        if ((e.which && e.which === 13) || (e.keyCode && e.keyCode === 13)) {
            e.preventDefault();
            bt = jQuery(e.currentTarget).parent('.recordListSearch').parent('tr').find('.btn_search');
            bt.click();
            return false;
        }
    });
});