<?php
  /**
   * The atkProfileAttribute is an attribute to edit a security profile.
   * The best way to use it is inside the class where you edit your
   * profile or usergroup records.
   *
   * @author Ivo Jansch (ivo@ibuildings.nl)
   * @version $Revision$
   *
   * $Id$
   *
   */

  // for editing profiles we create a custom attribute
  class atkProfileAttribute extends atkAttribute
  {
    function atkProfileAttribute($name, $flags)
    {
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH);
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

    function store($db, $record, $mode)
    {
      $delquery = "DELETE FROM ".atkconfig("auth_accesstable")."
                   WHERE ".atkconfig("auth_levelfield")."='".$record[$this->m_ownerInstance->primaryKeyField()]."'";

      $db->query($delquery);

      $checked = $record[$this->fieldName()];

      while (list($node, $actions)=each($checked))
      {
        for ($i=0;$i<count($actions);$i++)
        {
          $query = "INSERT INTO ".atkconfig("auth_accesstable")." (node, action, ".atkconfig("auth_levelfield").") ";
          $query.= "VALUES ('".$node."','".$actions[$i]."','".$record[$this->m_ownerInstance->primaryKeyField()]."')";

          $db->query($query);
        }
      }
    }

    function edit($record)
    {
      global $g_modules, $g_nodes, $config_atkroot, $g_layout;

      $g_layout->register_script($config_atkroot."atk/javascript/class.atkprofileattribute.js.php");

      /* get nodes */
      require_once($config_atkroot."atk/atknodetools.inc");
      include_once("config.nodes.inc");
      foreach ($g_modules as $modname)
      {
        $mod_inst = &getModule($modname);
        if (method_exists($mod_inst, "getNodes")) $mod_inst->getNodes();
      }

      $result.='<div align="right">
                  [<a href="javascript:profile_checkAll(\''.$this->fieldName().'\')">'.
                    text("check_all").
             '</a> <a href="javascript:profile_checkNone(\''.$this->fieldName().'\')">'.
                    text("check_none").
             '</a> <a href="javascript:profile_checkInvert(\''.$this->fieldName().'\')">'.
                    text("invert_selection").'</a>]</div>';

      /* get permissions */
      $checked = $record[$this->fieldName()];

      /***  main application ***/
      $nodes = $g_nodes["main"];
      if (count($nodes) > 0)
      {
        if (count($g_modules)>0)
        {
          // only show the header if there are more than only the main module.
          $result .= "<b>".text("app_title")."</b><br><br>";
        }
        while (list($node, $actionarr) = each($nodes))
        {
          $checked_actions = $checked[$node];
          if (is_array($actionarr))
          {
            $result.="<b>".text(getNodeType($node))."</b><br>";
            for ($i=0;$i<count($actionarr);$i++)
            {
              $action = $actionarr[$i];
              if (substr($action,0,1)=="*") $action=substr($action,1);
              $result.='<input type="checkbox" name="'.$this->formName().'[]" value="'.$node.".".$action.'" ';
              if (is_array($checked_actions)&&in_array($action, $checked_actions)) $result.=" checked";

              $label = text("permission_$node_$action");
              if (strtolower(substr($label,0,10))=="permission") // no suitable translation found
              {
                $label = text($action, $node); // generic action text.
              }
              $result.='> '.$label.'&nbsp;&nbsp;&nbsp;';
            }
            $result.="<br><br>\n";
          }
        }
      }

      /*** per module ***/
      foreach ($g_modules as $modname)
      {
        $nodes = $g_nodes[$modname];

        if (count($nodes) > 0)
        {
          $result .= "<br><hr><b>".text("title_$modname")."</b><br><br>";
          while (list($node, $actionarr) = each($nodes))
          {
            $checked_actions = $checked["$modname.$node"];
            if (is_array($actionarr))
            {
              $nodename = getNodeType($node);
              $result.="<b>".text($nodename)."</b><br>";
              for ($i=0;$i<count($actionarr);$i++)
              {
                $action = $actionarr[$i];
                if (substr($action,0,1)=="*") $action=substr($action,1);
                $result.='<input type="checkbox" name="'.$this->formName().'[]" value="'.$modname.".".$node.".".$action.'" ';
                if (is_array($checked_actions)&&in_array($action, $checked_actions)) $result.=" checked";

                $label = text("permission_".$modname."_".$nodename."_".$action); // module/node specific text
                if (strtolower(substr($label,0,10))=="permission") // no suitable translation found
                {
                  $label = text("permission_".$nodename."_".$action); // node specific text
                  if (strtolower(substr($label,0,10))=="permission")
                  {
                    $label = text($action, $node); // generic action text.
                  }
                }
                $result.='> '.$label.'&nbsp;&nbsp;&nbsp;';
              }
              $result.="<br><br>\n";
            }
          }
        }
      }
      return $result;
    }

    function fetchValue($postvars)
    {
      $checkboxes = $postvars[$this->fieldName()];
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


  }

?>
