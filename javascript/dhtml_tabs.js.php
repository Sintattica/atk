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

/**
 * Sets the current tab
 */
function showTab(tab)
{
	// If we are called without a name, we check if the parent has a stored tab for our page
	// If so, then we go there, else we go to the first tab (most of the time the 'default' tab)
	if (!tab)
	{
	  tab = getCurrentTab();
	  if (tab)
	  {
      // However if for some reason this tab does not exist, we switch to the default tab
      if (!document.getElementById('tab_'+tab)) tab = tabs[0];
    }
    else
    {
      tab = tabs[0];
    }
  }

  // Then we store what tab we are going to visit in the parent
	setCurrentTab(tab);

  showTr(tab);

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
	
	makeFCKEditable();
	
	// make tabs visible (to avoid reload quirks, they load invisible from the html
	wrapper = document.getElementById('tabtable');
	if (wrapper)
	{
	  wrapper.style.display='';
	}
}


/**
 * Because the FCK editor does not always agree with 
 * tabbing and no longer becomes editable if you switch 
 */
function makeFCKEditable()
{
  iframes = document.getElementsByTagName("iframe");
	for (i = 0; i < iframes.length; i++)
	{
	  obj = frames[iframes[i].id];
	  if (obj && obj.FCK && obj.FCK.MakeEditable) obj.FCK.MakeEditable();
	}
}

function getCurrentTab()
{
  return getTab(getCurrentNodetype(), getCurrentSelector());
}

function getTab(nodetype, selector)
{
  _initTabArray(nodetype, selector);
  return parent.document.tab[nodetype][selector];
}

function setCurrentTab(value)
{
  return setTab(getCurrentNodetype(), getCurrentSelector(), value);
}

function setTab(nodetype, selector, value)
{
  _initTabArray(nodetype, selector);
  parent.document.tab[nodetype][selector] = value;
}

/**
 * Makes sure we don't get any nasty JS errors by making sure
 * the arrays we use are always set before using them.
 */
function _initTabArray(nodetype, selector)
{
	if (!parent.document.tab) parent.document.tab=Array();
	if (!parent.document.tab[nodetype]) parent.document.tab[nodetype]=Array();
}