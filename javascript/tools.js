// For getting objects but perserving backwards compatibility
function get_object(name)
{
  if (document.getElementById)
  {
    return document.getElementById(name);
  }
  else if (document.all)
  {
    return document.all[name];
  }
  else if (document.layers)
  {
    return document.layers[name];
  }
  return false;
}

// For toggling the display on an object
function toggleDisplay(name, obj)
{  
  if (obj.style.display=="none")
  {
    obj.style.display="";		  
  }
  else
  {
    obj.style.display="none";
  }
}