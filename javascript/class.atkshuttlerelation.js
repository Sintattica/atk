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

function shuttle_move(id1, id2)
{
  var el1 = document.getElementById(id1);
  var el2 = document.getElementById(id2);
  var options = el1.options;

  for(var i=0; i<options.length; i++)
  {
    if (options[i].selected)
    {
      el2.options[el2.options.length] = (options[i]);
      i--;
    }
  }
}

function shuttle_moveall(id1, id2)
{
  var el1 = document.getElementById(id1);
  var el2 = document.getElementById(id2);
  var options = el1.options;

  for(var i=0; i<options.length; i++)
  {
    el2.options[el2.options.length] = (options[i]);
    i--;
  }
}

