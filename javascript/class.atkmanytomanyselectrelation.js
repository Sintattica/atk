if (!window.ATK) {
  var ATK = {};
}

ATK.ManyToManySelectRelation = {
  /**
   * Delete.
   */
  delete: function(el) {
    var li = $(el).up('li');
    li.parentNode.removeChild(li);
  },
  
  /**
   * Add.
   */
  add: function(el, url) {
    var params = { selector: $F(el) };
    var li = $(el).up('li');
    new Ajax.Updater(li, url, { parameters: params, insertion: Insertion.Before });
    $(el.name + '_search').value = '';
    el.value = '';      
  }
}