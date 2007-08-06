if (!window.ATK) {
  var ATK = {};
}

ATK.Dialog = Class.create();
ATK.Dialog.stack = [];
ATK.Dialog.prototype = {
  /**
   * Constructor.
   */
  initialize: function(title, url, theme, options) {
    this.title = title;
    this.url = url;
    this.theme = theme || 'alphacube';
    this.options = this.options || {};
 },

  /**
   * Eval JavaScript in response.
   */
  evalResponse: function(transport) {
    setTimeout(function() { transport.responseText.evalScripts() }, 10);
  },

  /**
   * Used internally.
   */
  onShow: function(transport) {
    this.evalResponse(transport);

    if (this.options.width || this.options.height) {
      this.delayedResize();
    }
  },

  /**
   * Delayed auto-resize. Sometimes needed because the content is not always
   * fully updated yet (you don't always know how long it takes to update the DOM).
   */
  delayedResize: function() {
    setTimeout(this.resize.bind(this), 100);
  },

  /**
   * Auto-resize the dialog.
   */
  resize: function() {
    var element = $('modal_dialog_message');

    var d = Element.getDimensions(element);
    var p = Position.cumulativeOffset(element);

    var window = Windows.getWindow(Dialog.dialogId);
    window.setSize(d.width, d.height);
    window.setLocation(p[1] - window.heightN, p[0] - window.widthW);
  },

  /**
   * Show dialog.
   */
  show: function() {
    ATK.Dialog.stack.push(this);

    var windowParameters = { className: this.theme, title: this.title };
    if (this.options.width)
      windowParameters['width'] = this.options.width;
    if (this.options.height)
      windowParameters['height'] = this.options.height;

    Dialog.info(
      { url: this.url, options: { onSuccess: this.onShow.bind(this) } },
      { windowParameters: windowParameters }
    );
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

    var evalFunc = this.evalResponse.bind(this);
    var successFunc = options['onSuccess'] || function() { };

    var options = options || {};
    options['parameters'] = params;
    options['onSuccess'] = function(transport) { evalFunc(transport); successFunc(transport); };

    new Ajax.Request(url, options);
  },

  /**
   * Save dialog contents and closes the dialog immediately.
   */
  saveAndClose: function(url, form, extraParams) {
    this.save(url, form, extraParams, { onSuccess: this.close.bind(this) });
    this.close();
  },

  /**
   * Close / cancel dialog.
   */
  close: function() {
    ATK.Dialog.stack.pop();
    Dialog.closeInfo();
  },

  /**
   * Update dialog contents.
   */
  update: function(content) {
    var element = $('modal_dialog_message');
    element.update(content);
    this.resize();
  }
};

/**
 * Returns the active dialog.
 */
ATK.Dialog.getCurrent = function() {
  return ATK.Dialog.stack[ATK.Dialog.stack.length - 1];
}