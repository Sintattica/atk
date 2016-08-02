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
    var $el = jQuery(select);
    var options = {};
    var select2, width;

    if ($el.is('select')) {
        if (typeof $el.data('with-empty-value') !== 'undefined') {
            options.templateSelection = options.templateResult = function (data) {
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

        select2 = $el.next('span.select2-container');
        width = select2.css('width');
        if (width.slice(-2) == 'px' && width.slice(0, -2) != '0') {
            select2.css({maxWidth: width, width: ''});
        }
    }
};
