
if (!window.ATK) {
  var ATK = {};
}

ATK.AttributeEditHandler = {
  refreshvalues: function(url) {
    var list = $('dialogform').elements['atkselector[]']; 

    var realList = [];
    if (typeof(list.length) == 'undefined') {
      realList.push($F(list));
    } else {
      for (var i = 0; i < list.length; i++) {
        realList[i] = $F(list[i]);
      }
    }
  
    var attr = $F('attributename');
    var params = { attributename: attr, 'atkselector[]': realList };
    new Ajax.Request(url, { method: 'post', parameters: params, onComplete: function(transport) { transport.responseText.evalScripts(); }});
  }
}