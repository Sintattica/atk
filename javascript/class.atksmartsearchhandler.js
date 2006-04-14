if (!window.ATK) {
  var ATK = new Object();
}

ATK.SmartSearchHandler = {
  registry: new Array(),
  
  /**
   * Register criterium.
   */
  registerCriterium: function(id) {
    var registry = ATK.SmartSearchHandler.registry;
    registry.push(id);
  },
  
  /**
   * Unregister criterium.
   */
  unregisterCriterium: function(id) {
    var registry = ATK.SmartSearchHandler.registry;
    
    for (i = 0; i < registry.length; i++) {
      if (registry[i] == id) {
        registry.splice(i, 1);
        return;
      }
    }    
  },  
  
  /**
   * Returns the maximum criterium identifier.
   */
  getMaxCriteriumId: function() {
    var max = 0;
    var registry = ATK.SmartSearchHandler.registry;
    
    for (i = 0; i < registry.length; i++) {
      if (registry[i] > max) {
        max = registry[i];
      }
    }
    
    return max;
  },
  
  /**
   * Add criterium.
   */
  addCriterium: function(url) {
    var params = 'next_criterium_id=' + (ATK.SmartSearchHandler.getMaxCriteriumId() + 1);
    new Ajax.Updater('criteria', url, { method: 'post', parameters: params, insertion: Insertion.Bottom, evalScripts: true, asynchronous: true });        
  },
  
  /**
   * Remove criterium.
   */   
  removeCriterium: function(id) {
    ATK.SmartSearchHandler.unregisterCriterium(id);
    
    var elementId = 'criterium_' + id + '_box';
    while ($(elementId) != null) {
      Element.remove(elementId);
    }
  },
  
  /**
   * Register a criterium field listener.
   */
  registerCriteriumFieldListener: function(name, prefix, fieldEl, fieldUrl, valueEl, valueUrl, modeEl, modeUrl) {
    new Form.Element.EventObserver(name, function() {
      var elements = Form.getElements($(name).form);

      var params = elements.findAll(function(el) {
         return el.name.substring(0, prefix.length) == prefix;
      }).collect(function(el) { 
        return Form.Element.serialize(el); 
      }).join('&');

      new Ajax.Updater(fieldEl, fieldUrl, { method: 'post', parameters: params, evalScripts: true, asynchronous: true });              
      new Ajax.Updater(valueEl, valueUrl, { method: 'post', parameters: params, evalScripts: true, asynchronous: true });                    
      new Ajax.Updater(modeEl,  modeUrl,  { method: 'post', parameters: params, evalScripts: true, asynchronous: true });                          
    });
  }
};