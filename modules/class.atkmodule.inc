<?php

  require_once($config_atkroot."atk/atkmenutools.inc");

/**
 * A generalization of an ATK module class.
 * All ATK modules have to extend this class.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 *
 * $Id$
 * $Log$
 * Revision 1.6  2001/09/12 09:04:58  ivo
 * Bugfixes to module modifiers that corrupted atknodes.
 *
 * Revision 1.5  2001/09/10 12:32:47  ivo
 * Improved module support. Modules can now create modifiers for other nodes.
 * Modules and external modules are now treated equally.
 * Added ability to specify order of menuitems
 *
 * Revision 1.4  2001/05/07 15:13:49  ivo
 * Put config_atkroot in all files.
 *
 * Revision 1.3  2001/05/01 09:49:49  ivo
 * Replaced all require() and include() calls by require_once() and
 * include_once() calls. The if(!DEFINED)... inclusion protection in files
 * is now obsolete.
 *
 * Revision 1.2  2001/04/24 13:51:50  ivo
 * Fixed some small bugs, and updated the language files, improved the menu.
 *
 * Revision 1.1  2001/04/23 13:21:22  peter
 * Introduction of module support. An ATK application can now have zero
 * or more modules which can, but don't have to, contain ATK nodes.
 *
 */
  
  class atkModule
  {
    /* module nodes */
    var $m_nodes = array();
    
    /* module attributes */
    var $m_attributes = array();
    
    /* module menu items */
    var $m_menu_items = array();
    
    /* preference links */
    var $m_preference_links = array();
    
    /* admin links */
    var $m_admin_links = array();
    
    var $m_name;

    /**
     * Constructor. The module needs to register it's nodes
     * overhere, create it's menuitems etc.
     */
    function atkModule($name)
    {
      $this->m_name = $name;
    }

    /**
     * Will return an array of atknode names and their supported actions
     * that are part of this module. If there are no atknodes within this
     * module an empty array will be returned.
     * @return array with atknode names
     */
    function getNodes()
    {
      return $this->m_nodes;
    }

    /**
     * Returns an array with filenames of attributes that need to be included
     * in order to let this module work properly.
     * @return array with attribute filenames
     */
    function getAttributes()
    {
      return $this->m_attributes;
    }

    /**
     * This method returns an array with menu items that need to be available 
     * in the main ATK menu. This function returns the array created with
     * the menuitem() method, and does not have to be extended!
     * @return array with menu items for this module
     */
    function getMenuItems()
    {
      return $this->m_menu_items;
    }

    /**
     * This method can be used to return an array similar to the menu array
     * but with links to (a) preference(s) page(s) for this module. The items
     * that will be returned have to be added with the preferencelink() method.
     * @return array with preference links for this module
     */
    function getPreferenceLinks()
    {
      return $this->m_preference_links;
    }

    /**
     * This method is similar to the getPreferenceLinks() method but instead
     * will return links to (an) admin page(s) for this module. The array which
     * will be returned have to be created with the adminlink() method.
     * @return array with admin links for thid module
     */
    function getAdminLinks()
    {
      return $this->m_admin_links;
    }

    /**
     * Register a node for this module
     * @param $node name of the node (without modulename!)
     * @param $action array with actions that can be performed on the node
     */
    function registerNode($node, $action)
    {
      $this->m_nodes[$node] = $action;
    }
    
    /**
     * Register an attribute for this module. This could come in handy
     * if another module is and add-on for the attribute and makes use
     * of the attributes that are already in the other module.
     * @param $file the filename of the attribute class
     */
    function registerAttribute($file)
    {
      $this->m_attributes[] = $file;
    }

    /**
     * Creates a new (sub)menu item or a new submenu
     * @param $name menu item or submenu name
     * @param $parent parent menu name (default is main)
     * @param $url the url the item points to
     * @param $enable enable this item or not
     */
    function menuitem($name, $url="", $parent="main", $enable=1, $order=0)
    {
//      if ($name == "-") $this->m_menu_items[$parent][] = "-"; else
      $this->m_menu_items[$parent][] = array("name" => $name, "url" => $url, "enable" => $enable, "order"=>$order);
    }
    
    /**
     * Creates a link to a preference page
     * @param $name the preference link name
     * @param $url the preference link url
     * @param $enable enable this link or not
     */
    function preferencelink($name, $url, $enable=1)
    {
      if (!empty($name) && !empty($url)) $this->m_preference_links[$name] = $url;
    }
    
    /**
     * Creates a link to an admin page
     * @param $name the admin link name
     * @param $url the admin link url
     * @param $enable enable this link or not
     */
    function adminlink($name, $url, $enable=1)
    {
      if (!empty($name) && !empty($url)) $this->m_admin_links[$name] = $url;
    }
        
  }

?>