if (!window.ATK) {
  var ATK = {};
}

ATK.Attribute = {
  /**
   * Refresh the attribute input form using Ajax.
   */   
  refresh: function(field, url) {
    var elements = Form.getElements('entryform');
    var queryComponents = new Array();

    for (var i = 0; i < elements.length; i++) {
      if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
        var queryComponent = Form.Element.serialize(elements[i]);
        if (queryComponent)
          queryComponents.push(queryComponent);
      }
    }

    var params = queryComponents.join('&');          

    new Ajax.Updater(field, url, {method: 'post', parameters: params, evalScripts: true, asynchronous: true });        
  }
};