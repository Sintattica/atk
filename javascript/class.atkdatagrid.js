if (!window.ATK) {
  var ATK = {};
}

ATK.DataGrid = {
  grids: $H(),

  /**
   * Registers a data grid.
   */
  register: function(name, baseUrl, embedded) {
    this.grids[name] = { name: name, baseUrl: baseUrl, embedded: embedded, locked: false };
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
   * Returns the form for the grid with the given name.<b> 
   */
  getForm: function(name) {
    return this.getContainer(name).up('form');
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
      var elements = Form.getElements(this.getForm(grid.name));
      for (var i = 0; i < elements.length; i++) {
        if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
          var queryComponent = Form.Element.serialize(elements[i]);
          if (queryComponent)
            queryComponents.push(queryComponent);
        }
      }
    }      
    
    queryComponents.push('atkdatagrid=' + encodeURIComponent(name));

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
   * Extracts fields from the datagrid form with the given needle in 
   * the name and returns them as overrides for use in the update method.
   *
   * It would be better to be able to check for a certain prefix, 
   * unfortunately not all atksearch* / atkcolcmd fields necessarily
   * have the needle at the start of the name. So for backwards 
   * compatibility we search the entire string for the needle. Luckily
   * the strings we are searching for are pretty unique within a form.
   */  
  extractOverrides: function(name, needle) {
    var overrides = {};
    
    var elements = this.getElements(name);
    elements.each(function(el) {
      if (el.name.indexOf(needle) >= 0) {
        overrides[el.name] = $F(el);
      }
    });
    
    console.debug(overrides);
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
  extractExtendedSortOverrides: function(name) {
    return ATK.DataGrid.extractOverrides(name, 'atkcolcmd');
  }
}