jQuery.noConflict();

if (!window.ATK) {
    var ATK = {};
}

jQuery.fn.select2.defaults.set('theme', 'bootstrap');
if (LANGUAGE) {
    jQuery.fn.select2.defaults.set('language', LANGUAGE);
}

jQuery.fn.visible = function () {
    return this.css('visibility', 'visible');
};

jQuery.fn.invisible = function () {
    return this.css('visibility', 'hidden');
};

jQuery.fn.visibilityToggle = function () {
    return this.css('visibility', function (i, visibility) {
        return (visibility == 'visible') ? 'hidden' : 'visible';
    });
};

if (Prototype && Prototype.BrowserFeatures.ElementExtensions) {
    var disablePrototypeJS = function (method, pluginsToDisable) {
            var handler = function (event) {
                event.target[method] = undefined;
                setTimeout(function () {
                    delete event.target[method];
                }, 0);
            };
            pluginsToDisable.each(function (plugin) {
                jQuery(window).on(method + '.bs.' + plugin, handler);
            });
        },
        pluginsToDisable = ['collapse', 'dropdown', 'modal', 'tooltip', 'popover'];
    disablePrototypeJS('show', pluginsToDisable);
    disablePrototypeJS('hide', pluginsToDisable);
}

ATK.enableSelect2 = function ($container) {
    jQuery.each($container.find('select[data-enable-select2]'), function (idx, el) {
        ATK.enableSelect2ForSelect(el);
    });
};

ATK.enableSelect2ForSelect = ATK.refreshSelect2ForSelect = function (select) {
    // var select2, width;
    var $el = jQuery(select);
    var options = {};

    if ($el.is('select')) {
        if (typeof $el.data('with-empty-value') !== 'undefined') {
            options.templateSelection = function (data) {
                if (data.id === $el.data('with-empty-value')) {
                    return jQuery('<span class="select-empty-value">' + data.text + '</span>');
                }
                return data.text;
            };
        }

        if ($el.data('enable-manytoonereleation-autocomplete')) {
            options = jQuery.extend(options, ATK.ManyToOneRelation.autocomplete);
        }

        $el.select2(options);

        if ($el.css('min-width') !== '0px') {
            $el.next('span.select2-container').css({minWidth: $el.css('min-width'), width: ''});
        }
    }
};

ATK.showAttribute = function (rowId) {
    var row = jQuery('#' + rowId);
    if (row) {
        row.removeClass('atkAttrRowHidden');
        row.find('select[data-enable-select2="1"]').each(function () {
            ATK.enableSelect2ForSelect(this);
        });
    }
};

ATK.hideAttribute = function (rowId) {
    jQuery('#' + rowId).addClass('atkAttrRowHidden');
};
