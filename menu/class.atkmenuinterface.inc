<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Abstract baseclass (interface) for all menu implementations. Any menu
   * implementation should extend this class and override the methods
   * defined by this interface.
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
	class atkmenuinterface
	{
	  var $m_height;

	  /**
	   * Render the menu
	   * @return String HTML fragment containing the menu.
	   */
		function render()
		{
		}

		/**
		 * Translates a menuitem with the menu_ prefix, or if not found without
		 *
		 * @param String $menuitem Menuitem to translate
		 * @param String $modname Module to which the menuitem belongs
		 * @return Translation of the given menuitem
		 */
		function getMenuTranslation($menuitem, $modname='atk')
		{
		  return atktext(array("menu_$menuitem", $menuitem), $modname);
		}

		/**
		 * Return the menu header
		 */
	  function getHeader($atkmenutop)
	  {
	  }

	  /**
	   * Return the menu footer
	   */
		function getFooter($atkmenutop)
		{
		}

		/**
		 * If the menu is displayed in the top frame of the application, this
		 * method should return the height of the frame that the menu requires.
		 *
		 * The framework calls this method to determine the frameset dimensions.
		 * @return int The required frame height.
		 */
		function getHeight()
		{
		}

		/**
		 * Retrieve the position in which the menu is displayed.
		 *
		 * The framework calls this method to determine the structure of the
		 * frameset.
		 * @return int The MENU_* frameposition.
		 */
		function getPosition()
		{
		}

		/**
		 * Retrieve the scrolling possibilities of the menu.
		 * @return int the MENU_* scroll definition
		 */
		function getScrollable()
		{
		}

		/**
		 * Determine if the menu can handle multiple levels
		 * of submenu.
		 * @return boolean True if multiple levels are supported, false if each
		 *                 menu can only have one level of submenuitems.
		 */
		function getMultilevel()
		{
		}
	}

?>