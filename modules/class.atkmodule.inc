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
 *
 */

  class atkModule
  {
    var $m_name;

    /**
     * Constructor. The module needs to register it's nodes
     * overhere, create it's menuitems etc.
     */
    function atkModule($name="")
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
     * Function that adds a menuitem to the menu. This function can be called
     * from inside the getMenuItems() function, to register a menuitem.
     */
    function menuitem($name="", $url="", $parent="main", $enable=1, $order=0)
    {
      // Call basic menuitem.
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
     * Construct a new node. A module can override this method for it's own nodes.
     * @param $node the node type
     * @return new node object
     */
    function &newNode($node)
    {
      global $g_modules, $config_atkroot, $g_overloaders;

      /* overloader check */
      if (!empty($g_overloaders[$node]))
      {
        atkdebug("Using overloader '".$g_overloaders[$node]."' for class '".$node."'");

        /* include original file, because overloaders extend the original class */
        $file = nodeFile($node);

        /* check for file */
        if (!file_exists($file))
        {
          atkerror("Cannot create node, because a required file ($file) does not exist!", "critical");
          return NULL;
        }

        /* return object */
        include_once($file);
        $node = $g_overloaders[$node];
      }

      $file = nodeFile($node);
      $type = getNodeType($node);
      $module = getNodeModule($node);

      /* check for file */
      if (!file_exists($file))
      {
        if (function_exists("missing_class_handler"))
        {
          return missing_class_handler($node);
        }
        else
        {
          atkerror("Cannot create node, because a required file ($file) does not exist!", "critical");
          return NULL;
        }
      }

      /* include file */
      include_once($file);

      /* initialize node */
      $node =  new $type();
      $node->m_module = $module;
      $node->init();

      return $node;
    }
  }
?>