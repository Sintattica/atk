if (!window.ATK) {
  var ATK = {};
}

ATK.Form = {
  /**
   * Submit an ATK form using an Ajax request.
   * 
   * @param url           Ajax request URL
   * @param ignoreATKVars ignore ATK form vars? defaults to true
   * @param onComplete    on complete callback, if none given the default behaviour 
   *                      is to eval scripts in the response text
   */
  ajaxSubmit: function(url, ignoreATKVars, onComplete) {
    var form = $(ATK.Dialog && ATK.Dialog.getCurrent() != null ? 'dialogform' : 'entryform');
    if (form == null) {
      return;
    }
    
    if (typeof(ignoreATKVars) == 'undefined') {
      ignoreATKVars = true;
    }
      
    var elements = Form.getElements(form);
    var queryComponents = new Array();

    for (var i = 0; i < elements.length; i++) {
      if (!ignoreATKVars || elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
        var queryComponent = Form.Element.serialize(elements[i]);
        if (queryComponent)
          queryComponents.push(queryComponent);
      }
    }

    var params = queryComponents.join('&');

    if (onComplete == null) {
      onComplete = function(transport) {
        transport.responseText.evalScripts();
      }
    }

    new Ajax.Request(url, { method: 'post', parameters: params, onComplete: onComplete });
  }
};