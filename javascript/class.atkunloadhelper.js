if (!window.ATK) {
  var ATK = {};
}

ATK.UnloadHelper = Class.create();
ATK.UnloadHelper.prototype = {
  /**
   * Construct.
   */
  initialize: function(form, message, isChanged) {
    // Intialize instance variables.
    this.form = $(form || 'entryform');
    
    if (this.form == null) return;
    
    this.message = message || 'If you navigate away from this page, all your changes will be lost.';
    this.isChanged = isChanged || false;
    this.enabled = true;    
    
    // Listen for submit events, because then the 
    // beforeunload handler should be disabled.
    Event.observe(this.form, 'submit', this.onSubmit.bind(this));
    
    // Override atkSubmit.
    this.oldATKSubmit = atkSubmit;
    atkSubmit = this.onATKSubmit.bind(this);
    
    // Listen for before unload events so that we can stop
    // the user from leaving the current page.
    window.onbeforeunload = this.onBeforeUnload.bind(this);
  },
  
  /**
   * Is the element value changed?
   */
  isElementChanged: function(el) {
    switch (el.type) {
      case 'text':
      case 'textarea':
      case 'hidden':
        return el.value != el.defaultValue;
        
      case 'checkbox':
      case 'radio':
        return el.checked != el.defaultChecked;
        
      case 'select-one':
        for (var i = 0; i < el.options.length; i++) {
          // If this option wasn't previously selected and this is
          // the first option then there should be another option that
          // has changed state too. Else the browser has automatically
          // selected the first option because no other option was 
          // selected and we shouldn't register this as a change. This
          // isn't the case
          if (el.options[i].selected != el.options[i].defaultSelected && i > 0) {
            return true;
          }
        }
        
        return false;

      case 'select-multiple':
        return $A(el.options).find(function(option) { 
          return option.selected != option.defaultSelected; 
        }) != null;
      
      default:
        return false;
    }
  },
  
  /**
   * Is element excluded?
   */
  isElementExcluded: function(el) {
    var container = $(el).up('.atkdatagrid-container');
    return container != null;
  },
  
  /**
   * Is the form changed?
   */
  isFormChanged: function() {
    // detect if the form is changed
    var elements = this.form.getElements();
    for (i = 0; i < elements.length; i++) {
      if (this.isElementChanged(elements[i]) && !this.isElementExcluded(elements[i])) {
        return true;
      }
    }

    return false;
  },

  /**
   * Save if this form is changed or not inside the
   * form so we can use it later on to detect if
   * there were already changes before coming back to
   * this page.
   */
  setChanged: function(value) {
    var input = document.createElement('input');
    input.setAttribute('type', 'hidden');
    input.setAttribute('name', '__atkunloadhelper');
    input.setAttribute('value', value ? 1 : 0);
    this.form.appendChild(input);
  },  
  
  /**
   * Before submitting the form we disable the before unload handler.
   * If the user has explictly choosen to submit the form he/she
   * shouldn't be reminded about leaving the page.
   */
  onSubmit: function() {
    this.enabled = false;
    
    // reset the current changed value in the form
    this.setChanged(false);
    
    return true;
  },
  
  /**
   * Before submitting the form we disable the before unload handler.
   * If the user has choosen to add a nested record or something else
   * that triggers an atkSubmit he/she shouldn't be reminded about 
   * leaving the page.
   */
  onATKSubmit: function() {
    this.enabled = false;

    // store the current changed value in the form
    this.setChanged(this.isFormChanged());
    
    return this.oldATKSubmit.apply(null, arguments);
  },
  
  /**
   * Before unloading the page we check if one or more
   * fields in the form has been changed. If so we return
   * a message that is shown to the user.
   */
  onBeforeUnload: function() {
    // Internet Explorer also triggers the before unload event for Ajax requests. In that
    // case we don't need to warn the user about leaving the page.
    if (Ajax.activeRequestCount == 0 && this.enabled && (this.isChanged || this.isFormChanged())) {
      return this.message;
    }
  }
};