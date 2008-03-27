function shuttle_selectAll(id)
{
  var el = document.getElementById(id);
  var options = el.options;

  for(var i=0; i<options.length; i++)
  {
    options[i].selected = true;
  }
  return true;
}

function shuttle_move(id1, id2, action, el)
{
  var el1 = document.getElementById(id1);
  var el2 = document.getElementById(id2);

  var values = [];
  var unset = [];
  
  el2.selectedIndex = -1;    
  
  for (var i = 0; i < el1.options.length; i++)
  {
    if (el1.options[i].selected)
    {
      el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
      el2.options[el2.options.length - 1].selected = true;
      values.push(el1.options[i].value);
      unset.unshift(i);
    }
  }
   
  for (var i = 0; i < unset.length; i++)
  {
    el1.options[unset[i]] = null;
  }

  el1.selectedIndex = -1;  
  
  shuttle_save(values, action, el)

  // trigger on change event  
  if (document.createEventObject)
  {
    // dispatch for IE
    var evt = document.createEventObject();
    el2.fireEvent('onchange',evt)
  }
  else
  {
    // dispatch for firefox + others
    var evt = document.createEvent("HTMLEvents");
    evt.initEvent('change', true, true ); // event type,bubbling,cancelable
    el2.dispatchEvent(evt);
  }
}

function shuttle_moveall(id1, id2, action, el)
{
  var el1 = document.getElementById(id1);
  var el2 = document.getElementById(id2);

  var values = [];  
  
  for(var i=0; i<el1.options.length; i++)
  {
    el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
    values.push(el1.options[i].value);
  }

  el1.options.length=0;
  
  shuttle_save(values, action, el); 
}

function getParams(url, selectEl)
{
  var elements = Form.getElements('entryform');
  var queryComponents = new Array();

  for (var i = 0; i < elements.length; i++) {
    if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
      var queryComponent = Form.Element.serialize(elements[i]);
      if (queryComponent)
        queryComponents.push(queryComponent);
    }
  }
  
  // add non-selected options
  var selectEl = $(selectEl);
  for (var i = 0; i < selectEl.options.length; i++)
  {
    if (!selectEl.options[i].selected)
    {
      var pair = {};
      pair[selectEl.name] = selectEl.options[i].value;
      queryComponents.push(Hash.toQueryString(pair));
    }
  }

  return queryComponents.join('&');
}

function shuttle_refresh(url, selectEl, parent, side)
{
  if (side) $(parent).value = side;
  new Ajax.Request(url, { method: 'post', parameters: getParams(url, selectEl), onComplete: function(transport) { transport.responseText.evalScripts(); }});
}

function shuttle_save(values, action, el)
{
  var current = $F(el);
  current = current.length > 0 ? current.parseJSON() : [];
  
  if (action == 'add')
  {
    current = current.concat(values)
  }
  else
  {
    for (var i = 0; i < values.length; i++)
    {
      var index = current.indexOf(values[i]);
      current.splice(index, 1);
    }
  }
  
  $(el).value = current.toJSONString();
}