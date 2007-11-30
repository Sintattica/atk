if (!window.ATK) {
  var ATK = {};
}

ATK.DataGrid = {
  grids: $H(),

  /**
   * Registers a data grid.
   */
  register: function(name, formName, baseUrl, embedded) {
    this.grids[name] = { name: name, formName: formName, baseUrl: baseUrl, embedded: embedded };
  },
  
  /**
   * Returns the information for the grid with the given name.
   */
  get: function(name) {
    return this.grids[name];
  },
  
  /**
   * Returns the grid form elements.
   */
  getElements: function(name) {
   return $A($(name + '_container').getElementsByTagName('*')).inject([],
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
  update: function(name, plainOverrides, jsOverrides) {
    var overrides = Object.extend(plainOverrides, jsOverrides);
    var grid = this.get(name);

    var queryComponents = [];  
    
    // serialize the datagrid fields
    var elements = this.getElements(name);
    elements.each(function(el) {
      if (!el.disabled) {
        var key = 'atkdg_AE_' + grid.name + '_AE_' + el.name;
        var value = $F(el);
        var queryComponent = encodeURIComponent(key) + '=' + encodeURIComponent(value);
        queryComponents.push(queryComponent);
      }
    }); 
    
    // overrides     
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
    var options = { parameters: params, evalScripts: true };
    new Ajax.Updater(name + '_container', grid.baseUrl, options);
  }
}