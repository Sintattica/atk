if (!window.ATK) {
    var ATK = {};
}

ATK.Attribute = {
    /**
     * Refresh the attribute input form using Ajax.
     */
    callDependencies: function (url, el) {
        var form = null, pn = el.parentNode;

        // Loop trough the previous nodes to find the parent form element of our
        // element. We stop searching when we reached the body element. 
        while (pn.tagName !== 'body' && pn.tagName !== 'BODY') {
            if (pn.tagName === 'form' || pn.tagName === 'FORM') {
                form = pn;
                break;
            } else {
                pn = pn.parentNode;
            }
        }

        if (form === null) {
            return;
        }

        ATK.Attribute.refreshForm(form, url);
    },
    /**
     * Refresh the attribute input form using Ajax.
     */
    refresh: function (url) {
        ATK.Attribute.refreshForm('#entryform', url);
    },
    serializeShuttle: function (element) {
        var values = [];
        var length = element.length;
        var pair = {};

        if (!length) {
            return null;
        }
        for (var i = 0; i < length; i++) {
            var opt = element.options[i];
            values.push(jQuery(opt).val());
        }
        pair[element.name] = values;
        return jQuery.param(pair);
    },
    refreshDisplay: function (url) {
        // ajax call and execute scripts
        jQuery.post(url, [], function (responseText) {
            jQuery('<div>').html(responseText).find("script").each(function () {
                var text = jQuery(this).text();
                if(text) {
                    jQuery.globalEval(text);
                }
            });
        });
    },
    refreshForm: function (form, url) {
        var elements = jQuery(form).find(':input');
        var queryComponents = [];

        elements.each(function (index, el) {
            var $el = jQuery(el);
            var name = $el.attr('name');
            var queryComponent = null;
            if (name && name.substring(0, 3) !== 'atk') {
                if ($el.hasClass('shuttle_select') && name.substring(name.length - 4) !== '_sel') {
                    queryComponent = ATK.Attribute.serializeShuttle(el);
                } else {
                    queryComponent = $el.serialize();
                }
            }
            if (queryComponent) {
                queryComponents.push(queryComponent);
            }
        });

        // atkErrorFields is a global array
        if (typeof atkErrorFields !== 'undefined') {
            jQuery(atkErrorFields).each(function (index, field) {
                var queryComponent = {
                    'atkerrorfields[]': field
                };

                queryComponents.push(jQuery.param(queryComponent));
            });
        }

        // ajax call and execute scripts
        jQuery.post(url, queryComponents.join('&'), function (responseText) {
            jQuery('<div>').html(responseText).find("script").each(function () {
                var text = jQuery(this).text();
                if(text) {
                    jQuery.globalEval(text);
                }
            });
        });
    }
};
