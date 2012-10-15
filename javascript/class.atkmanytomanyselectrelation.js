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
  add: function(el, url) {
    var params = { selector: $F(el) };
    var li = $(el).up('li');
    new Ajax.Updater(li, url, { parameters: params, insertion: Insertion.Before, evalScripts: true });
    
    if (el.type == 'select-one')
    {
      el.selectedIndex = 0;  
    }
    else
    {
      $(el.name + '_search').value = '';
      el.value = '';      
    }
  }
}