<?php

  class atkOutlookMenu extends atkmenuinterface
  {
    function render()
    {
      include_once(atkconfig("menu_layout").".inc");
    }

    function getPosition()
    {
      return MENU_LEFT;
    }

    function getScrollable()
    {
      return MENU_UNSCROLLABLE;
    }
		
		function getMultilevel()
		{
			return MENU_NOMULTILEVEL;
		}
  }

?>