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
   * @version $Revision$
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

    function atkProfileAttribute($name, $parentAttrName="", $flags=0)
    {
      if (is_numeric($parentAttrName))
      {
        $flags = $parentAttrName;
        $parentAttrName = "";
      }

      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH|AF_HIDE_LIST);
      $this->m_parentAttrName = $parentAttrName;
    }

    function load($db, $record)
    {
      $query = "SELECT *
                FROM ".atkconfig("auth_accesstable")."
                WHERE ".atkconfig("auth_levelfield")."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      $result = Array();
      $rows = $db->getrows($query);
      for ($i=0;$i<count($rows);$i++)
      {
        $result[$rows[$i]["node"]][] = $rows[$i]["action"];
      }
      return $result;
    }

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

    function store($db, $record, $mode)
    {
      global $g_user;

      $delquery = "DELETE FROM ".atkconfig("auth_accesstable")."
                   WHERE ".atkconfig("auth_levelfield")."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      if ($db->query($delquery))
      {
        $isAdmin =  ($g_user['name']=='administrator' || $this->canGrantAll());
        $allActions = $this->getAllActions($record);
        $editableActions = $this->getEditableActions($record);

        $checked = $record[$this->fieldName()];

        $children = array();
        if (!empty($this->m_parentAttrName))
          $children = $this->getChildGroups($db, $record[$this->m_ownerInstance->primaryKeyField()]);

        foreach ($checked as $node => $actions)
        {
          $nodeModule = getNodeModule($node);
          $nodeType = getNodeType($node);

          $validActions = array();
          if (is_array($allActions[$nodeModule][$nodeType]))
            $validActions = array_intersect($actions, $allActions[$nodeModule][$nodeType]);
          if (!$isAdmin && is_array($editActions[$nodeModule][$nodeType]))
            $validActions = array_intersect($validActions, $editActions[$nodeModule][$nodeType]);

          foreach ($validActions as $action)
          {
            $query = "INSERT INTO ".atkconfig("auth_accesstable")." (node, action, ".atkconfig("auth_levelfield").") ";
            $query.= "VALUES ('".escapeSQL($node)."','".escapeSQL($action)."','".$record[$this->m_ownerInstance->primaryKeyField()]."')";

            if (!$db->query($query))
            {
              // error.
              return false;
            }
          }

          if (count($children) > 0 && count($validActions) > 0)
          {
            $query = "DELETE FROM ".atkconfig("auth_accesstable")." ".
                     "WHERE ".atkconfig("auth_levelfield")." IN (".implode(",", $children).") ".
                     "AND node = '".escapeSQL($node)."' ".
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
     */
    function getAllActions($record)
    {
      global $g_modules, $g_moduleflags, $g_nodes;

      $result = array();

      // hierarchic groups, only return actions of parent (if this record has a parent)
      $parentAttr = $this->m_parentAttrName;
      if (!empty($parentAttr) && is_numeric($record[$parentAttr]))
      {
        $db = &atkGetDb();
        $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable")." ".
                 "WHERE ".atkconfig("auth_levelfield")." = ".$record[$parentAttr];
        $rows = $db->getRows($query);

        foreach ($rows as $row)
        {
          $module = getNodeModule($row['node']);
          $node = getNodeType($row['node']);
          $result[$module][$node][] = $row['action'];
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

          // retrieve actions for each node
          $nodes = isset($g_nodes[$module]) ? $g_nodes[$module] : array();
          foreach ($nodes as $node => $actions)
            if (is_array($actions))
              $result[$module][$node] = $actions;
        }
      }

      return $result;
    }

    /**
     * Returns a list of actions that should be edittable by the user.
     */
    function getEditableActions($record)
    {
      global $g_user;

      $levels = "";
      if (!is_array($g_user['level']))
        $levels = "'".$g_user['level']."'";
      else $levels = "'".implode("','",$g_user['level'])."'";

      // retrieve editable actions by user's levels
      $db = &atkGetDb();
      $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable"). " WHERE ".atkconfig("auth_levelfield")." IN (".$levels.")";
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
     * Returns the currently selected actions.
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


    function edit($record)
    {
      global $g_user;

      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkprofileattribute.js.php");

      $result='<div align="right">
                  [<a href="javascript:profile_checkAll(\''.$this->fieldName().'\')">'.
                    text("check_all").
             '</a> <a href="javascript:profile_checkNone(\''.$this->fieldName().'\')">'.
                    text("check_none").
             '</a> <a href="javascript:profile_checkInvert(\''.$this->fieldName().'\')">'.
                    text("invert_selection").'</a>]</div>';

      $isAdmin =  ($g_user['name']=='administrator' || $this->canGrantAll());
      $allActions = $this->getAllActions($record);
      $editableActions = $this->getEditableActions($record);
      $selectedActions = $this->getSelectedActions($record);

      $showModule = count($allActions) > 1 && ($isAdmin || count($editableActions) > 1);

      foreach ($allActions as $module => $nodes)
      {
        if ($showModule)
          $result .= "<br><hr><b>".text("title_$module")."</b><br><br>";

        foreach ($nodes as $node => $actions)
        {
          $showBox = $isAdmin || count(array_intersect($actions, (is_array($editableActions[$module][$node])?$editableActions[$module][$node]:array()))) > 0;

          if ($showBox)
            $result .= "<b>".text($node)."</b><br>";

          foreach ($actions as $action)
          {
            $isEditable = $isAdmin || atk_in_array($action, $editableActions[$module][$node]);
            $isSelected = is_array($selectedActions[$module][$node]) && in_array($action, $selectedActions[$module][$node]);

            if ($isEditable)
            {
              $result .= '<input type="checkbox" name="'.$this->formName().'[]" value="'.$module.".".$node.".".$action.'" ';
              $result .= ($isSelected ? ' checked="checked"' : '').'></input> ';
              $result .= $this->permissionName($action, $node, $module).'&nbsp;&nbsp;&nbsp;';
            }
            else
            {
              $result .= '<input type="hidden" name="'.$this->formName().'[]'.'" value="'.$module.".".$node.".".$action.'"></input>';
            }
          }

          if ($showBox)
            $result .= "<br /><br />\n";
        }
      }

      return $result;
    }

    /**
     * Return the translated name of a permission.
     */
    function permissionName($action, $nodename="", $modulename="")
    {
      $label = text("permission_".$action, $nodename, $modulename); // module/node specific text
      if (strtolower(substr($label,0,10))=="permission") // no suitable translation found
      {
        $label = text($action, $nodename, $modulename); // generic action text.
      }
      return $label;
    }

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
        $elems = split("\.",$checkboxes[$i]);
        if (count($elems)==3)
        {
          $node = $elems[0].".".$elems[1];
          $action = $elems[2];
        }
        else if (count($elems)==2)
        {
          $node = $elems[0];
          $action = $elems[1];
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

    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array();
    }

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

  }

?>