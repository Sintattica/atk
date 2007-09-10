<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage modules
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * The atkModule abstract base class.
   *
   * All modules in an ATK application should derive from this class, and
   * override the methods of this abstract class as they see fit.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage modules
   * @abstract
   */
  class atkModule
  {
    var $m_name;

    /**
     * Constructor. The module needs to register it's nodes
     * overhere, create its menuitems etc.
     * @param String $name The name of the module.
     */
    function atkModule($name="")
    {
      $this->m_name = $name;
    }

    /**
     * Register nodes with their supported actions. Can be used
     * for security etc.
     */
    function getNodes()
    {
    }

    /**
     * Returns an array with filenames of attributes that need to be included
     * in order to let this module work properly.
     * @return array with attribute filenames
     */
    function getAttributes()
    {
    }

    /**
     * This method returns an array with menu items that need to be available
     * in the main ATK menu. This function returns the array created with
     * the menuitem() method, and does not have to be extended!
     * @return array with menu items for this module
     */
    function getMenuItems()
    {
    }

    /**
     * Create a new menu item
     *
     * Both main menu items, separators, submenus or submenu items can be
     * created, depending on the parameters passed.
     * 
     * Function that adds a menuitem to the menu. This function can be called
     * from inside the getMenuItems() function, to register a menuitem.
     *
     * @param String $name The menuitem name. The name that is displayed in the
     *                     userinterface can be influenced by putting
     *                     "menu_something" in the language files, where 'something'
     *                     is equal to the $name parameter.
     *                     If "-" is specified as name, the item is a separator.
     *                     In this case, the $url parameter should be empty.
     * @param String $url The url to load in the main application area when the
     *                    menuitem is clicked. If set to "", the menu is treated
     *                    as a submenu (or a separator if $name equals "-").
     *                    The dispatch_url() method is a useful function to
     *                    pass as this parameter.
     * @param String $parent The parent menu. If omitted or set to "main", the
     *                       item is added to the main menu.
     * @param mixed $enable This parameter supports the following options:
     *                      1: menuitem is always enabled
     *                      0: menuitem is always disabled
     *                         (this is useful when you want to use a function
     *                         call to determine when a menuitem should be
     *                         enabled. If the function returns 1 or 0, it can
     *                         directly be passed to this method in the $enable
     *                         parameter.
     *                      array: when an array is passed, it should have the
     *                             following format:
     *                             array("node","action","node","action",...)
     *                             When an array is passed, the menu checks user
     *                             privileges. If the user has any of the
     *                             node/action privileges, the menuitem is
     *                             enabled. Otherwise, it's disabled.
     * @param int $order The order in which the menuitem appears. If omitted,
     *                   the items appear in the order in which they are added
     *                   to the menu, with steps of 100. So, if you have a menu
     *                   with default ordering and you want to place a new
     *                   menuitem at the third position, pass 250 for $order. 
     */
    function menuitem($name="", $url="", $parent="main", $enable=1, $order=0)
    {
      /* call basic menuitem */
      menuitem($name, $url, $parent, $enable, $order, $this->m_name);
    }

    /**
     * This method can be used to return an array similar to the menu array
     * but with links to (a) preference(s) page(s) for this module. The items
     * that will be returned have to be added with the preferencelink() method.
     * @return array with preference links for this module
     */
    function getPreferenceLinks()
    {
    }

    /**
     * This method is similar to the getPreferenceLinks() method but instead
     * will return links to (an) admin page(s) for this module. The array which
     * will be returned have to be created with the adminlink() method.
     * @return array with admin links for thid module
     */
    function getAdminLinks()
    {
    }

    /**
     * Returns the node overloader if it exists. Else it
     * will just return the module/node name of the given node.
     * @param $node module/node string
     * @return (overloader) module/node string
     */
    function &nodeOverloader($node)
    {
      global $g_overloaders;

      /* overloader check */
      if (!empty($g_overloaders[$node]))
      {
        atkdebug("Using overloader '".$g_overloaders[$node]."' for class '".$node."'");
        return newNode($g_overloaders[$node], FALSE);
      }

      /* no overloader */
      else
      {
        $tmp = NULL;
        return $tmp;
      }
    }
    
    /**
     * Returns the node file for the given node.
     *
     * @param string $node the node type
     * @return string node filename
     */
    function getNodeFile($node)
    {
      return nodeFile($node);
    }
    
    /**
     * Returns the fixture path for the given fixture.
     * 
     * @param string $fixture <module.fixture> string
     * @return string path to fixture without extension
     */
    function getFixturePath($fixture)
    {
      $module = getNodeModule($fixture);
      $fixture = getNodeType($fixture);
      $path = moduleDir($module).'testcases/fixtures/'.$fixture;
      return $path;
    }

    /**
     * Construct a new node. A module can override this method for it's own nodes.
     * @param $node the node type
     * @return new node object
     */
    function &newNode($node)
    {
      /* include the base file */
      atkimport("atk.atknode");

      $corporate_base = atkconfig("corporate_node_base");
      if ($corporate_base!="")
      {
        atkimport($corporate_base);
      }

      /* check for file */
      $file = $this->getNodeFile($node);
      if (!file_exists($file))
      {
        atkimport("atk.utils.atkclassloader");
        $res = atkClassLoader::invokeFromString(atkconfig("missing_class_handler"), array("node"=>$node,"module"=>$this->m_name));
        if ($res!==false)
        {
          return $res;
        }
        else
        {
          atkerror("Cannot create node, because a required file ($file) does not exist!", "critical");
          return NULL;
        }
      }

      /* include file */
      include_once($file);

      /* module */
      $module = getNodeModule($node);

      // set the current module scope, this will be retrieved in atkNode
      // to set it's $this->m_module instance variable in an early stage
      atkModule::setModuleScope($module);  
      
      /* now that we have included the node source file, we check
       * for overloaders (because overloaders might need the included file!)
       */
      $overloader = &$this->nodeOverloader($node);
      if ($overloader != NULL)
      {
        $overloader->m_module = $module;
        atkModule::resetModuleScope(); 
        return $overloader;
      }

      /* initialize node and return */
      $type = getNodeType($node);
      $node =& new $type();
      $node->m_module = $module;
      atkModule::resetModuleScope(); 
      return $node;
    }
    
    /**
     * Set current module scope.
     *
     * @param string $module current module
     * @static
     */
    function setModuleScope($module)
    {
      global $g_atkModuleScope;
      $g_atkModuleScope = $module;
    }
    
    /**
     * Returns the current module scope.
     *
     * @return string current module
     * @static
     */
    function getModuleScope()
    {
      global $g_atkModuleScope;
      return $g_atkModuleScope;
    }
    
    /**
     * Resets the current module scope.
     * 
     * @static
     */
    function resetModuleScope()
    {
      atkModule::setModuleScope(null);
    }

    /**
     * Checks if a certain node exists for this module.
     * @param $node the node type
     * @return node exists?
     */
    function nodeExists($node)
    {
      // check for file
      $file = $this->getNodeFile($node);
      return file_exists($file);
    }

    function getModifierFunctions(&$node)
    {
      return array($node->m_type . "_modifier", str_replace(".", "_", $node->atknodetype()) . "_modifier");
    }

    /**
     * Modifies the given node
     *
     * @param atkNode $node Node to be modified
     */
    function modifier(&$node)
    {
      // Determine the modifier name and existance for modifiers that modify any node having the this node's type in any module
      $specificmodifiers = $this->getModifierFunctions($node);

      // Set the number of applied modifiers to zero
      $appliedmodifiers = 0;

      // Loop through the possible modifiers and apply them if found
      foreach ($specificmodifiers as $modifiername)
      {
        // If the modifiers is found
        if (method_exists($this, $modifiername))
        {
          // Add a debug line so we know, the modifier is applied
          atkdebug(sprintf("Applying modifier %s from module %s to node %s", $modifiername, $this->m_name, $node->m_type));

          // Apply the modifier
          $node->m_modifier = $this->m_name;
          $this->$modifiername($node);
          $node->m_modifier = "";

          // Increase the number of applied modifiers
          $appliedmodifiers++;
        }
      }

      // If none of the modifiers was found, add a warning to the debug log
      if ($appliedmodifiers == 0)
        atkdebug(sprintf("Failed to apply modifier function %s from module %s to node %s; modifier function not found", implode(" or ", $specificmodifiers), $this->m_name, $node->m_type), DEBUG_WARNING);
    }
  }
?>