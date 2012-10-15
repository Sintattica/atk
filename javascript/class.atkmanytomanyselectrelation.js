if (!window.ATK) {
  var ATK = {};
}

ATK.ManyToManySelectRelation = {
  /**
   * Delete.
   */
  deleteItem: function(el) {
    var li = $(el).up('li');
    li.parentNode.removeChild(li);
  },
  
  /**
   * Add.
   */
  add: function(el, url, makeSortable) {
    var params = { selector: $F(el) };
    var li = $(el).up('li');
    
  new Ajax.Request(
          url, {
            parameters: params,
            onSuccess: function(response) {
                li.insert({before: response.responseText});
                response.responseText.evalScripts();
                if(makeSortable)
                {
                    ATK.ManyToManySelectRelation.makeItemsSortable(li.up('ul'));
                }
            }
    });
    
    if (el.type == 'select-one')
    {
      el.selectedIndex = 0;  
    }
    else
    {
      $(el.name + '_search').value = '';
      el.value = '';      
    }
  },
  makeItemsSortable: function(id) {
    Sortable.create($(id), { constraint: 'vertical', only: 'atkmanytomanyselectrelation-selected'});
  }
}
