if (!window.ATK) {
  var ATK = {};
}

ATK.DataGrid = {
  grids: $H(),

  /**
   * Registers a data grid.
   */
  register: function(name, formName, baseUrl, embedded) {
    this.grids[name] = { name: name, formName: formName, baseUrl: baseUrl, embedded: embedded, locked: false };
  },
  
  /**
   * Returns the information for the grid with the given name.
   */
  get: function(name) {
    return this.grids[name];
  },
  
  /**
   * Returns the container for the grid with the given name.
   */
  getContainer: function(name) {
    return $(name + '_container');
  },
  
  /**
   * Returns the grid form elements.
   */
  getElements: function(name) {
   return $A(this.getContainer(name).getElementsByTagName('*')).inject([],
      function(elements, child) {
        if (Form.Element.Serializers[child.tagName.toLowerCase()])
          elements.push(Element.extend(child));
        return elements;
      }
    );  
  },

  /**
   * Updates/refreshes the data grid with the given name. 
   *
   * All current parameter values will be applied, except the ones overriden 
   * by the overrides.
   */
  update: function(name, plainOverrides, jsOverrides, jsCallback) {
    var grid = this.get(name);
    
    // prevent multiple updates to the same grid at once
    if (grid.locked) return;
    grid.locked = true;
    
    // show that grid is updating
    this.getContainer(name).setOpacity(0.5);
  
    // overrides
    var overrides = Object.extend(plainOverrides, jsOverrides);   
    if (jsCallback != null) {
      overrides = Object.extend(overrides, jsCallback(name));
    }
    
    // convert overrides to query components
    var queryComponents = [];     
    $H(overrides).each(function(item) {
      var key = 'atkdg_AE_' + grid.name + '_AE_' + item.key;    
      var queryComponent = encodeURIComponent(key) + '=' + encodeURIComponent(item.value);
      queryComponents.push(queryComponent);
    });    
    
    // if embedded we also serialize the edit data form fields
    if (grid.embedded) {
      var elements = Form.getElements(grid.formName);
      for (var i = 0; i < elements.length; i++) {
        if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
          var queryComponent = Form.Element.serialize(elements[i]);
          if (queryComponent)
            queryComponents.push(queryComponent);
        }
      }
    }      

    var params = queryComponents.join('&');   
    var options = { parameters: params, evalScripts: true, onComplete: this.updateCompleted.bind(this, name) };
    new Ajax.Updater(name + '_container', grid.baseUrl, options);
  },
  
  /**
   * After update of the grid has successfully completed.
   */
  updateCompleted: function(name) {
    this.getContainer(name).setOpacity(1.0);
    this.get(name).locked = false;    
  },
  
  /**
   * Extracts fields from the datagrid form with the given name prefix
   * and returns them as overrides for use in the update method.
   */  
  extractOverrides: function(name, prefix) {
    var overrides = {};
    
    var elements = this.getElements(name);
    elements.each(function(el) {
      if (el.name.substring(0, prefix.length) == prefix) {
        overrides[el.name] = $F(el);
      }
    });
    
    return overrides;  
  },  
  
  /**
   * Extracts the search fields from the datagrid form and returns them
   * as overrides for use in the update method.
   */
  extractSearchOverrides: function(name) {
    return ATK.DataGrid.extractOverrides(name, 'atksearch');
  },
  
  /**
   * Extracts the extended sort fields from the datagrid form and returns them
   * as overrides for use in the update method.
   */  
  extractExtendendSortOverrides: function(name) {
    return ATK.DataGrid.extractOverrides(name, 'atkcolcmd');
  }
}