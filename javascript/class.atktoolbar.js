  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage javascript
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

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
   alert("Selecteer eerst het stuk tekst dat u wilt bewerken (werkt alleen in Internet Explorer)");
   return;
  }
 
  selection = document.selection.createRange();
  current   = selection.text;
 
  if (current == '')
  {
   alert("Selecteer eerst het stuk tekst dat u wilt bewerken (werkt alleen in Internet Explorer)");
   // nothing selected
   return;
  }
    
  NewWindow(url,title,400,400,'yes');
  
}

