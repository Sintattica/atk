if (!window.ATK) {
  var ATK = {};
}

ATK.Dialog = Class.create();
ATK.Dialog.stack = [];
ATK.Dialog.prototype = {
  /**
   * Constructor.
   */
  initialize: function(title, url, params, theme, options, windowOptions) {
    this.title = title;
    this.url = url;
    this.params = params || {};
    this.theme = theme || 'alphacube';
    this.options = options || {};
    this.windowOptions = windowOptions || {};
  },
  
  /**
   * Calculate exact scroll dimensions, better than window.js own
   * getScrollDimensions because it takes padding etc. into account.
   */
  getScrollDimensions: function(element) {
    var els = element.style;
    var originalVisibility = els.visibility;
    var originalPosition = els.position;
    var originalDisplay = els.display;
    els.visibility = 'hidden';
    els.position = 'absolute';
    els.display = 'block';
    var originalWidth = element.scrollWidth;
    var originalHeight = element.scrollHeight;
    els.display = originalDisplay;
    els.position = originalPosition;
    els.visibility = originalVisibility;

    return { width: originalWidth, height: originalHeight };  
  },

  /**
   * Resize the dialog to the given dimensions.
   *
   * If no width and or height is given the dialog tries to determine the optimal
   * dimensions by itself.
   */
  resize: function(width, height) {
    if (!this.window) return;

    if (width && height)
      this.window.content.setStyle({ width: width + 'px', height: height + 'px' });    
    else if (width)
      this.window.content.setStyle({ width: width + 'px', height: 'auto' });
    else if (height)
      this.window.content.setStyle({ width: 'auto', height: height + 'px' });
    else
      this.window.content.setStyle({ width: 'auto', height: 'auto' });      
      
    var dimensions = this.getScrollDimensions(this.window.content);
    this.window.setSize(width || dimensions.width, height || dimensions.height, true);
    this.window.center({ auto: false });
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
    
    this.resize(this.options.width, this.options.height);

    this.window.content.setStyle({ visibility: '' });
    this.window.activate();      
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
      resizable: false
    }

    this.window = new UI.Window(windowOptions);
    this.window.setZIndex(1000);
    this.window.header.setStyle({ paddingRight: '0px' });
    this.window.header.update(this.title.escapeHTML());
    
    var params = 
      $H(this.params).toQueryString() + '&' +
      (this.options.serializeForm ? this.serializeForm() : '');

    var updaterOptions = {
      evalScripts: true,
      parameters: params,
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
    var resizeFunc = this.resize.bind(this, this.options.width, this.options.height);
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
    this.window.destroy(); 
    if (Prototype.Browser.IE) {
      $(document.body).setStyle({ overflow: 'hidden' });
    }
    this.window = null;
  },

  /**
   * Update dialog contents.
   */
  update: function(content) {
    this.window.content.update(content);
    this.resize(this.options.width, this.options.height);
  },

  /**
   * Update dialog contents with the results of the given URL.
   */
  ajaxUpdate: function(url, params) {
    var params = 
      $H(params || {}).toQueryString() + '&' +
      (this.options.serializeForm ? this.serializeForm() : '');  
  
    var options = {};   
    options['evalScripts'] = true;
    options['onComplete'] = this.resize.bind(this, this.options.width, this.options.height);
    options['parameters'] = params;
	
    new Ajax.Updater(this.window.content, url, options);
  },

  /**
   * Reload dialog contents.
   */
  reload: function() {
	  this.ajaxUpdate(this.url, this.params);
  }
};

/**
 * Returns the active dialog.
 */
ATK.Dialog.getCurrent = function() {
  return ATK.Dialog.stack[ATK.Dialog.stack.length - 1];
}