if (!window.ATK) {
  var ATK = {};
}

ATK.Dialog = Class.create();
ATK.Dialog.stack = [];
ATK.Dialog.prototype = {
  /**
   * Constructor.
   */
  initialize: function(title, url, theme, options, windowOptions) {
    this.title = title;
    this.url = url;
    this.theme = theme || 'alphacube';
    this.options = options || {};
    this.windowOptions = windowOptions || {};
  },

  /**
   * Auto-resize the dialog.
   */
  resize: function() {
    if (!this.window) return;
	  var dimensions = this.window.content.getScrollDimensions();
	  this.window.setSize(dimensions.width, dimensions.height, true); 
	  this.window.center({ auto: true });
  },

  /**
   * Serialize form.
   */
  serializeForm: function() {
    var elements = Form.getElements(this.options.serializeForm);
    var queryComponents = new Array();

    for (var i = 0; i < elements.length; i++) {
      if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
        var queryComponent = Form.Element.serialize(elements[i]);
        if (queryComponent)
          queryComponents.push(queryComponent);
      }
    }

    return queryComponents.join('&');
  },

  /**
   * Now the content is loaded into the dialog content element we
   * can finally show it to the user.
   */
  handleComplete: function() {
    this.window.content.setStyle({ visibility: 'hidden' });
    this.window.show(true);
    
    if (this.options.width && this.options.height) {
      this.window.setSize(this.options.width, this.options.height);
      this.window.center({ auto: true });      
    } else {
      this.resize();
    }

    this.window.content.setStyle({ visibility: '' });
    this.window.focus();      
  },

  /**
   * Show dialog.
   */
  show: function() {
    ATK.Dialog.stack.push(this);
    
    var windowOptions = { 
      theme: this.theme, 
      shadow: true, 
      shadowTheme: 'mac_shadow',
      superflousEffects: false,
      minimize: false,
      maximize: false,
      close: false,
      resizable: false,
      draggable: false
    }

    this.window = new UI.Window(windowOptions);
    this.window.setZIndex(1000);
    this.window.header.setStyle({ paddingRight: '0px' });
    this.window.header.removeClassName('move_handle');
    this.window.header.update(this.title.escapeHTML());
    
    var updaterOptions = {
      evalScripts: true,
      parameters: this.options.serializeForm ? this.serializeForm : null,
      onComplete: this.handleComplete.bind(this)
    }
    
    new Ajax.Updater(this.window.content, this.url, updaterOptions);
  },

  /**
   * Save dialog contents using an Ajax request to the given URL.
   * The given extraParams (optional) will be appended at the end of the
   * serialized dialog form.
   */
  save: function(url, form, extraParams, options) {
    var params = Form.serialize(form);

    if (extraParams != null)
      params += '&' + $H(extraParams).toQueryString();

    if (window.getCurrentTab)
    	params += '&atkparenttab=' + getCurrentTab();

    var options = options || {};

    var dummyFunc = function() {};
    var resizeFunc = this.options.width && this.options.height ? dummyFunc : this.resize.bind(this);
    var completeFunc = options['onComplete'] || dummyFunc;

    var options = options || {};
    options['parameters'] = params;
    options['onComplete'] = function(transport) {
      transport.responseText.evalScripts(); 
      resizeFunc();
      completeFunc(transport); 
    };

    new Ajax.Request(url, options);
  },

  /**
   * Save dialog contents and closes the dialog immediately.
   */
  saveAndClose: function(url, form, extraParams) {
    this.save(url, form, extraParams, { onComplete: this.close.bind(this) });
    this.close();
  },

  /**
   * Close / cancel dialog.
   */
  close: function() {
    ATK.Dialog.stack.pop();
    if (!this.window) return;
    this.window.hide();    
    this.window = null;
  },

  /**
   * Update dialog contents.
   */
  update: function(content) {
    this.window.content.update(content);
    this.resize();
  },

  /**
   * Update dialog contents with the results of the given URL.
   */
  ajaxUpdate: function(url) {
    var options = {};   
    options['evalScripts'] = true;
    options['onSuccess'] = this.resize.bind(this);
    if (this.options.serializeForm) {
      options['parameters'] = this.serializeForm();
    }
	
    new Ajax.Updater(this.window.content, url, options);
  },

  /**
   * Reload dialog contents.
   */
  reload: function() {
	  this.ajaxUpdate(this.url);
  }
};

/**
 * Returns the active dialog.
 */
ATK.Dialog.getCurrent = function() {
  return ATK.Dialog.stack[ATK.Dialog.stack.length - 1];
}