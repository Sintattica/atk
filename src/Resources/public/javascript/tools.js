// ATK TOOLS
if (!window.ATK) {
    var ATK = {};
}

ATK.Tools = {
    scripts: null,
    absScriptPath: function (url) {
        var script = document.createElement('script');
        script.src = url;
        return script.src;
    },
    loadScript: function (url) {
        if (ATK.Tools.scripts === null) {
            ATK.Tools.scripts = [];
            jQuery('script').each(function (index, script) {
                if (script.src) {
                    ATK.Tools.scripts.push(ATK.Tools.absScriptPath(script.src));
                }
            });
        }

        var absUrl = ATK.Tools.absScriptPath(url);
        if (ATK.Tools.scripts.indexOf(absUrl) < 0) {
            ATK.Tools.scripts.push(absUrl);
            jQuery.getScript(absUrl);
        }
    },
    loadStyle: function (url, media) {
        var head = document.getElementsByTagName("head")[0];
        var css = document.createElement('link');
        css.type = 'text/css';
        css.rel = 'stylesheet';
        css.media = media || 'all';
        css.href = url;
        head.appendChild(css);
    },

    /**
     * For getting objects but perserving backwards compatibility
     */
    get_object: function (name) {
        if (document.getElementById) {
            return document.getElementById(name);
        } else if (document.all) {
            return document.all[name];
        } else if (document.layers) {
            return document.layers[name];
        }
        return false;
    },

    /**
     * Toggles the display on an object
     */
    toggleDisplay: function (name, obj) {
        if (obj.style.display === "none") {
            obj.style.display = "";
        } else {
            obj.style.display = "none";
        }
    },

    /**
     * Transforms the first character of string to uppercase
     * e.g. kittie => Kittie
     */

    ucfirst: function (stringtt) {
        return stringtt.charAt(0).toUpperCase() + stringtt.substring(1, stringtt.length)
    },

    /**
     * Replace an occurrence of a string
     */
    str_replace: function (haystack, needle, replace, casesensitive) {
        if (casesensitive)
            return (haystack.split(needle)).join(replace);

        needle = needle.toLowerCase();

        var replaced = "";
        var needleindex = haystack.toLowerCase().indexOf(needle);
        while (needleindex > -1) {
            replaced += haystack.substring(0, needleindex) + replace;
            haystack = haystack.substring(needleindex + needle.length);
            needleindex = haystack.toLowerCase().indexOf(find);
        }
        return (replaced + haystack);
    },

    /**
     * Gets the atkselector of the current node
     */
    getCurrentSelector: function () {
        var selectorobj = ATK.Tools.get_object("atkselector");
        var selector;
        if (selectorobj) {
            if (selectorobj.value) {
                selector = selectorobj.value;
            } else if (selectorobj.innerHTML) {
                selector = selectorobj.innerHTML;
            }
        }
        return selector;
    },

    /**
     * Gets the atknodetype of the current node
     */
    getCurrentNodetype: function () {
        var nodetypeobj = ATK.Tools.get_object("atknodeuri");
        var nodetype;
        if (nodetypeobj) {
            // IE works with .value, while the Gecko engine uses .innerHTML
            if (nodetypeobj.value) {
                nodetype = nodetypeobj.value;
            } else if (nodetypeobj.innerHTML) {
                nodetype = nodetypeobj.innerHTML;
            }
        }
        return nodetype;
    },

    hideAttrib: function (attrib) {
        var el = document.getElementById('ar_' + attrib);
        el.style.display = "none";
    },

    showAttrib: function (attrib) {
        var el = document.getElementById('ar_' + attrib);
        el.style.display = '';
    },

    newWindow: function (mypage, myname, w, h, scroll, resize, statusbar, menubar, toolbar, personalbar, titlebar) {
        var winl = 10;
        var wint = 10;
        scroll = scroll || 'no';
        statusbar = statusbar || 'no';
        menubar = menubar || 'no';
        toolbar = toolbar || 'no';
        personalbar = personalbar || 'no';
        titlebar = titlebar || 'no';
        var resizable = resize || 'no';
        var winprops = "height=" + h + ",width=" + w + ",top=" + wint + ",left=" + winl + ",scrollbars=" + scroll + ", resizable=" + resizable + ",status=" + statusbar + ",menubar=" + menubar + ",toolbar=" + toolbar + ",personalbar=" + personalbar + ",titlebar=" + titlebar;
        var win = window.open(mypage, myname, winprops);
        if (parseInt(navigator.appVersion) >= 4) {
            win.window.focus();
        }
    },

    showAttribute: function (rowId) {
        var row = jQuery('#' + rowId);
        if (row) {
            row.removeClass('atkAttrRowHidden');
            row.find('select[data-enable-select2="1"]').each(function () {
                ATK.Tools.enableSelect2ForSelect(this);
            });
        }
    },

    hideAttribute: function (rowId) {
        jQuery('#' + rowId).addClass('atkAttrRowHidden');
    },

    hideTab: function (tabId) {
        jQuery('#' + tabId).hide();
    },

    showTab: function (tabId) {
        jQuery('#' + tabId).show();
    },

    enableSelect2: function ($container) {
        jQuery.each($container.find('select[data-enable-select2]'), function (idx, el) {
            ATK.Tools.enableSelect2ForSelect(el);
        });
    },

    enableSelect2ForSelect: function (select) {
        // var select2, width;
        const $el = jQuery(select);
        let options = {};

        if ($el.is('select')) {
            if (typeof $el.data('with-empty-value') !== 'undefined') {
                options.templateSelection = function (data) {
                    if (data.id === $el.data('with-empty-value')) {
                        return jQuery('<span class="select-empty-value">' + data.text + '</span>');
                    }
                    return data.text;
                };
            }

            if (typeof $el.data('no-search') !== 'undefined') {
                options.minimumResultsForSearch = Infinity;
            }

            if ($el.data('enable-manytoonereleation-autocomplete')) {
                options = jQuery.extend(options, ATK.ManyToOneRelation.autocomplete);
            }

            $el.select2(options);

            if ($el.css('min-width') !== '0px') {
                $el.next('span.select2-container').css({minWidth: $el.css('min-width'), width: ''});
            }
        }
    },

    debounce: function (func, wait, immediate) {
        // taken fron http://underscorejs.org/
        var timeout, args, context, timestamp, result;

        var later = function () {
            var last = Date.now() - timestamp;

            if (last < wait && last >= 0) {
                timeout = setTimeout(later, wait - last);
            } else {
                timeout = null;
                if (!immediate) {
                    result = func.apply(context, args);
                    if (!timeout) context = args = null;
                }
            }
        };

        return function () {
            context = this;
            args = arguments;
            timestamp = Date.now();
            var callNow = immediate && !timeout;
            if (!timeout) timeout = setTimeout(later, wait);
            if (callNow) {
                result = func.apply(context, args);
                context = args = null;
            }

            return result;
        };
    }
};
