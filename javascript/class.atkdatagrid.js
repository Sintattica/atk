if (!window.ATK) {
  var ATK = {};
}

ATK.DataGrid = {
  grids: $H(),

  /**
   * Registers a data grid.
   */
  register: function(name, formName, baseUrl) {
    this.grids[name] = { name: name, formName: formName, baseUrl: baseUrl };
  },
  
  /**
   * Returns the information for the grid with the given name.
   */
  get: function(name) {
    return this.grids[name];
  },

  /**
   * Updates/refreshes the data grid with the given name. 
   *
   * All current parameter values will be applied, except the ones override 
   * by the overrides.
   */
  update: function(name, overrides) {
    var grid = this.get(name);
    console.debug(grid);

    var queryComponents = [];  
    var elements = Form.getElements(grid.formName);
    elements.each(function(el) {
      if (el.name && el.name.substring(0, name.length + 1) == name + '_') {
        var key = el.name.substring(name.length + 1)
        var value = $F(el);
        var queryComponent = encodeURIComponent(key) + '=' + encodeURIComponent(value);
        queryComponents.push(queryComponent);
      }
    });
    
    $H(overrides).each(function(item) {
      var queryComponent = encodeURIComponent(item.key) + '=' + encodeURIComponent(item.value);
      queryComponents.push(queryComponent);
    });

    var params = queryComponents.join('&');
    var options = { parameters: params, evalScripts: true };
    new Ajax.Updater(name + '_container', grid.baseUrl, options);
  }
}