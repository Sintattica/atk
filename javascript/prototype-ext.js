/**
 * This file contains overrides for the basic Prototype methods
 * to fix some minor issues or add some small functionality.
 */

// Override evalScripts to make sure scripts get executed in 
// the context of the current document. This makes it possible
// to add functions etc. in the document's context.
String.prototype.evalScripts = function() {
  return this.extractScripts().map(function(script) { 
    if (window.execScript) {
      return window.execScript(script);
    } else if (navigator.userAgent.indexOf('Safari') != -1) {
      window.setTimeout(script, 0);
    } else {
      return window.eval(script);
    }      
  });
};

// Changed:
// for (var name in headers)
//   this.transport.setRequestHeader(name, headers[name]);
//
// To:
// for (var name in headers)
//   if (typeof headers[name] != 'function')
//     this.transport.setRequestHeader(name, headers[name]);
//
// Because it was causing problems with rico.js! 
// For more information, visit: http://forum.openrico.org/topic/2228
Ajax.Request.prototype.setRequestHeaders = function() {
  var headers = {
    'X-Requested-With': 'XMLHttpRequest',
    'X-Prototype-Version': Prototype.Version,
    'Accept': 'text/javascript, text/html, application/xml, text/xml, */*'
  };

  if (this.method == 'post') {
    headers['Content-type'] = this.options.contentType +
      (this.options.encoding ? '; charset=' + this.options.encoding : '');

    /* Force "Connection: close" for older Mozilla browsers to work
     * around a bug where XMLHttpRequest sends an incorrect
     * Content-length header. See Mozilla Bugzilla #246651.
     */
    if (this.transport.overrideMimeType &&
        (navigator.userAgent.match(/Gecko\/(\d{4})/) || [0,2005])[1] < 2005)
          headers['Connection'] = 'close';
  }

  // user-defined headers
  if (typeof this.options.requestHeaders == 'object') {
    var extras = this.options.requestHeaders;

    if (typeof extras.push == 'function')
      for (var i = 0, length = extras.length; i < length; i += 2)
        headers[extras[i]] = extras[i+1];
    else
      $H(extras).each(function(pair) { headers[pair.key] = pair.value });
  }

  for (var name in headers)
    if (typeof headers[name] != 'function')
      this.transport.setRequestHeader(name, headers[name]);
};

// Updater extends Request, also update the setRequestHeaders method.
Ajax.Updater.prototype.setRequestHeaders = Ajax.Request.prototype.setRequestHeaders;