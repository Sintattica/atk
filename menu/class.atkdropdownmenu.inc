<?php

  class atkDropdownMenu extends atkmenuinterface
  {
    var $height;

    function atkDropdownMenu() 
    {
      $this->height = "52";
    }

    function render()
    {
      include_once(atkconfig("menu_layout").".inc");
    }

    function getHeight()
    {
      return $this->height;
    }	

    function getPosition()
    {
      return MENU_TOP;
    }

    function getScrollable()
    {
      return MENU_UNSCROLLABLE;
    }

		function getMultilevel()
		{
			return MENU_MULTILEVEL;
		}
  }

?>