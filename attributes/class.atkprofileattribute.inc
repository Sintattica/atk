<?php
  /**
   * The atkProfileAttribute is an attribute to edit a security profile.
   * The best way to use it is inside the class where you edit your 
   * profile or usergroup records.
   *
   * @author Ivo Jansch (ivo@ibuildings.nl)
   *
   * $Id$
   *
   * $Log$
   * Revision 4.5  2001/09/24 07:28:59  ivo
   * Fixed a bug with module handling in profile attribute.
   *
   * Revision 4.4  2001/09/11 11:17:45  ivo
   * atkprofileattribute now works correctly with modules.
   *
   * Revision 4.3  2001/09/10 12:32:46  ivo
   * Improved module support. Modules can now create modifiers for other nodes.
   * Modules and external modules are now treated equally.
   * Added ability to specify order of menuitems
   *
   * Revision 4.2  2001/06/29 11:26:59  ivo
   * Patches to oracle support.
   *
   * Revision 4.1  2001/06/18 10:05:51  ivo
   * New attribute: atkProfileAttribute, for editing security profiles.
   * New node flag: NF_NO_SECURITY
   *   
   *
   */

  // for editing profiles we create a custom attribute
  class atkProfileAttribute extends atkAttribute
  {
    function profileAttribute($name, $flags)
    {
      $this->atkAttribute($name, $flags);
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
      global $g_modules;  
    
      /* get nodes */
      require_once("atk/atknodetools.inc");
      include_once("config.nodes.inc");
  
      /* get permissions */
      $checked = $record[$this->fieldName()];
      
      /***  main application ***/
      $nodes = $GLOBALS["g_nodes"];
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
              $result.='> '.text($action).'&nbsp;&nbsp;&nbsp;';
            }
            $result.="<br><br>";
          }
        }
      }
   
      /*** per module ***/
      foreach ($g_modules as $modname => $pathname)
      {
        $mod_inst = getModule($modname);
        $nodes = $mod_inst->getNodes();
        if (count($nodes) > 0)
        {
          $result .= "<br><b><u>".text("title_$modname")."</u></b><br><br>";      
          while (list($node, $actionarr) = each($nodes))
          {
            $checked_actions = $checked["$modname.$node"];
            if (is_array($actionarr))
            {
              $result.="<b>".text(getNodeType($node))."</b><br>";
              for ($i=0;$i<count($actionarr);$i++)
              {
                $action = $actionarr[$i];
                if (substr($action,0,1)=="*") $action=substr($action,1);
                $result.='<input type="checkbox" name="'.$this->formName().'[]" value="'.$modname.".".$node.".".$action.'" ';
                if (is_array($checked_actions)&&in_array($action, $checked_actions)) $result.=" checked";
                $result.='> '.text($action).'&nbsp;&nbsp;&nbsp;';
              }
              $result.="<br><br>";
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
    
  }
  
?>
