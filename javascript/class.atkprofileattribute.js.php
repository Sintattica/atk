<?php
 /**
  *
  * Profile Attribute Javascripts
  *
  * This file contains javascript functions for the profile attribute, for selecting
  * all boxes.
  *
  * @author Ivo Jansch <ivo@ibuildings.nl>
  * @version $Revision$
  *
  * $Id$
  *
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