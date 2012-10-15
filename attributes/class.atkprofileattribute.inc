<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6177 $
   * $Id$
   */

  /**
   * The atkProfileAttribute is an attribute to edit a security profile.
   * The best way to use it is inside the class where you edit your
   * profile or usergroup records.
   *
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkProfileAttribute extends atkAttribute
  {
    var $m_parentAttrName;
    var $m_accessField;

    /**
     * Constructor
     *
     * @param string $name The name of the attribute
     * @param string $parentAttrName
     * @param int $flags The flags of this attribute
     * @return atkProfileAttribute
     */
    function atkProfileAttribute($name, $parentAttrName="", $flags=0)
    {
      if (is_numeric($parentAttrName))
      {
        $flags = $parentAttrName;
        $parentAttrName = "";
      }

      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH|AF_HIDE_LIST);
      $this->m_parentAttrName = $parentAttrName;
      
      $this->m_accessField = atkconfig('auth_accessfield');
      if (empty($this->m_accessField))
        $this->m_accessField = atkconfig('auth_levelfield');
    }

    /**
     * Load this record
     *
     * @param atkDb $db The database object
     * @param array $record The record
     * @return array Array with loaded values
     */
    function load(&$db, $record)
    {
      $query = "SELECT *
                FROM ".atkconfig("auth_accesstable")."
                WHERE ".$this->m_accessField."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      $result = Array();
      $rows = $db->getrows($query);
      for ($i=0;$i<count($rows);$i++)
      {
        $result[$rows[$i]["node"]][] = $rows[$i]["action"];
      }
      return $result;
    }

    /**
     * Get child groups
     *
     * @param atkDb $db The database object
     * @param int $id The id to search for
     * @return array
     */
    function getChildGroups(&$db, $id)
    {
      $result = array();
      if (!is_numeric($id))
        return $result;

      $query = "SELECT ".$this->m_ownerInstance->primaryKeyField()." ".
               "FROM ".$this->m_ownerInstance->m_table." ".
               "WHERE ".$this->m_parentAttrName." = $id";


      $rows = $db->getRows($query);
      foreach ($rows as $row)
      {
        $id = $row[$this->m_ownerInstance->primaryKeyField()];
        $result = array_merge($result, array($id), $this->getChildGroups($db, $id));
      }

      return $result;
    }

    /**
     * Store the value of this attribute in the database
     *
     * @param atkDb $db The database object
     * @param array $record The record which holds the values to store
     * @param string $mode The mode we're in
     * @return bool True if succesfull, false if not
     */
    function store(&$db, $record, $mode)
    {
      global $g_user;

      // Read the current actions available/editable and user rights before changing them
      $isAdmin = ($g_user['name']=='administrator' || $this->canGrantAll());
      $allActions = $this->getAllActions($record, false);
      $editableActions = $this->getEditableActions($record);

      $delquery = "DELETE FROM ".atkconfig("auth_accesstable")."
                   WHERE ".$this->m_accessField."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      if ($db->query($delquery))
      {

        $checked = $record[$this->fieldName()];

        $children = array();
        if (!empty($this->m_parentAttrName))
          $children = $this->getChildGroups($db, $record[$this->m_ownerInstance->primaryKeyField()]);

        foreach ($checked as $node => $actions)
        {
          $actions = array_unique($actions);

          $nodeModule = getNodeModule($node);
          $nodeType = getNodeType($node);

          $validActions = array();

          if (is_array($allActions[$nodeModule][$nodeType]))
            $validActions = array_intersect($actions, $allActions[$nodeModule][$nodeType]);

          // If you're not an admin, leave out all actions which are not editable (none if no editable actions available)
          if (!$isAdmin)
            $validActions = isset($editableActions[$nodeModule][$nodeType]) ? array_intersect($validActions, $editableActions[$nodeModule][$nodeType]) : array();

          foreach ($validActions as $action)
          {
            $query = "INSERT INTO ".atkconfig("auth_accesstable")." (node, action, ".$this->m_accessField.") ";
            $query.= "VALUES ('".$db->escapeSQL($node)."','".$db->escapeSQL($action)."','".$record[$this->m_ownerInstance->primaryKeyField()]."')";

            if (!$db->query($query))
            {
              // error.
              return false;
            }
          }

          if (count($children) > 0 && count($validActions) > 0)
          {
            $query = "DELETE FROM ".atkconfig("auth_accesstable")." ".
                     "WHERE ".$this->m_accessField." IN (".implode(",", $children).") ".
                     "AND node = '".$db->escapeSQL($node)."' ".
                     "AND action NOT IN ('".implode("','", $validActions)."')";

            if (!$db->query($query))
            {
              // error.
              return false;
            }
          }
        }
      }

      return true;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                this attribute's value without showing it.
     */
    function hide($record="", $fieldprefix="")
    {
      // get checks
      $checked = $record[$this->fieldName()];

      // rebuild hidden fields from checked boxes
      $rights  = "";

      foreach($checked as $key=>$val)
      {
        for ($i=0;$i<=count($val)-1;$i++)
        {
          $value   = $key.".".$val[$i];
          $rights .= '<input type="hidden" name="rights[]" value="'.$value.'">';
        }
      }
      return $rights;
    }

    /**
     * Retrieve all possible module/node actions.
     * 
     * @param array $record The record
     * @return array Array with actions
     */
    function getAllActions($record, $splitPerSection=false)
    {
      global $g_modules, $g_moduleflags, $g_nodes;

      $result = array();

      // hierarchic groups, only return actions of parent (if this record has a parent)
      $parentAttr = $this->m_parentAttrName;
      if (!empty($parentAttr) && is_numeric($record[$parentAttr]))
      {
        $db = &atkGetDb();
        $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable")." ".
                 "WHERE ".$this->m_accessField." = ".$record[$parentAttr];
        $rows = $db->getRows($query);

        foreach ($rows as $row)
        {
          $module = getNodeModule($row['node']);
          $node = getNodeType($row['node']);
          $result[$module][$module][$node][] = $row['action'];
        }
      }

      // non-hierarchic groups, or root
      else
      {
        // include node information
        require_once(atkconfig("atkroot")."atk/atknodetools.inc");
        if (file_exists("config.nodes.inc"))
          include_once("config.nodes.inc");

        // get nodes for each module
        foreach (array_keys($g_modules) as $module)
        {
          if (!isset($g_moduleflags[$module]) || !hasFlag($g_moduleflags[$module], MF_NORIGHTS))
          {
            $instance = &getModule($module);
            if (method_exists($instance, "getNodes"))
              $instance->getNodes();
          }
        }
        
        // retrieve all actions after we registered all actions
        $result = $g_nodes;
      }
      
      if (!$splitPerSection)
      {
        $temp = array();
        foreach ($result as $section => $modules)
        {
          foreach ($modules as $module => $nodes)
          {
            if (!is_array($temp[$module]))
            {
              $temp[$module] = array();
            }
            
            $temp[$module] = array_merge($temp[$module], $nodes);
          }
        }
        
        $result = $temp;
      }
      
      return $result;
    }

    /**
     * Returns a list of actions that should be edittable by the user.
     * 
     * @param array $record The record
     * @return array Array with editable actions
     */
    function getEditableActions($record)
    {
      $user = getUser();
      $levels = "";
      if (!is_array($user['level']))
        $levels = "'".$user['level']."'";
      else $levels = "'".implode("','",$user['level'])."'";

      // retrieve editable actions by user's levels
      $db = &atkGetDb();
      $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable"). " WHERE ".$this->m_accessField." IN (".$levels.")";
      $rows = $db->getRows($query);

      $result = array();
      foreach ($rows as $row)
      {
        $module = getNodeModule($row['node']);
        $node = getNodeType($row['node']);
        $result[$module][$node][] = $row['action'];
      }

      return $result;
    }

    /**
     * Initially use an empty rights array.
     *
     * @return array initial rights
     */
    function initialValue()
    {
      return array();
    }
    
    /**
     * Returns the currently selected actions.
     * 
     * @param array $record The record
     * @return array array with selected actions
     */
    function getSelectedActions($record)
    {
      $selected = $record[$this->fieldName()];

      $result = array();
      foreach ($selected as $node => $actions)
      {
        $module = getNodeModule($node);
        $node = getNodeType($node);
        $result[$module][$node] = $actions;
      }

      return $result;
    }

    /**
     * Display rights.
     * It will only display the rights & nodes that are selected for the user.
     *
     * @param array $record
     *
     * @return string Displayable string
     */
    public function display($record)
    {
      $user = getUser();
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");
      $this->_restoreDivStates($page);

      $result  = '';
      $isAdmin =  ($user['name']=='administrator' || $this->canGrantAll());
      
      $allActions      = $this->getAllActions($record, false);
      $editableActions = $this->getEditableActions($record);
      $selectedActions = $this->getSelectedActions($record);

      $showModule = count($allActions) > 1 && ($isAdmin || count($editableActions) > 1);

      $firstModule = true;

      foreach ($allActions as $module => $nodes)
      {
        // If we have more then one module, split up the module results by collapsable div's
        if ($showModule)
        {
          $result .= "<br><hr>";
          if ($firstModule)
            $firstModule = false;
          else
            $result .= '</div><br>';
          $result .= '<b><a href="javascript:void(0)" onclick="profile_swapProfileDiv(\'div_' . $module . "', '" . atkConfig('atkroot') . '\'); return false;"><img src="' . atkConfig('atkroot') . 'atk/images/plus.gif" border="0" id="img_div_' . $module .'></a>&nbsp;</b>' . atktext(array('title_' . $module, $module), $module) . '<br>';
          $result .= "<div id='div_$module' name='div_$module' style='display: none;'>";
          $result .= "<input type='hidden' name=\"divstate['div_$module']\" id=\"divstate['div_$module']\" value='closed' />";
          $result .= "<br>";
        }
        
        foreach ($nodes as $node => $actions)
        {
          $showBox = $isAdmin || count(array_intersect($actions, (is_array($editableActions[$module][$node])?$editableActions[$module][$node]:array()))) > 0;
          $display_node_str       = false;
          $display_tabs_str       = false;
          $node_result            = '';
          $permissions_string     = '';
          $tab_permissions_string = '';

          foreach ($actions as $action)
          {
            $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

            // If the action of a node is selected for this user we will show the node,
            // otherwise we won't
            if ($isSelected) 
            {
              $display_node_str = true;
              if (substr($action,0,4) == "tab_")
              {
                $display_tabs_str = true;
                $tab_permissions_string .= $this->permissionName($action, $node, $module) . '&nbsp;&nbsp;&nbsp;';
              }
              else
              {
                $permissions_string     .= $this->permissionName($action, $node, $module) . '&nbsp;&nbsp;&nbsp;';
              }
            }
          }
          
          if ($showBox)
          {
            $node_result .= "<b>".atktext($node, $module)."</b><br>";
            $node_result .=   $permissions_string;
            if ($display_tabs_str)
              $node_result .= "<br>Tabs:&nbsp;".$tab_permissions_string;
            $node_result .= "<br /><br />\n";
          }else{
            $node_result .= $permissions_string;
            if ($display_tabs_str)
              $node_result .= "<br>Tabs:&nbsp;".$tab_permissions_string;
          }
            
          if ($display_node_str)
            $result .= $node_result;
        }
      }

      return $result;
    }
    
    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      $user = getUser();
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");

      $this->_restoreDivStates($page);

      $result='<div align="right">
                  [<a href="javascript:void(0)" onclick="profile_checkAll(\''.$this->fieldName().'\'); return false;">'.
                    atktext("check_all").
             '</a> | <a href="javascript:void(0)" onclick="profile_checkNone(\''.$this->fieldName().'\'); return false;">'.
                    atktext("check_none").
             '</a> | <a href="javascript:void(0)" onclick="profile_checkInvert(\''.$this->fieldName().'\'); return false;">'.
                    atktext("invert_selection").'</a>]</div>';

      $isAdmin =  ($user['name']=='administrator' || $this->canGrantAll());
      $allActions = $this->getAllActions($record, true);
      $editableActions = $this->getEditableActions($record);
      $selectedActions = $this->getSelectedActions($record);

      $showSection = count($allActions) > 1;

      foreach ($allActions as $section => $modules)
      {
        if ($showSection)
        {
          $result .= "</div><br>";
          $result .= "<span  onclick=\"profile_swapProfileDiv('div_$section','".atkConfig("atkroot")."');\" style=\"cursor: pointer; font-size: 110%; font-weight: bold\"><img src=\"".atkConfig("atkroot")."atk/images/plus.gif\" border=\"0\" id=\"img_div_$section\">&nbsp;".atktext(array("title_$section", $section), $section)."</span><br/>";
          $result .= "<div id='div_$section' name='div_$section' style='display: none; padding-left: 15px'>";
          $result .= "<input type='hidden' name=\"divstate['div_$section']\" id=\"divstate['div_$section']\" value='closed' />";
          $result.='<div style="font-size: 80%; margin-top: 4px; margin-bottom: 4px" >
                  [<a  style="font-size: 100%" href="javascript:void(0)" onclick="profile_checkAllByValue(\''.$this->fieldName().'\',\''.$section.'.\'); return false;">'.
                    atktext("check_all", "atk").
             '</a> | <a  style="font-size: 100%" href="javascript:void(0)" onclick="profile_checkNoneByValue(\''.$this->fieldName().'\',\''.$section.'.\'); return false;">'.
                    atktext("check_none", "atk").
             '</a> | <a  style="font-size: 100%" href="javascript:void(0)" onclick="profile_checkInvertByValue(\''.$this->fieldName().'\',\''.$section.'.\'); return false;">'.
                    atktext("invert_selection", "atk").'</a>]</div>';
          $result .= "<br>";
        }
                  
        foreach ($modules as $module => $nodes)
        {
          foreach ($nodes as $node => $actions)
          {
            $showBox = $isAdmin || count(array_intersect($actions, (is_array($editableActions[$module][$node])?$editableActions[$module][$node]:array()))) > 0;
  
            if ($showBox)
              $result .= "<b>".atktext($node, $module)."</b><br>";
  
            $tabs_str = "";
            $display_tabs_str = false;
  
            // Draw action checkboxes
            foreach ($actions as $action)
            {
              $temp_str = "";
  
              $isEditable = $isAdmin || atk_in_array($action, $editableActions[$module][$node]);
              $isSelected = isset($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);
  
              if ($isEditable)
              {
                if (substr($action,0,4) == "tab_")
                  $display_tabs_str = true;
  
                $temp_str .= '<input type="checkbox" name="'.$this->formName().'[]" '.$this->getCSSClassAttribute("atkcheckbox").' value="'.$section.".".$module.".".$node.".".$action.'" ';
                $temp_str .= ($isSelected ? ' checked="checked"' : '').'></input> ';
                $temp_str .= $this->permissionName($action, $node, $module).'&nbsp;&nbsp;&nbsp;';
              }
  
              if (substr($action,0,4) == "tab_")
                $tabs_str .= $temp_str;
              else
                $result .= $temp_str;
            }
  
            if ($display_tabs_str)
              $result .= "<br>Tabs:&nbsp;";
  
            $result .= $tabs_str;
  
            if ($showBox)
              $result .= "<br /><br />\n";
          }
        }
      }
      
      $result = '<div style="min-width: 700px">'.$result.'</div>';
      
      return $result;
    }

    /**
     * Return the translated name of a permission.
     * 
     * @param string $action The name of the action
     * @param string $nodename The name of the node
     * @param string $modulename The name of the module
     * @return String The translated permission name
     */
    function permissionName($action, $nodename="", $modulename="")
    {
      $keys = array(
        'permission_'.$modulename.'_'.$nodename.'_'.$action,
        'action_'.$modulename.'_'.$nodename.'_'.$action,
        'permission_'.$nodename.'_'.$action,
        'action_'.$nodename.'_'.$action,
        'permission_'.$action,
        'action_'.$action,
        $action
      );
      
      // don't use text() function of attribute, because of auto module detection
      $label = atktext($keys, $modulename, $nodename);
      
      return $label;
    }

    /**
     * Convert values from an HTML form posting to an internal value for
     * this attribute.
     *
     * For the regular atkAttribute, this means getting the field with the
     * same name as the attribute from the html posting.
     *
     * @param array $postvars The array with html posted values ($_POST, for
     *                        example) that holds this attribute's value.
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
      $checkboxes = array();
      if (isset($postvars[$this->fieldName()]))
      {
        $checkboxes = $postvars[$this->fieldName()];
      }

      $actions = Array();
      for($i=0;$i<count($checkboxes);$i++)
      {
        $elems = explode(".",$checkboxes[$i]);
        if (count($elems)==4)
        {
          $node = $elems[1].".".$elems[2];
          $action = $elems[3];
        }
        else if (count($elems)==3)
        {
          $node = $elems[1];
          $action = $elems[2];
        }
        else
        {
          // never happens..
          atkdebug("profileattribute encountered incomplete combination");
        }
        $actions[$node][] = $action;
      }

      return $actions;
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array();
    }

    /**
     * Return the database field type of the attribute.
     *
     * Note that the type returned is a 'generic' type. Each database
     * vendor might have his own types, therefor, the type should be
     * converted to a database specific type using $db->fieldType().
     *
     * If the type was read from the table metadata, that value will
     * be used. Else, the attribute will analyze its flags to guess
     * what type it should be. If AF_AUTO_INCREMENT is set, the field
     * is probaly "number". If not, it's probably "string".
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.
     */
    function dbFieldType()
    {
      return "";
    }

    /**
     * Checks whether the current user has the 'grantall' privilege (if such a
     * privilege exists; this is determined by the application by setting
     * $config_auth_grantall_privilege.
     *
     * @return boolean
     */
    function canGrantAll()
    {
      $privilege_setting = atkconfig("auth_grantall_privilege");

      if ($privilege_setting!="")
      {
        global $g_securityManager;
        list($mod, $node, $priv) = explode(".", $privilege_setting);
        return $g_securityManager->allowed($mod.".".$node, $priv);
      }
      return false;
    }

    /**
     * Restore divs states
     *
     * @param atkPage $page
     */
    function _restoreDivStates(&$page)
    {
      $postvars = &$this->m_ownerInstance->m_postvars;
      if (!isset($postvars['divstate']) || !is_array($postvars['divstate']) || sizeof($postvars['divstate']) == 0)
        return;

      $divstate = $postvars['divstate'];
      $onLoadScript = "";

      foreach($divstate as $key => $value)
      {
        $key = substr($key,2,-2);
        if ($value == "opened")
          $onLoadScript .= "profile_swapProfileDiv('$key','".atkConfig("atkroot")."');";
      }
      $page->register_loadscript($onLoadScript);
    }
  }

?>
