
if (!window.ATK) {
  var ATK = {};
}

ATK.AttributeEditHandler = {
  refreshvalues: function(url) {
    var params = Form.serialize('dialogform');
    new Ajax.Request(url, { method: 'post', parameters: params, onComplete: function(transport) { transport.responseText.evalScripts(); }});
  }
}