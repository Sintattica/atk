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
        if (ATK.Tools.scripts == null) {
            ATK.Tools.scripts = $A();
            $$('script').each(function (script) {
                if (script.src) {
                    ATK.Tools.scripts.push(ATK.Tools.absScriptPath(script.src));
                }
            });
        }

        var url = ATK.Tools.absScriptPath(url);
        if (ATK.Tools.scripts.indexOf(url) < 0) {
            ATK.Tools.scripts.push(url);
            new ATK.Tools.ScriptRequest(url);
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
    debounce: function(func, wait, immediate) {
        // taken fron http://underscorejs.org/
        var timeout, args, context, timestamp, result;

        var later = function() {
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

        return function() {
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

ATK.Tools.ScriptRequest = Class.create();
Object.extend(Object.extend(ATK.Tools.ScriptRequest.prototype, Ajax.Request.prototype), {
    initialize: function (url) {
        Ajax.Request.prototype.initialize.apply(this, [url, {
            asynchronous: false, method: 'get', evalJS: false,
            onSuccess: this.evalResponse.bind(this)
        }]);
    },
    evalResponse: function (transport) {
        try {
            var script = transport.responseText + "\n";
            if (window.execScript) {
                window.execScript(script);
            } else if (navigator.userAgent.indexOf('Safari') != -1) {
                window.setTimeout(script, 0);
            } else {
                return window.eval(script);
            }
        } catch (e) {
            this.dispatchException(e);
        }
    }
});