function modifySelection(pre, post)
{
  var selection;
  var current;
 
  if (!document.selection)
  {
   return;
  }
 
  selection = document.selection.createRange();
  current   = selection.text;
 
  if (current == '')
  {
   return;
  }
 
  selection.text = pre + current + post;
  selection.parentElement().focus();
}

function popupSelection(url,title)
{
  var selection;
  var current;
 
  if (!document.selection)
  {   
   return;
  }
 
  selection = document.selection.createRange();
  current   = selection.text;
 
  if (current == '')
  {
   // nothing selected
   return;
  }
    
  NewWindow(url,title,600,300,'yes');
  
}

