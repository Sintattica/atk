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
   
function showTab(tab)
{
	// First, get the class names of all elements
	var tags = document.getElementsByTagName("tr");
	
	// Next, check wether the parent tab array has been set
	if (!parent.document.tab)
	{
	  parent.document.tab=Array();
	}

	// If we are called without a name, we check if the parent has a stored tab for our page
	// If so, then we go there, else we go to the first tab (most of the time the 'default' tab)
	if (!tab)
	{
	  if (parent.document.tab[document.location.href])
    {
      tab = parent.document.tab[document.location.href];
    }
    else
    {
      tab = tabs[0];
    }
  }
  
  // Then we store what tab we are going to visit in the parent
	parent.document.tab[document.location.href] = tab;
	
	// Every element that does not have the current tab as class or 'alltabs'
	// is set to display: none
	for (i = 0; i < tags.length; i++)
	{
		var tabclass = tags.item(i).className;
		var id = tags.item(i).id;

		if (id.substring(0,3)=="ar_")
		{
		  if (tabclass==tab||tabclass=="alltabs")
		  {		  
  		  tags.item(i).style.display="";		  
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

	// Then when set the colors or the tabs, the active tab gets a different color
	for(j = 0; j < tabs.length; j++)
	{
		if(tabs[j] == tab)
		{
			document.getElementById('tab_'+tabs[j]).className = 'activetab';
		}
		else
		{
		  document.getElementById('tab_'+tabs[j]).className = 'passivetab';
		}
	}
}