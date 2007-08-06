if (!window.ATK) {
  var ATK = {};
}

ATK.Attribute = {
  /**
   * Refresh the attribute input form using Ajax.
   */
  refresh: function(url, focusFirstFormEl) {
    var form = $(ATK.Dialog && ATK.Dialog.getCurrent() != null ? 'dialogform' : 'entryform'); // TODO: find a better way to detect the correct form
    if (form == null) return;
    
    var elements = Form.getElements(form);
    var queryComponents = new Array();

    for (var i = 0; i < elements.length; i++) {
      if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
        var queryComponent = Form.Element.serialize(elements[i]);
        if (queryComponent)
          queryComponents.push(queryComponent);
      }
    }

    atkErrorFields.each(function(field) {
      var queryComponent = $H({ 'atkerrorfields[]': field }).toQueryString();
      queryComponents.push(queryComponent);
    });

    var params = queryComponents.join('&');

    var func = function(transport) {
      transport.responseText.evalScripts();
      if (form == 'dialogform' && ATK.Dialog && ATK.Dialog.getCurrent() != null) {
        ATK.Dialog.getCurrent().delayedResize();
      }
    };
    if (focusFirstFormEl) {
      func = function(transport) {
        transport.responseText.evalScripts();
        try { placeFocus(); } catch (ex) {}
        if (form == 'dialogform' && ATK.Dialog && ATK.Dialog.getCurrent() != null) {
          ATK.Dialog.getCurrent().delayedResize();
        }
      };
    }

    /*
    // disable form fields that are going to be replaced
    $(field).getElementsBySelector('*').each(function(el) {
      el.style.visibility = 'hidden';
    });

    var span = document.createElement('span');
    var img = document.createElement('img');
    img.src = 'themes/achievo_modern/images/spinner.gif';
    span.appendChild(img);
    $(field).appendChild(span);
    Position.absolutize(span);
    Position.clone(field, span);
    */

    new Ajax.Request(url, { method: 'post', parameters: params, evalScripts: true, onComplete: func });
  },

  refreshDisplay: function(url) {
    var func = function(transport) { transport.responseText.evalScripts(); };
    new Ajax.Request(url, { method: 'post', evalScripts: true, onComplete: func });
  }
};