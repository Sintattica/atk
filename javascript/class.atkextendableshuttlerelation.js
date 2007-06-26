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

  var newel = el1.cloneNode(false);
  newel.options.length=0;

  for(var i=0; i<el1.options.length; i++)
  {
    if (el1.options[i].selected)
    {
      el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
      saveValue(el1.options[i].value, action, el);
    }
    else
    {
      newel.options[newel.options.length] = new Option(el1.options[i].text, el1.options[i].value);
    }
  }

  el1.options.length=0;
  for (i=0; i<newel.options.length; i++)
  {
    el1.options[el1.options.length] = new Option(newel.options[i].text, newel.options[i].value);
  }
}

function shuttle_moveall(id1, id2, action, el)
{
  var el1 = document.getElementById(id1);
  var el2 = document.getElementById(id2);

  for(var i=0; i<el1.options.length; i++)
  {
    el2.options[el2.options.length] = new Option(el1.options[i].text, el1.options[i].value);
    saveValue(el1.options[i].value, action, el);
  }

  el1.options.length=0;
}

function getParams(url, selectEl)
{
    shuttle_selectAll(selectEl);

    var elements = Form.getElements('entryform');
    var queryComponents = new Array();

    for (var i = 0; i < elements.length; i++) {
      if (elements[i].name && elements[i].name.substring(0, 3) != 'atk') {
        var queryComponent = Form.Element.serialize(elements[i]);
        if (queryComponent)
          queryComponents.push(queryComponent);
      }
    }

    return queryComponents.join('&');
}

function shuttle_refresh(url, selectEl, parent, side)
{
  if (side) $(parent).value = side;
  new Ajax.Request(url, { method: 'post', parameters: getParams(url, selectEl), onComplete: function(transport) { transport.responseText.evalScripts(); }});
}

function saveValue(value, action, el)
{
  if (action=='add')
  {
    if ($F(el))
      val = $F(el).parseJSON();
    else
      val = [];
    val[val.length] = value;
    $(el).value = val.toJSONString();
  }
  else
  {
    if ($F(el))
    {
      val = $F(el).parseJSON();
      $(el).value = val.without(value).toJSONString();
    }
  }
}