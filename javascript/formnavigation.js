// this script supplies cursor control for form field.
// we can step throught all the fields with the CTRL and an arrowkey
// for a couple of fieldtypes is also a special navigation possible with arrowkeys
//   (only when the arrowkeys aren't used within that specific field

var currentFocus;

//support for Netscape/Mozilla browsers, not fully functional
ver = navigator.appVersion; len = ver.length;
for(iln = 0; iln < len; iln++) if (ver.charAt(iln) == "(") break;
netscape = (ver.charAt(iln+1).toUpperCase() != "C");


// this function sets the current focus
// this has to be a function because of the inputField.onfocus event in function init_nav()
function registerFocus(comp)
{
  currentFocus = comp;
}

// this function gives every field an onfocus event so we know which field has the focus
// thereby is the focus set on the first not hidden field
function init_nav()
{
  document.onkeydown = keyDown;
  for (var i = 0; i < document.entryform.elements.length; i++)
  {
    inputField = document.entryform.elements[i];

    // not hidden fields bet an onfocus event
    if (document.entryform.elements[i].type != "hidden")
    {
      inputField.onfocus = function () { registerFocus(this); };
    }
  }

  // the first not hidden field gets focus
  for (var i = 0; i < document.entryform.elements.length; i++)
  {
    if (document.entryform.elements[i].type != "hidden")
    {
      document.entryform.elements[i].focus();
      currentFocus = document.entryform.elements[i];
      break;
    }
  }
}

function keyDown(DnEvents)
{ // handles a keypress
  k = (netscape) ? DnEvents.which : window.event.keyCode;

  if (k == 39 || (k == 40 && window.event.ctrlKey))
  {
    // (CTRL + arrow down) or arrow right is pressed
    for (var i = 0; i < document.entryform.elements.length; i++)
    {
      // we are looking for the number of the field element which has focus
      if (document.entryform.elements[i] == currentFocus)
      {
        // we found the focussed field element

        if ((i < document.entryform.elements.length - 1)
        && (document.entryform.elements[i].type == "select-one"
        || document.entryform.elements[i].type == "checkbox"
        || document.entryform.elements[i].type == "submit"
        || document.entryform.elements[i].type == "reset"))
        {
          // we may navigate to the next not hidden field
          while ((i + 1)<= document.entryform.elements.length && document.entryform.elements[i+1].type == "hidden")
          {
            i++;
          }

          if ((i + 1)<= document.entryform.elements.length)
          {
            document.entryform.elements[i + 1].focus();
          }
          break;
        }
      }
    }
  }

  if (k == 40)
  { // arrow down is pressed
    for (var i = 0; i < document.entryform.elements.length; i++)
    {
      // we are looking for the number of the field element which has focus
      if (document.entryform.elements[i] == currentFocus)
      {
        // we found the focussed field element

        if ((i < document.entryform.elements.length - 1)
        && (document.entryform.elements[i].type == "text"
        || document.entryform.elements[i].type == "checkbox"
        || (document.entryform.elements[i].type == "textarea" && window.event.ctrlKey)
        || document.entryform.elements[i].type == "password"
        || document.entryform.elements[i].type == "submit"
        || document.entryform.elements[i].type == "reset"))
        {
          // we navigate to the next not hidden field
          while ((i + 1)<= document.entryform.elements.length && document.entryform.elements[i+1].type == "hidden")
          {
            i++;
          }

          if ((i + 1)<= document.entryform.elements.length)
          {
            document.entryform.elements[i + 1].focus();
          }
          break;
        }

        // we must have special attention for the radio-attribute, we may only leave this attribute
        // with a CTRL + arrow down, and we have to navigate to the next not field which is not an radioattribute

        if (( i < document.entryform.elements.length - 1) && document.entryform.elements[i].type == "radio")
        {
          while (document.entryform.elements[i+1].type == "radio" && window.event.ctrlKey && (i + 1)<= document.entryform.elements.length)
          {
            i++;
          }

          if ((i + 1)<= document.entryform.elements.length)
          {
            document.entryform.elements[i + 1].focus();
          }
          break;
        }
      }
    }
  }

  if (k == 37 || (k == 38 && window.event.ctrlKey))
  { // (CTRL + arrow up) or arrow left is pressed
    for (var i = 0; i < document.entryform.elements.length; i++)
    {
      // we are looking for the number of the field element which has focus

      if (document.entryform.elements[i] == currentFocus)
      {
        // we found the focussed field element
        if ((i >= 1)
        && (document.entryform.elements[i].type == "select-one"
        || document.entryform.elements[i].type == "checkbox"
        || document.entryform.elements[i].type == "submit"
        || document.entryform.elements[i].type == "reset"))
        {
          // we navigate to the previous not-hidden field
          while ((i-1)>= 0 && document.entryform.elements[i-1].type == "hidden")
          {
            i--;
          }

          if ((i - 1)>= 0)
          {
            document.entryform.elements[i - 1].focus();
          }
          break;
        }
      }
    }
  }

  if (k == 38)
  { // arrow up is pressed
    for (var i = 0; i < document.entryform.elements.length; i++)
    {
      // we are looking for the number of the field element which has focus

      if (document.entryform.elements[i] == currentFocus)
      {
        // we found the focussed field element
        if ((i >= 1)
        && (document.entryform.elements[i].type == "text"
        || document.entryform.elements[i].type == "checkbox"
        || (document.entryform.elements[i].type == "textarea" && window.event.ctrlKey)
        || document.entryform.elements[i].type == "password"
        || document.entryform.elements[i].type == "submit"
        || document.entryform.elements[i].type == "reset"))
        {
          // we navigate to the previous not hidden field
          while ((i-1)>= 0 && document.entryform.elements[i-1].type == "hidden")
          {
            i--;
          }

          if ((i - 1)>= 0)
          {
            document.entryform.elements[i - 1].focus();
          }
          break;
        }

        // we must have special attention for the radio-attribute, we may only leave this attribute
        // with a CTRL + arrow up, and we have to navigate to the next not field which is not an radioattribute

        if (( i > 0) && document.entryform.elements[i].type == "radio")
        {
          while (document.entryform.elements[i-1].type == "radio" && window.event.ctrlKey && (i-1)>= 0)
          {
            i--;
          }

          if ((i - 1)>= 0)
          {
            document.entryform.elements[i - 1].focus();
          }
          break;
        }
      }
    }
  }
}


