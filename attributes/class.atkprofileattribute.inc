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
    function atkProfileAttribute($name, $flags=0)
    {
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH|AF_HIDE_LIST);
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


    function edit($record)
    {
      global $g_modules, $g_nodes, $config_atkroot, $g_layout, $g_moduleflags;
      global $g_user, $g_db;

      $g_layout->register_script($config_atkroot."atk/javascript/class.atkprofileattribute.js.php");
      
      /* get nodes */
      require_once($config_atkroot."atk/atknodetools.inc");

      if (file_exists("config.nodes.inc"))
      {
        include_once("config.nodes.inc");
      }
      foreach ($g_modules as $modname => $pathname)
      {
        if (!hasFlag($g_moduleflags[$modname], MF_NORIGHTS))
        {
          $mod_inst = &getModule($modname);
          if (method_exists($mod_inst, "getNodes")) $mod_inst->getNodes();
        }
      }

      $result.='<div align="right">
                  [<a href="javascript:profile_checkAll(\''.$this->fieldName().'\')">'.
                    text("check_all").
             '</a> <a href="javascript:profile_checkNone(\''.$this->fieldName().'\')">'.
                    text("check_none").
             '</a> <a href="javascript:profile_checkInvert(\''.$this->fieldName().'\')">'.
                    text("invert_selection").'</a>]</div>';
                    
      /* get permissions group*/
      $checked = $record[$this->fieldName()];

      /*get permission of user to find out which permission may be changed*/
      if(!is_array($g_user['level']))
        $levels = "'".$g_user['level']."'";
      else
        $levels = "'".implode("','",$g_user['level'])."'";
      $query = "SELECT DISTINCT node, action FROM ".atkconfig("auth_accesstable"). " WHERE ".atkconfig("auth_levelfield")." IN (".$levels.")";
      
      $activeNodeActions = $g_db->getrows($query);

      foreach( $activeNodeActions as $a)
      {
        $active[$a['node']][] = $a['action'];
      }

      /***  main application ***/
      $nodes = $g_nodes["main"];
      if (count($nodes) > 0)
      {
        $showModule=false;
        $mainHtml="";
        foreach($nodes as $node => $actionarr)
        {
          $checked_actions = $checked[$node];
          $active_actions = $active[$node];
          if (is_array($actionarr))
          {
            $html="";
            $showNode = false;

            for ($i=0;$i<count($actionarr);$i++)
            {

              $action = $actionarr[$i]; 
              if (substr($action,0,1)=="*") $action=substr($action,1);             
              
              if ((is_array($active_actions) && in_array($action, $active_actions)) || $g_user['name']=='administrator')
              {              
                $boxActive = true;
                $showModule = true;
                //'.($disable ? "DISABLED " : "").'
                $html.='<input type="checkbox" name="'.$this->formName().'[]" value="'.$node.".".$action.'" ';
                if (is_array($checked_actions)&&in_array($action, $checked_actions)) $html.=" checked";
  
                $label = text("permission_$node_$action");
                if (strtolower(substr($label,0,10))=="permission") // no suitable translation found
                {
                  $label = text($action, $node); // generic action text.
                }
                $html.='> '.$label.'&nbsp;&nbsp;&nbsp;';
              }
              else
              {
                if (is_array($checked_actions)&&in_array($action, $checked_actions))
                {
                  $html .= '<input type="hidden" name="'.$this->formName().'[]'.
                    '" value="'.$modname.".".$node.".".$action.'">';
                }
              }
            }
            
            if($boxActive)  
            {
              $mainHtml .="<b>".text(getNodeType($node))."</b><br>";
              $mainHtml.=$html;
              $mainHtml.="<br><br>\n";
            }
            else $mainHtml.=$html;
          }
          if (count($g_modules)>0 && $showModule)
          {
            // only show the header if there are more than only the main module.
            $result .= "<b>".text("app_title")."</b><br><br>".$mainHtml;
          }
        }
      }
      /*** per module ***/
      foreach ($g_modules as $modname => $pathname)
      {
        $nodes = $g_nodes[$modname];
        $mainHtml="";
        if (count($nodes) > 0)
        {        
          $showModule=false;
          foreach($nodes as $node => $actionarr)
          {
            $checked_actions = $checked["$modname.$node"];
            $active_actions = $active["$modname.$node"];
            if (is_array($actionarr))
            {
              $nodename = getNodeType($node);              
              $html="";
              $boxActive=false;
              for ($i=0;$i<count($actionarr);$i++)
              {                
                $action = $actionarr[$i];
                if (substr($action,0,1)=="*") $action=substr($action,1);

                if ((is_array($active_actions) && in_array($action, $active_actions)) || $g_user['name']=='administrator')
                {
                  $showModule=true;
                  $boxActive=true;
                  $html.='<input  type="checkbox" name="'.$this->formName().'[]" value="'.$modname.".".$node.".".$action.'" ';
                  if (is_array($checked_actions)&&in_array($action, $checked_actions)) $html.=" checked";

                  $label = $this->permissionName($action, $node, $modname);
                  $html.='> '.$label.'&nbsp;&nbsp;&nbsp;';
                }
                else
                {
                  if (is_array($checked_actions)&&in_array($action, $checked_actions))
                  {
                    $html .= '<input type="hidden" name="'.$this->formName().'[]'.
                      '" value="'.$modname.".".$node.".".$action.'">';
                  }
                }
              }
              if($boxActive)  
              {
                $mainHtml .="<b>".text(getNodeType($nodename))."</b><br>";
                $mainHtml .=$html;
                $mainHtml .="<br><br>\n";
              }
              else 
              {
                $mainHtml.=$html;
              }
            }
          }          
          if ($showModule)
          {
            // only show the header if there are more than only the main module.
            $result .= "<br><hr><b>".text("title_$modname")."</b><br><br>".$mainHtml;
          } 
          else    $result .= $mainHtml;
          
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
        $label = text($action, $node, $modulename); // generic action text.        
      }
      return $label;
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
 
    function dbFieldType()
    {
      return "";
    }

  }

?>
