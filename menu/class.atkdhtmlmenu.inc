<?php

  class atkDhtmlMenu extends atkmenuinterface
  {
    var $height;

    function atkDhtmlMenu() 
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
      return MENU_TOP;
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