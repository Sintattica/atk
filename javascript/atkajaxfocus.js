if (!window.ATK) {
  var ATK = {};
}

if (!window.ATK.Ajax) {
  ATK.Ajax = {};
}

if (!window.ATK.Ajax.Focus) {
  ATK.Ajax.Focus = {
    lastForm: null,
    lastElement: null,
    
    /**
     * Handle focus event.
     */
    handleFocus: function() {
      ATK.Ajax.Focus.lastForm = this.form;
      ATK.Ajax.Focus.lastElement = this.name || this.id; 
    },
    
    /**
     * Register focus listeners.
     */
    registerListeners: function() {
      $A(document.forms).each(function(form) {
        Form.getElements(form).each(function(el) {
          if (el.type != 'hidden') {
            el.onfocus = ATK.Ajax.Focus.handleFocus; 
          }
        });
      });
    },

    /**
     * Restore focus to last focused element.
     */
    restoreFocus: function() {
      if (ATK.Ajax.Focus.lastForm && 
          ATK.Ajax.Focus.lastElement &&
          ATK.Ajax.Focus.lastForm[ATK.Ajax.Focus.lastElement]) {
        window.setTimeout(function() { ATK.Ajax.Focus.lastForm[ATK.Ajax.Focus.lastElement].focus(); }, 1);
      }
      else if ($(ATK.Ajax.Focus.lastElement)) {
         window.setTimeout(function() { $(ATK.Ajax.Focus.lastElement).focus(); }, 1);        
      }
    }
  };
};

/**
 * Overload onElementEvent function so we make sure the focus is first
 * moved to the new component before the Ajax request is made. We do this
 * by using a small timeout before calling the event handler.
 */
Abstract.EventObserver.prototype.onElementEvent = function() {
  var self = this;
  window.setTimeout(
    function() {
      var value = self.getValue();
      if (self.lastValue != value) {
        self.callback(self.element, value);
        self.lastValue = value;
      }
    }, 1
  );
};

/**
 * Register Ajax responder to restore the focus and
 * register new listeners when needed.
 */
Ajax.Responders.register({
  onComplete: function() {
    if (Ajax.activeRequestCount == 0) {
      ATK.Ajax.Focus.registerListeners();    
      ATK.Ajax.Focus.restoreFocus();
    }
  }
});

/**
 * Register focus listeners on window load.
 */
Event.observe(window, "load", ATK.Ajax.Focus.registerListeners);