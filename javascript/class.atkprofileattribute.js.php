<?php
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
  
?>

function profile_checkAll(fieldname)
{    
  with (document.entryform)
  {
    for(i=0; i<elements.length; i++)
    {
      if (elements[i].name.substr(0,fieldname.length)==fieldname)
      {
        elements[i].checked = true;
      }
    }
  }
}

function profile_checkNone(fieldname)
{    
  with (document.entryform)
  {
    for(i=0; i<elements.length; i++)
    {
      if (elements[i].name.substr(0,fieldname.length)==fieldname)
      {
        elements[i].checked = false;
      }
    }
  }
}

function profile_checkInvert(fieldname)
{    
  with (document.entryform)
  {
    for(i=0; i<elements.length; i++)
    {
      if (elements[i].name.substr(0,fieldname.length)==fieldname)
      {
        elements[i].checked = !elements[i].checked;
      }
    }
  }
}