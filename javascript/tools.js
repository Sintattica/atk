/**
 * For getting objects but perserving backwards compatibility
 */
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

/**
 * Toggles the display on an object
 */
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

/**
 * Transforms the first character of string to uppercase
 * e.g. kittie => Kittie
 */
function ucfirst(stringtt)
{
  return stringtt.charAt(0).toUpperCase()+stringtt.substring(1,stringtt.length)
}

/**
 * Replace an occurrence of a string
 */
function str_replace(haystack,needle,replace,casesensitive)
{
	if(casesensitive) return(haystack.split(needle)).join(replace);

	needle=needle.toLowerCase();

	var replaced="";
	var needleindex=haystack.toLowerCase().indexOf(needle);
	while(needleindex>-1)
	{
		replaced+=haystack.substring(0,needleindex)+replace;
		haystack=haystack.substring(needleindex+needle.length);
		needleindex=haystack.toLowerCase().indexOf(find);
	}
	return(replaced+haystack);
}

/**
 * Gets the atkselector of the current node
 */
function getCurrentSelector()
{
  var selectorobj = get_object("atkselector");

  if (selectorobj.value)
  {
    var selector = selectorobj.value;
  }
  else if (selectorobj.innerHTML)
  {
    var selector = selectorobj.innerHTML;
  }
  return selector;
}


/**
 * Gets the atknodetype of the current node
 */
function getCurrentNodetype()
{
  var nodetypeobj  = get_object("atknodetype");

  // IE works with .value, while the Gecko engine uses .innerHTML
  if (nodetypeobj.value)
  {
    var nodetype = nodetypeobj.value;
  }
  else if (nodetypeobj.innerHTML)
  {
    var nodetype = nodetypeobj.innerHTML;
  }
  return nodetype;
}

function reloadapp()
{
  if (!top.reloaded) top.reloaded = new Array();
  for (i=0;i<top.frames.length;i++)
  {
    if (!top.reloaded[i])
    {
      top.frames[i].location.reload();
      top.reloaded[i] = 1;
    }
  }
}

function showTr(tab)
{
	if (tab == null) { tab = "default" };

  // First, get the class names of all elements
	var tags = document.getElementsByTagName("tr");

	// First check if there are any attributes to show for this tab, sometimes
	// in edit mode other tabs exist then in view mode.
	var found = false;
	for (i = 0; i < tags.length; i++)
	{
		var tabclass = tags.item(i).className;
		var id = tags.item(i).id;

		if (id.substring(0,3)=="ar_" && (tabclass.indexOf(tab) != -1 || tabclass=="alltabs"))
		{
		  found = true;
		}
	}

	// tab not found, change nothing!
	if (!found)
	{
	  return;
	}

	// Every element that does not have the current tab as class or 'alltabs'
	// is set to display: none
	for (i = 0; i < tags.length; i++)
	{
		var tabclass = tags.item(i).className;
		var id = tags.item(i).id;

		if (id.substring(0,3)=="ar_")
		{
		  if (tabclass.indexOf(tab) != -1 || tabclass=="alltabs")
		  {
  		  tags.item(i).style.display="";
  		  found = true;
		  }
		  else
		  {
  		  tags.item(i).style.display="none";
		  }
		}
		else
		{
		  // Don't touch any element that is not an attribute row
		}
	}
}