<?php
atkimport('atk.datagrid.atkdgcomponent');

class atkDGList extends atkDGComponent 
{
  var $m_actionSessionStatus = SESSION_NESTED;
  
  function render()
  {
    $grid = $this->getGrid();
    $data = $this->getRecordlistData($grid->getNode(), $grid->getRecords(), $grid->getActions(), $grid->getFlags(), $grid->getSuppressList(), $grid->getFormName(), $grid->getNavigation(), $grid->getPrefix());
    $ui   = $grid->getNode()->getUi();
    $res = $ui->render($grid->getNode()->getTemplate("admin"), array("vorientation"=>$data["vorientation"],
                                                 "rows"=>$data["rows"],
                                                 "header"=>$data["header"],
                                                 "search"=>$data["search"],
                                                 "sort"=>$data["sort"],
                                                 "total"=>$data["total"],
                                                 "searchstart"=>$data["searchstart"],
                                                 "searchend"=>$data["searchend"],
                                                 "sortstart"=>$data["sortstart"],
                                                 "sortend"=>$data["sortend"],
                                                 "liststart"=>$data["liststart"],
                                                 "listend"=>$data["listend"],
                                                 "listid"=>$data["listid"],
                                                 "mra"=>$data["mra"]),$grid->getNode()->m_module);
    return $res;  
  }  
  
  /**
   * Get records for a recordlist without actually rendering the recordlist.
   * @param Array   $recordset    the list of records
   * @param Array   $actions      the default actions array
   * @param Integer $flags        recordlist flags (see the top of this file)
   * @param Array   $suppressList fields we don't display
   * @param String  $formName     if embedded the form name in which we are embedded
   * @param Array   $navigation   Navigation links
   * @param String  $embedprefix  The prefix for embeded fields
   * @return String The rendered recordlist
   */
  function getRecordlistData(&$node, $recordset, $actions, $flags=0, $suppressList="", $formName="", $navigation=array(),$embedprefix="")
  {
    $grid = $this->getGrid();
    $theme = &atkinstance("atk.ui.atktheme");
    $page = &atkPage::getInstance();
    $page->register_style($theme->stylePath("recordlist.css",$grid->getNode()->m_module));

    $listName = $grid->getName();
    $page->register_script(atkconfig("atkroot")."atk/javascript/recordlist.js");

    $defaulthighlight = $theme->getAttribute("highlight");
    $selectcolor = $theme->getAttribute("select");

    /* retrieve list array */
    $list = $this->listArray($recordset, $flags, "", $actions, $suppressList, $embedprefix);

    /* Check if some flags are still valid or not... */
    if (hasFlag($flags, RL_MRA) && (count($list["mra"]) == 0 || count($list["rows"]) == 0)) $flags ^= RL_MRA;
    if (!hasFlag($flags, RL_NO_SEARCH) && count($list["search"]) == 0) $flags |= RL_NO_SEARCH;
    if (hasFlag($flags, RL_MRPA) && (count($grid->getNode()->m_priority_actions) == 0 || count($list["rows"]) == 0)) $flags ^= RL_MRPA;
    elseif (hasFlag($flags, RL_MRPA))
    {
      $flags = ($flags | RL_MRA | RL_MRPA ) ^ RL_MRA;
      if ($grid->getNode()->m_priority_max == 0)
        $grid->getNode()->m_priority_max = $grid->getNode()->m_priority_min + count($list["rows"]) - 1;
    }

    $orientation  = atkconfig('recordlist_orientation', $theme->getAttribute("recordlist_orientation"));
    $vorientation = trim(atkconfig('recordlist_vorientation', $theme->getAttribute("recordlist_vorientation")));

    $ui = &$grid->getNode()->getUi();

    if (is_object($ui) && is_object($page))
    {

      /**************/
      /* HEADER ROW */
      /**************/
      $headercols = array();

      if ($this->_hasActionColumn($list) && count($list["rows"]) == 0)
      {
        if ($orientation == "left" || $orientation == "both")
        {
          // empty cell above search button, if zero rows
          // if $orientation is empty, no search button is shown, so no empty cell is needed
          $headercols[] = array("content"=>"&nbsp;");
        }
      }
      if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA))
      {
        $headercols[] = array("content"=>""); // Empty leader on top of mra action list.
      }
      if (hasFlag($flags, RL_LOCK))
      {
        $headercols[] = array("content"=>'<img src="'.atkconfig("atkroot").'atk/images/lock_head.gif">');
      }
      if (($orientation == "left" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0))
      {
        $headercols[] = array("content"=>"");
      }
      //Todo: For speedup we must move hasFlag($this->m_flags, RL_EMBED out of cycle or to listArray()
      foreach (array_values($list["heading"]) as $head)
      {
        if (empty($head["order"]))
        {
          $headercols[] = array("content"=>$head["title"]);
        }
        else
        {
          $call = htmlentities($grid->getUpdateCall(array('atkorderby' => $head['order'])));
          $link = '<a href="javascript:void(0)" onclick="'.$call.'">'.$head["title"].'</a>';
          $headercols[] = array("content" => $link);
        }
      }

