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
   
function placeFocus()
{
  if (document.forms.length == 0) return;

  var fields = document.forms[0].elements;
  for (i = 0; i < fields.length; i++) 
  { 
    var field = fields[i];
    var type = field.type.toLowerCase();     
    if (type == "text" || type == "textarea" || type.toString().charAt(0) == "s") 
    {
      var found = false;
      
      var node = field.parentNode;
      while (node != null)
      {
        if (node.nodeName.toLowerCase() == 'tr')
        {
          found = node.id != null && node.id.substring(0, 3) == 'ar_' && node.style.display != 'none';
          if(found) field.focus();
          break;
        }
        
        node = node.parentNode;
      }
      
      if (found) break;
    }
  }
}
