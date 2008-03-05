if (!window.ATK) {
  var ATK = {};
}

ATK.Tools = {
  scripts: null,
  
  absScriptPath: function(url) {
    var script = document.createElement('script');
    script.src = url;
    return script.src;
  },
  
  loadScript: function(url) {
    if (ATK.Tools.scripts == null) {
      ATK.Tools.scripts = $A();
      $$('script').each(function(script) {
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
  }
};

ATK.Tools.ScriptRequest = Class.create();
Object.extend(Object.extend(ATK.Tools.ScriptRequest.prototype, Ajax.Request.prototype), {
  initialize: function(url) {
    Ajax.Request.prototype.initialize.apply(this, [url, { asynchronous: false, method: 'get' }]);
  },

  onSuccess: function() {
    if ((this.getHeader('Content-type') || 'text/javascript').strip().
      match(/^(text|application)\/(x-)?(java|ecma)script(;.*)?$/i)) {
      return;
    }
    
    this.evalResponse();
  },
  
  evalResponse: function() {
    try {
      var script = this.transport.responseText + "\n";
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