      if (($orientation == "right" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0))
      {
        $headercols[] = array("content"=>"");
      }

      if ($this->_hasActionColumn($list) && count($list["rows"]) == 0)
      {
        if ($orientation == "right" || $orientation == "both")
        {
          // empty cell above search button, if zero rows
          // if $orientation is empty, no search button is shown, so no empty cell is needed
          $headercols[] = array("content"=>"&nbsp;");
        }
      }


      /**************/
      /* SORT   ROW */
      /**************/
      $sortcols = array();
      $sortstart=""; 
      $sortend="";
      if (hasFlag($flags, RL_EXT_SORT))
      {
        $call = htmlentities($grid->getUpdateCall(array('atkstartat' => 0)));
        $button = '<input type="button" value="'.atktext("sort").'" onclick="'.$call.'">';
        if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA))
        {
          $sortcols[] = array("content"=>""); // Empty leader on top of mra action list.
        }
        if (hasFlag($flags, RL_LOCK))
        {
          $sortcols[] = array("content"=>"");
        }
        if ($orientation == "left" || $orientation == "both")
        {
          $sortcols[] = array("content"=>$button);
        }

        foreach (array_keys($list["heading"]) as $key)
        {
          if (isset($list["sort"][$key])) $sortcols[] = array("content"=>$list["sort"][$key]);
        }

        if ($orientation == "right" || $orientation == "both")
        {
          $sortcols[] = array("content"=>$button);
        }
      }

      /**************/
      /* SEARCH ROW */
      /**************/

      $searchcols = array();
      $searchstart=""; 
      $searchend="";
      if (!hasFlag($flags, RL_NO_SEARCH))
      {
        $call = htmlentities($grid->getUpdateCall(array('atkstartat' => 0)));
        $button = '<input type="submit" class="btn_search" value="'.atktext("search").'" onclick="'.$call.' return false;">';
        if (!hasFlag($flags, RL_NO_EXTENDED_SEARCH)&&!$grid->getNode()->hasFlag(NF_NO_EXTENDED_SEARCH))
        {
          $button .= '<br>'.href(atkSelf()."?atknodetype=".$this->getMasterNodeType()."&atkaction=".$node->getExtendedSearchAction(),"(".atktext("search_extended").")",SESSION_NESTED);
        }

       // $searchstart = '<a name="searchform"></a>';
       $searchstart = "";

        if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA))
        {
          $searchcols[] = array("content"=>"");
        }
        if (hasFlag($flags, RL_LOCK))
        {
          $searchcols[] = array("content"=>"");
        }
        if ($orientation == "left" || $orientation == "both")
        {
          $searchcols[] = array("content"=>$button);
        }

        foreach (array_keys($list["heading"]) as $key)
        {
          if (isset($list["search"][$key]))
          {
            $searchcols[] = array("content"=>$list["search"][$key]);
          }
          else
          {
            $searchcols[] = array("content"=>"");
          }
        }
        if ($orientation == "right" || $orientation == "both")
        {
          $searchcols[] = array("content"=>$button);
        }
      }

      /*******************************************/
      /* MULTI-RECORD-(PRIORITY-)ACTIONS FORM DATA */
      /*******************************************/
      $liststart = "";
      $listend = "";
      if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA))
      {
        $page->register_script(atkconfig("atkroot")."atk/javascript/formselect.js");

        if (hasFlag($flags, RL_MRA))
        {
          $liststart.= '<script language="javascript" type="text/javascript">var '.$listName.' = new Object();</script>';
        }
      }

      /********/
      /* ROWS */
      /********/

      $records = array();
      $keys = array_keys($actions);
      $actionurl = (count($actions)>0) ? $actions[$keys[0]] : '';
      $actionloader = "rl_a['".$listName."'] = {};";
      $actionloader.= "\nrl_a['".$listName."']['base'] = '".session_vars($this->m_actionSessionStatus,1,$actionurl)."';";
      $actionloader.= "\nrl_a['".$listName."']['embed'] = ".(hasFlag($flags, RL_EMBED)?'true':'false').";";

      if (isset($navigation["next"]) && isset($navigation["next"]["url"]))
      {
        $actionloader.="\nrl_a['".$listName."']['next'] = '".$navigation["next"]["url"]."';";
      }
      if (isset($navigation["previous"]) && isset($navigation["previous"]["url"]))
      {
        $actionloader.="\nrl_a['".$listName."']['previous'] = '".$navigation["previous"]["url"]."';";
      }

      for ($i = 0, $_i = count($list["rows"]); $i < $_i; $i++)
      {
        $record = array();

        /* Special rowColor method makes it possible to change the row color based on the record data.
         * the method can return a simple value (which will be used for the normal row color), or can be
         * an array, in which case the first element will be the normal row color, and the second the mouseover
         * row color, example: function rowColor(&$record, $num) { return array('red', 'blue'); }
         */
        $method = "rowColor";
        $bgn="";
        $bgh = $defaulthighlight;
        if (method_exists($grid->getNode(), $method))
        {
          $bgn = $grid->getNode()->$method($recordset[$i], $i);
          if (is_array($bgn)) list($bgn, $bgh) = $bgn;
        }


        /* alternate colors of rows */
        $record["background"] = $bgn;
        $record["highlight"] = $bgh;
        $record["rownum"] = $i;
        $record["id"] = $listName.'_'.$i;
        $record["type"] = $list["rows"][$i]["type"];

        /* multi-record-priority-actions -> priority selection */
        if (hasFlag($flags, RL_MRPA))
        {
          $select = '<select name="'.$listName.'_atkselector[]">'.
                      '<option value="'.rawurlencode($list["rows"][$i]["selector"]).'"></option>';
          for ($j = $grid->getNode()->m_priority_min; $j <= $grid->getNode()->m_priority_max; $j++)
            $select .= '<option value="'.$j.'">'.$j.'</option>';
          $select .= '</select>';
          $record["cols"][] = array("content" => $select, "type" => "mrpa");
        }

        /* multi-record-actions -> checkbox */
        elseif (hasFlag($flags, RL_MRA))
        {
          if (count($list["rows"][$i]["mra"]) > 0)
            $record["cols"][] = array(
                         "content"=>'<input type="checkbox" name="'.$listName.'_atkselector[]" value="'.rawurlencode($list["rows"][$i]["selector"]).'" class="atkcheckbox" onclick="if (this.disabled) this.checked = false">'.
                                    '<script language="javascript"  type="text/javascript">'.$listName.'["'.rawurlencode($list["rows"][$i]["selector"]).'"] = new Array("'.implode($list["rows"][$i]["mra"], '","').'");</script>',
                          "type" => "mra");

          else $record["cols"][] = array("content"=>"");
        }

        /* locked? */
        if (hasFlag($flags, RL_LOCK))
        {
          if (is_array($list["rows"][$i]["lock"]))
          {
            $alt = $list["rows"][$i]["lock"]["user_id"]." / ".$list["rows"][$i]["lock"]["user_ip"];
            $record["cols"][] = array("content"=>'<img src="'.atkconfig("atkroot").'atk/images/lock.gif" alt="'.$alt.'" border="0">', "type" => "lock");
          }
          else $record["cols"][] = array("content"=>"");
        }

        $str_actions = "<span class=\"actions\">";
        $actionloader.="\nrl_a['".$listName."'][".$i."] = {};";
        $icons = (atkconfig('recordlist_icons', $theme->getAttribute("recordlist_icons"))===false ||
                  atkconfig('recordlist_icons', $theme->getAttribute("recordlist_icons"))==='false'?false:true);

        foreach ($list["rows"][$i]["actions"] as $name => $url)
        {
          if (substr($url, 0, 11) == 'javascript:')
          {
            $call = substr($url, 11);
            $actionloader.="\nrl_a['{$listName}'][{$i}]['{$name}'] = function() { $call; };";
          }
          else
          {
            $actionloader.="\nrl_a['{$listName}'][{$i}]['{$name}'] = '$url';";
          }

          $link = $name;

          if($icons == true)
          {
            $icon = $theme->iconPath(strtolower($name),"recordlist", $grid->getNode()->m_module);
            $link = sprintf('<img class="recordlist" border="0" src="%1$s" alt="%2$s" title="%2$s">', $icon, atktext($name, $grid->getNode()->m_module, $grid->getNode()->m_type));
          }
          else
          {
            $link = atktext($name, $grid->getNode()->m_module, $grid->getNode()->m_type);
          }

          $confirmtext= "false";
          if (atkconfig("recordlist_javascript_delete") && $name=="delete")
          $confirmtext = "'".$grid->getNode()->confirmActionText($name)."'";
          $str_actions.='<a href="'."javascript:rl_do('$listName',$i,'$name',$confirmtext);".'">'.$link.'</a>&nbsp;';
        }

        $str_actions.="</span>";
        /* actions (left) */
        if ($orientation == "left" || $orientation == "both")
        {
          if (!empty($list["rows"][$i]["actions"]))
          {
            $record["cols"][] = array("content"=>$str_actions, "type" => "actions");
          }
          else if ($this->_hasActionColumn($list))
          {
            $record["cols"][] = array("content"=>"");
          }
        }

        /* columns */
        foreach ($list["rows"][$i]["data"] as $html)
          $record["cols"][] = array("content"=>$html, "type" => "data");

        /* actions (right) */
        if ($orientation=="right"||$orientation=="both")
        {
          if (!empty($list["rows"][$i]["actions"])) $record["cols"][] = array("content"=>$str_actions, "type" => "actions");
          else if ($this->_hasActionColumn($list))
          {
            $record["cols"][] = array("content"=>"");
          }
        }

        $records[] = $record;

      }

      $page->register_loadscript($actionloader);
      $this->m_actionloader = $actionloader;

      /*************/
      /* TOTAL ROW */
      /*************/
      $totalcols = array();

      if (count($list["total"]) > 0)
      {
        if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA)) $totalcols[] = array("content"=>"");
        if (hasFlag($flags, RL_LOCK)) $totalcols[] = array("content"=>"");
        if (($orientation == "left" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0))
          $totalcols[] = array("content"=>"");

        foreach (array_keys($list["heading"]) as $key)
        {
          $totalcols[] = array("content"=>(isset($list["total"][$key])?$list["total"][$key]:""));
        }

        if (($orientation == "right" || $orientation == "both") && ($this->_hasActionColumn($list) && count($list["rows"]) > 0))
         $totalcols[] = array("content"=>"");
      }

      /*************************************************/
      /* MULTI-RECORD-PRIORITY-ACTION FORM (CONTINUED) */
      /*************************************************/
      $mra = "";
      if (hasFlag($flags, RL_MRPA))
      {
        $target = session_url(atkSelf().'?atknodetype='.$this->getMasterNodeType(), SESSION_NESTED);

        /* multiple actions -> dropdown */
        if (count($grid->getNode()->m_priority_actions) > 1)
        {
          $mra = '<select name="'.$listName.'_atkaction">'.
                       '<option value="">'.atktext("with_selected").':</option>';

          foreach ($grid->getNode()->m_priority_actions as $name)
            $mra .= '<option value="'.$name.'">'.atktext($name).'</option>';

          $mra .= '</select>&nbsp;'.$this->getCustomMraHtml().
                     '<input type="button" class="btn" value="'.atktext("submit").'" onclick="atkSubmitMRPA(\''.$listName.'\', this.form, \''.$target.'\')">';
        }

        /* one action -> only the submit button */
        else
        {
          $mra= $this->getCustomMraHtml().'<input type="hidden" name="'.$listName.'_atkaction" value="'.$grid->getNode()->m_priority_actions[0].'">'.
                     '<input type="button" class="btn" value="'.atktext($grid->getNode()->m_priority_actions[0]).'" onclick="atkSubmitMRPA(\''.$listName.'\', this.form, \''.$target.'\')">';
        }
      }


      /****************************************/
      /* MULTI-RECORD-ACTION FORM (CONTINUED) */
      /****************************************/
      elseif (hasFlag($flags, RL_MRA))
      {
        $target = session_url(atkSelf().'?atknodetype='.$grid->getNode()->atkNodeType().'&atktarget='.$grid->getNode()->m_postvars['atktarget'].'&atktargetvar='.$grid->getNode()->m_postvars['atktargetvar'].'&atktargetvartpl='.$grid->getNode()->m_postvars['atktargetvartpl'], SESSION_NESTED);

        $mra = (count($list["rows"]) > 1 ?
                    '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'all\')">'.atktext("select_all").'</a> / '.
                    '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'none\')">'.atktext("deselect_all").'</a> / '.
                    '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'invert\')">'.atktext("select_invert").'</a> '
                   :
                    '');

        /* multiple actions -> dropdown */
        if (count($list["mra"]) > 1)
        {
          $mra .= '<select name="'.$listName.'_atkaction" onchange="javascript:updateSelectable(\''.$listName.'\', this.form)">'.
                     '<option value="">'.atktext("with_selected").':</option>';

          foreach ($list["mra"] as $name)
          {
            if ($grid->getNode()->allowed($name))
            {
              $mra .= '<option value="'.$name.'">'.atktext($name, $grid->getNode()->m_module, $grid->getNode()->m_type).'</option>';
            }
          }

          $mra .= '</select>&nbsp;'.$this->getCustomMraHtml().
                     '<input type="button" class="btn" value="'.atktext("submit").'" onclick="atkSubmitMRA(\''.$listName.'\', this.form, \''.$target.'\')">';
        }

        /* one action -> only the submit button */
        else
        {
          if ($grid->getNode()->allowed($list["mra"][0]))
          {
            $mra .= '&nbsp; <input type="hidden" name="'.$listName.'_atkaction" value="'.$list["mra"][0].'">'.
            $this->getCustomMraHtml().
                       '<input type="button" class="btn" value="'.atktext($list["mra"][0],$grid->getNode()->m_module, $grid->getNode()->m_type).'" onclick="atkSubmitMRA(\''.$listName.'\', this.form, \''.$target.'\')">';
          }
        }
      }

      if (atkconfig("use_keyboard_handler"))
      {
        $kb = &atkKeyboard::getInstance();
        $kb->addRecordListHandler($listName, $selectcolor, count($records));
      }

      $recordListData = array("vorientation"=>$vorientation,
                              "rows"=>$records,
                              "header"=>$headercols,
                              "search"=>$searchcols,
                              "sort"=>$sortcols,
                              "total"=>$totalcols,
                              "searchstart"=>$searchstart,
                              "searchend"=>$searchend,
                              "sortstart"=>$sortstart,
                              "sortend"=>$sortend,
                              "liststart"=>$liststart,
                              "listend"=>$listend,
                              "listid"=>$listName,
                              "mra"=>$mra);

      return $recordListData;
    }
  }

  /**
   * Checks wether the recordlist should display a column which holds the actions.
   *
   * @access private
   * @param Array $list The recordlist data
   * @return bool Wether the list should display an extra column to hold the actions
   */
  function _hasActionColumn($list)
  {
    if($this->m_hasActionColumn == 0)
    {
      // when there's a search bar, we always need an extra column (for the button)
      if (!hasFlag($this->m_flags, RL_NO_SEARCH))
      {
        $this->m_hasActionColumn = true;
      }
      // when there's an extended sort bar, we also need the column (for the sort button)
      else if (hasFlag($this->m_flags, RL_EXT_SORT))
      {
        $this->m_hasActionColumn = true;
      }
      else
      {
        // otherwise, it depends on whether one of the records has actions defined.
        $this->m_hasActionColumn = false;

        foreach ($list["rows"] as $record)
        {
          if (!empty($record['actions']))
          {
            $this->m_hasActionColumn = true;
            break;
          }
        }
      }
    }
    return $this->m_hasActionColumn;
  }

  function getCustomMraHtml()
  {
    $grid = $this;
    if (method_exists($grid->getNode(), "getcustommrahtml"))
    {
      $output = $grid->getNode()->getCustomMraHtml();
      return $output;
    }
  }  

  /**
   * Function outputs an array with all information necessary to output a recordlist.
   *
   * @param Array   $recordset    List of records that need to be displayed
   * @param Integer $flags        Recordlist flags
   * @param String  $prefix       Prefix for each column name (used for subcalls)
   * @param Array   $actions      List of default actions for each record
   * @param Array   $suppress     An array of fields that you want to hide
   * @param String  $embedprefix  The prefix for embeded fields
   *
   * The result array contains the following information:
   *  "heading"  => for each visible column an array containing: "title" {, "url"}
   *  "search"   => for each visible column HTML input field(s) for searching
   *  "rows"     => list of rows, per row: "data", "actions", "mra", "record"
   *  "totalraw" => for each totalisable column the sum value field(s) (raw)
   *  "total"    => for each totalisable column the sum value (display)
   *  "mra"      => list of all multi-record actions
   *
   * @return see above
   */
  function listArray(&$recordset, $flags=0, $prefix="", $actions=array(), $suppress=array(), $embedprefix="")
  {
    $grid = $this->getGrid();
    
    if (!is_array($suppress)) $suppress = array();
    $result = array("heading" => array(), "search" => array(), "rows" => array(),
                    "totalraw" => array(), "total" => array(), "mra" => array());

    $prefix = $this->getGrid()->getName().'_';

    $columnConfig = &$grid->getNode()->getColumnConfig();

    /* get the heading and search columns */
    $atksearchpostvar = isset($grid->getNode()->m_postvars["atksearch"]) ? $grid->getNode()->m_postvars["atksearch"] : null;
    if (!hasFlag($flags, RL_NO_SEARCH)) $grid->getNode()->setAttribSizes();
    foreach (array_keys($grid->getNode()->m_attribIndexList) as $r)
    {
      $name = $grid->getNode()->m_attribIndexList[$r]["name"];
      if (!in_array($name, $suppress))
      {
        $attribute = &$grid->getNode()->m_attribList[$name];
        $attribute->addToListArrayHeader($grid->getNode()->m_action, $result, $prefix, $flags, $atksearchpostvar, $columnConfig, $this->getGrid());
      }
    }

    /* actions array can contain multi-record-actions */
    if (count($actions) == 2 && count(array_diff(array_keys($actions), array("actions", "mra"))) == 0)
    {
      $mra = $actions["mra"];
      $actions = $actions["actions"];
    }
    else
      $mra = $grid->getNode()->hasFlag(NF_NO_DELETE) ? array() : array("delete");

    /* get the rows */
    for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
    {
      $result["rows"][$i] = array("columns" => array(), "actions" => $actions, "mra" => $mra, "record" => &$recordset[$i], "data" => array());
      $result["rows"][$i]["selector"] = $grid->getNode()->primaryKey($recordset[$i]);
      $result["rows"][$i]["type"]="data";
      $row = &$result["rows"][$i];

      /* locked */
      if (hasFlag($flags, RL_LOCK))
      {
        $result["rows"][$i]["lock"] = $grid->getNode()->m_lock->isLocked($result["rows"][$i]["selector"], $grid->getNode()->m_table);
        if (is_array($result["rows"][$i]["lock"]))
        {
          unset($row["actions"]["edit"]);
          unset($row["actions"]["delete"]);
          $row["mra"] = array();
        }
      }

      /* actions / mra */
      $grid->getNode()->recordActions($row["record"], $row["actions"], $row["mra"]);
      $result["mra"] = array_merge($result["mra"], $row["mra"]);
      foreach($row["actions"] as $name => $url)
      {
        if (!empty($url) && $grid->getNode()->allowed($name, $row["record"]))
        {
          /* dirty hack */
          $atkencoded = strpos($url, "_15B") > 0;

          $url = str_replace("%5B","[",     $url);
          $url = str_replace("%5D","]",     $url);
          $url = str_replace("_1"."5B","[", $url);
          $url = str_replace("_1"."5D","]", $url);

          if ($atkencoded) $url = str_replace('[pk]', atkurlencode(rawurlencode($row["selector"]), false), $url);
          else $url = str_replace('[pk]', rawurlencode($row["selector"]), $url);

          $parser = new atkStringParser($url);
          $url = $parser->parse($row["record"],true);
          $row["actions"][$name] = $url;
        }
        else
        {
          unset($row["actions"][$name]);
        }
      }

      /* columns */
      foreach (array_keys($grid->getNode()->m_attribIndexList) as $r)
      {
        $name = $grid->getNode()->m_attribIndexList[$r]["name"];
        if (!in_array($name, $suppress))
        {
          $attribute = &$grid->getNode()->m_attribList[$name];
          $attribute->addToListArrayRow($grid->getNode()->m_action, $result, $i, $prefix, $flags);
        }
      }
    }

    if (hasFlag($flags, RL_EXT_SORT) && $columnConfig->hasSubTotals())
    {
      atkimport("atk.recordlist.atktotalizer");
      $totalizer = &new atkTotalizer($grid->getNode(), $columnConfig);
      $result["rows"] = $totalizer->totalize($result["rows"]);
    }

    if (hasFlag($flags, RL_MRA))
      $result["mra"] = array_values(array_unique($result["mra"]));

    return $result;
  }  
  

  function getMasterNode()
  {
    if (is_object($this->m_masternode)) return $this->m_masternode;
    return $this->getGrid()->getNode(); // treat rendered node as master
  }

  function getMasterNodeType()
  {
    $node = &$this->getMasterNode();
    return $node->atkNodeType();
  }  
}
?>