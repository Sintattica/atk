<?php

  class atkPlainMenu extends atkmenuinterface
  {
    var $height;

    function atkPlainMenu() 
    {
      $this->height = "50";
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
      switch(atkconfig("menu_pos", "left"))
      {
        case "left": return MENU_LEFT;
        case "right": return MENU_RIGHT;
        case "top": return MENU_TOP;
        case "bottom": return MENU_BOTTOM;
      }
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