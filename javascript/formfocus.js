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
  if (document.forms.length > 0) 
  {
    var field = document.forms[0];
    for (i = 0; i < field.length; i++) 
    {      
      if (((field.elements[i].type == "text") || (field.elements[i].type =="textarea") || (field.elements[i].type.toString().charAt(0) == "s"))) 
      {
        if (field.elements[i].id) 
        {
          obj = get_object('ar_'+field.elements[i].id);
          if (obj && obj.style.display!=='none')
          {
            document.forms[0].elements[i].focus();
            break;
          }
        }
      }
    }
  }
}
