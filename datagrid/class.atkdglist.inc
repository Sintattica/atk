<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @copyright (c)2000-2006 Ibuildings.nl BV
 * @copyright (c)2000-2006 Ivo Jansch
 * 
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 */

atkimport('atk.datagrid.atkdgcomponent');

/**
 * The data grid is used to render tables containing records.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class atkDGList extends atkDGComponent
{
  /**
   * Renders the recordlist for the given data grid.
   *
   * @param atkDataGrid $grid the data grid
   * @return string rendered HTML
   */
  function render(&$grid)
  {
    $baseUrlData = parse_url($baseUrl);
    $baseQuery = $baseUrlData['query'];
    
    if (!hasFlag($this->m_flags, RL_EMBED))
    {
      if (empty($formName)) 
        $formName = $listName;
        
      $formstart = 
        '<form name="'.$formName.'" method="post">'.
        session_form(SESSION_DEFAULT).
        '<input type="hidden" name="atknodetype" value="'.$this->m_node->atkNodeType().'">'.
        '<input type="hidden" name="atkaction" value="'.$this->m_node->m_action.'">'.
        '<input type="hidden" name="atkstartat" value="0">'; // reset atkstartat to first page          
        
      $formend = '</form>';       
    }      

    $theme = &atkinstance("atk.ui.atktheme");
    $page = &atkPage::getInstance();
    $page->register_style($theme->stylePath("recordlist.css",$this->m_node->m_module));

    if (empty($listName))
      $listName = "rl_".getUniqueId("normalRecordList");
      
    $page->register_script(atkconfig("atkroot")."atk/javascript/recordlist.js");
    $page->register_script(atkconfig("atkroot")."atk/javascript/prototype/prototype.js");
    $page->register_script(atkconfig("atkroot")."atk/javascript/atkbusy.js");      

    $defaulthighlight = $theme->getAttribute("highlight");
    $selectcolor = $theme->getAttribute("select");

    /* retrieve list array */
    $list = $this->listArray($listName, $recordset, $flags, "", $actions, $suppressList, $embedprefix);

    /* Check if some flags are still valid or not... */
    if (hasFlag($flags, RL_MRA) && (count($list["mra"]) == 0 || count($list["rows"]) == 0)) $flags ^= RL_MRA;
    if (!hasFlag($flags, RL_NO_SEARCH) && count($list["search"]) == 0) $flags |= RL_NO_SEARCH;
    if (hasFlag($flags, RL_MRPA) && (count($this->m_node->m_priority_actions) == 0 || count($list["rows"]) == 0)) $flags ^= RL_MRPA;
    elseif (hasFlag($flags, RL_MRPA))
    {
      $flags = ($flags | RL_MRA | RL_MRPA ) ^ RL_MRA;
      if ($this->m_node->m_priority_max == 0)
        $this->m_node->m_priority_max = $this->m_node->m_priority_min + count($list["rows"]) - 1;
    }

    $orientation  = atkconfig('recordlist_orientation', $theme->getAttribute("recordlist_orientation"));
    $vorientation = trim(atkconfig('recordlist_orientation', $theme->getAttribute("recordlist_vorientation")));

    $ui = &$this->m_node->getUi();

    if (is_object($ui) && is_object($page))
    {

      /**************/
      /* HEADER ROW */
      /**************/
      $headercols = array();

      if (!hasFlag($flags, RL_NO_SEARCH) && count($list["rows"]) == 0)
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
      if (($orientation == "left" || $orientation == "both") && $this->hasActionsinList($list))
      {
        $headercols[] = array("content"=>"");
      }

      foreach (array_values($list["heading"]) as $head)
      {
        if (hasFlag($this->m_flags, RL_EMBED) && !empty($head["url"]))
        {
          $head["url"] = str_replace("atkorderby=","atkorderby{$embedprefix}=",$head["url"]);
        }

        if (empty($head["order"]))
        {
          $headercols[] = array("content"=>$head["title"]);
        }
        else
        {
          $url = $baseUrl;
          $order = rawurlencode($head["order"]);
          $onclick = "new Ajax.Updater('{$listName}_container', '$url', {method: 'post', parameters: Form.serialize('$formName') + '&{$baseQuery}&atkorderby={$order}', evalScripts: true, asynchronous: true })";
          $href = '<a href="#" onclick="'.$onclick.'">'.$head["title"].'</a>';
          $headercols[] = array("content"=>$href);
        }
      }

      if (($orientation == "right" || $orientation == "both") && $this->hasActionsinList($list))
      {
        $headercols[] = array("content"=>"");
      }

      if (!hasFlag($flags, RL_NO_SEARCH) && count($list["rows"]) == 0)
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
      $sortstart=""; $sortend="";
      if (hasFlag($flags, RL_EXT_SORT))
      {
        $button = '<input type="submit" value="'.atktext("sort").'">';
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

        //$sortstart = '<a name="sortform"></a>';

        foreach (array_keys($list["heading"]) as $key)
        {
          if (isset($list["sort"][$key])) $sortcols[] = array("content"=>$list["sort"][$key]);
        }

        $sortend = '';

        if ($orientation == "right" || $orientation == "both")
        {
          $sortcols[] = array("content"=>$button);
        }
      }

      /**************/
      /* SEARCH ROW */
      /**************/

      $searchcols = array();
      $searchstart=""; $searchend="";
      if (!hasFlag($flags, RL_NO_SEARCH))
      {
        $url = $baseUrl;
        $onClick = "new Ajax.Updater('{$listName}_container', '$url', {method: 'post', parameters: Form.serialize(this.form) + '&{$baseQuery}', evalScripts: true, asynchronous: true })";
        $button = '<input type="submit" class="btn_search" value="'.atktext("search").'" onclick="'.$onClick.'; return false">';

        if (!hasFlag($flags, RL_NO_EXTENDED_SEARCH)&&!$this->m_node->hasFlag(NF_NO_EXTENDED_SEARCH))
        {
          $button .= '<br>'.href($_SERVER["PHP_SELF"]."?atknodetype=".$this->getMasterNodeType()."&atkaction=".$node->getExtendedSearchAction(),"(".atktext("search_extended").")",SESSION_NESTED);
        }

        //$searchstart = '<a name="searchform"></a>';
        if (!hasFlag($this->m_flags, RL_EMBED))
        {
          $formstart.= '<input type="hidden" name="atksearchmode" value="'.$searchmode.'">';
        }

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

        $searchend = "";
        if (!hasFlag($this->m_flags, RL_EMBED)) $searchend = '';
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
      $actionloader = "rl_a['".$listName."'] = new Array();";
      $actionloader.= "\nrl_a['".$listName."']['base'] = '".session_vars(SESSION_NESTED,1,$actionurl)."';";
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
        if (method_exists($this->m_node, $method))
        {
          $bgn = $this->m_node->$method($recordset[$i], $i);
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
          $select = '<select name="'.$listName.'_atkselector['.$i.']">'.
                      '<option value="'.rawurlencode($list["rows"][$i]["selector"]).'"></option>';
          for ($j = $this->m_node->m_priority_min; $j <= $this->m_node->m_priority_max; $j++)
            $select .= '<option value="'.$j.'">'.$j.'</option>';
          $select .= '</select>';
          $record["cols"][] = array("content" => $select, "type" => "mrpa");
        }

        /* multi-record-actions -> checkbox / radiobutton */
        elseif (hasFlag($flags, RL_MRA))
        {
          if (count($list["rows"][$i]["mra"]) > 0)
          {
            $inputHTML = '';
            
            switch($mraSelectionMode)
            {
              case MRA_SINGLE_SELECT:
                // no atkselector array index, will be added later on using JavaScript
                $inputHTML = '<input type="radio" name="'.$listName.'_atkselector" value="'.$list["rows"][$i]["selector"].'" onclick="if (this.disabled) this.checked = false">';                
                break;
              case MRA_NO_SELECT:
                $inputHTML = 
                  '<input type="checkbox" disabled="disabled" checked="checked">'.
                  '<input type="hidden" name="'.$listName.'_atkselector['.$i.']" value="'.$list["rows"][$i]["selector"].'">';
                break;
              case MRA_MULTI_SELECT:
              default:
                $inputHTML = '<input type="checkbox" name="'.$listName.'_atkselector['.$i.']" value="'.$list["rows"][$i]["selector"].'" onclick="if (this.disabled) this.checked = false">';                
            }              
            
            $record["cols"][] = array("content"=>$inputHTML.'<script language="javascript"  type="text/javascript">'.$listName.'["'.$list["rows"][$i]["selector"].'"] = new Array("'.implode($list["rows"][$i]["mra"], '","').'");</script>', "type" => "mra");
          }
          else $record["cols"][] = array("content"=>"", "type" => "mra");
        }

        /* locked? */
        if (hasFlag($flags, RL_LOCK))
        {
          if (is_array($list["rows"][$i]["lock"]))
          {
            $alt = $list["rows"][$i]["lock"]["user_id"]." / ".$list["rows"][$i]["lock"]["user_ip"];
            $record["cols"][] = array("content"=>'<img src="'.atkconfig("atkroot").'atk/images/lock.gif" alt="'.$alt.'" border="0">', "type" => "lock");
          }
          else $record["cols"][] = array("content"=>"", "type" => "lock");
        }

        $str_actions = "<span class=\"actions\">";
        $actionloader.="\nrl_a['".$listName."'][".$i."] = new Array();";
        $icons = (atkconfig('recordlist_icons', $theme->getAttribute("recordlist_icons"))===false ||
                  atkconfig('recordlist_icons', $theme->getAttribute("recordlist_icons"))==='false'?false:true);

        foreach ($list["rows"][$i]["actions"] as $name => $url)
        {
          $actionloader.="\nrl_a['".$listName."'][".$i."]['".$name."'] = '".$url."';";

          $link = $name;

          if($icons == true)
          {
            $icon = $theme->iconPath(strtolower($name),"recordlist", $this->m_node->m_module);
            $link = sprintf('<img width="16" height="16" border="0" src="%1$s" alt="%2$s" title="%2$s">', $icon, atktext($name, $this->m_node->m_module, $this->m_node->m_type));
          }
          else
          {
            $link = atktext($name, $this->m_node->m_module, $this->m_node->m_type);
          }

          $confirmtext= "false";
          if (atkconfig("recordlist_javascript_delete") && $name=="delete")
          $confirmtext = "'".$this->m_node->confirmActionText($name)."'";
          $str_actions.='<a href="'."javascript:rl_do('$listName',$i,'$name',$confirmtext);".'">'.$link.'</a>&nbsp;';
        }

        $str_actions.="</span>";
        /* actions (left) */
        if ($orientation == "left" || $orientation == "both")
        {
          if (!empty($list["rows"][$i]["actions"]))
            $record["cols"][] = array("content"=>$str_actions, "type" => "actions");
          //else $record["cols"][] = array("content"=>"", "type" => "actions");
        }

        /* columns */
        foreach ($list["rows"][$i]["data"] as $html)
          $record["cols"][] = array("content"=>$html, "type" => "data");

        /* actions (right) */
        if ($orientation=="right"||$orientation=="both")
        {
          if (!empty($list["rows"][$i]["actions"])) 
            $record["cols"][] = array("content"=>$str_actions, "type" => "actions");
          //else $record["cols"][] = array("content"=>"", "type" => "actions");
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
        if ($orientation == "left" || $orientation == "both") $totalcols[] = array("content"=>"");

        foreach (array_keys($list["heading"]) as $key)
        {
          $totalcols[] = array("content"=>(isset($list["total"][$key])?$list["total"][$key]:""));
        }

        if (($orientation == "right" || $orientation == "both") && $this->hasActionsinList($list))
         $totalcols[] = array("content"=>"");
      }

      /*************************************************/
      /* MULTI-RECORD-PRIORITY-ACTION FORM (CONTINUED) */
      /*************************************************/
      $mra = "";
      if (hasFlag($flags, RL_MRPA))
      {
        $target = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->getMasterNodeType(), SESSION_NESTED);

        /* multiple actions -> dropdown */
        if (count($this->m_node->m_priority_actions) > 1)
        {
          $mra = '<select name="'.$listName.'_atkaction">'.
                       '<option value="">'.atktext("with_selected").':</option>';

          foreach ($this->m_node->m_priority_actions as $name)
            $mra .= '<option value="'.$name.'">'.atktext($name).'</option>';

          $mra .= '</select>&nbsp;'.$this->getCustomMraHtml().
                     '<input type="button" class="btn" value="'.atktext("submit").'" onclick="atkSubmitMRPA(\''.$listName.'\', this.form, \''.$target.'\')">';
        }

        /* one action -> only the submit button */
        else
        {
          $mra= $this->getCustomMraHtml().'<input type="hidden" name="'.$listName.'_atkaction" value="'.$this->m_node->m_priority_actions[0].'">'.
                     '<input type="button" class="btn" value="'.atktext($this->m_node->m_priority_actions[0]).'" onclick="atkSubmitMRPA(\''.$listName.'\', this.form, \''.$target.'\')">';
        }
      }

      /****************************************/
      /* MULTI-RECORD-ACTION FORM (CONTINUED) */
      /****************************************/
      elseif (hasFlag($flags, RL_MRA))
      {
        $target = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->m_node->atkNodeType(), SESSION_NESTED);
        $embedded = hasFlag($this->m_flags, RL_EMBED) ? 'true' : 'false';

        $mra = (count($list["rows"]) > 1 && $mraSelectionMode != MRA_SINGLE_SELECT && $mraSelectionMode != MRA_NO_SELECT ?
                    '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'all\')">'.atktext("select_all").'</a> / '.
                    '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'none\')">'.atktext("deselect_all").'</a> / '.
                    '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'invert\')">'.atktext("select_invert").'</a>&nbsp;'
                   :
                    '');

        /* multiple actions -> dropdown */
        if (count($list["mra"]) > 1)
        {
          $mra .= '<select name="'.$listName.'_atkaction" onchange="javascript:updateSelectable(\''.$listName.'\', this.form)">'.
                     '<option value="">'.atktext("with_selected").':</option>';

          foreach ($list["mra"] as $name)
          {
            if ($this->m_node->allowed($name))
            {
              $mra .= '<option value="'.$name.'">'.atktext($name, $this->m_node->m_module, $this->m_node->m_type).'</option>';
            }
          }

          $mra .= '</select>&nbsp;'.$this->getCustomMraHtml().
                     '<input type="button" class="btn" value="'.atktext("submit").'" onclick="atkSubmitMRA(\''.$listName.'\', this.form, \''.$target.'\', '.$embedded.')">';
        }

        /* one action -> only the submit button */
        else
        {
          if ($this->m_node->allowed($list["mra"][0]))
          {
            $mra .= '&nbsp; <input type="hidden" name="'.$listName.'_atkaction" value="'.$list["mra"][0].'">'.
            $this->getCustomMraHtml().
                       '<input type="button" class="btn" value="'.atktext($list["mra"][0],$this->m_node->m_module, $this->m_node->m_type).'" onclick="atkSubmitMRA(\''.$listName.'\', this.form, \''.$target.'\')">';
          }
        }
      }

      if (atkconfig("use_keyboard_handler"))
      {
        $kb = &atkKeyboard::getInstance();
        $kb->addRecordListHandler($listName, $selectcolor, count($records));
      }

      $res = $ui->render("recordlist.tpl", array("vorientation"=>$vorientation,
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
                                                 "formstart"=>$formstart,
                                                 "formend"=>$formend,
                                                 "mra"=>$mra),$this->m_node->m_module);

      return $res;
    }
  }

  /**
   * Checks wether there are any actions in the recordlist data
   *
   * @param Array $list The recordlist data
   * @return bool Wether the list has actions
   */
  function hasActionsInList($list)
  {
    foreach ($list["rows"] as $record)
    {
      if (!empty($record['actions'])) return true;
    }
    return false;
  }

  function getCustomMraHtml()
  {
    if (method_exists($this->m_node, "getcustommrahtml"))
    {
      $output = $this->m_node->getCustomMraHtml();
      return $output;
    }
  }

  /**
   * Function outputs an array with all information necessary to output a recordlist.
   *
   * @param String  $name         The recordlist name
   * @param Array   $recordset    List of records that need to be displayed
   * @param Integer $flags        Recordlist flags
   * @param String  $prefix       Prefix for each column name (used for subcalls)
   * @param Array   $actions      List of default actions for each record
   * @param Array   $suppress     An array of fields that you want to hide
   * @param String  $embedprefix  The prefix for embeded fields
   *
   * The result array contains the following information:
   *  "name"     => the recordlist name
   *  "heading"  => for each visible column an array containing: "title" {, "url"}
   *  "search"   => for each visible column HTML input field(s) for searching
   *  "rows"     => list of rows, per row: "data", "actions", "mra", "record"
   *  "totalraw" => for each totalisable column the sum value field(s) (raw)
   *  "total"    => for each totalisable column the sum value (display)
   *  "mra"      => list of all multi-record actions
   *
   * @return see above
   */
  function listArray($name, &$recordset, $flags=0, $prefix="", $actions=array(), $suppress=array(), $embedprefix="")
  {
    if (!is_array($suppress)) $suppress = array();
    $result = array("name" => $name, "heading" => array(), "search" => array(), "rows" => array(),
                    "totalraw" => array(), "total" => array(), "mra" => array());

    if (hasFlag($this->m_flags, RL_EMBED) && $embedprefix)
    {
      $prefix = $embedprefix."][";
    }

    $columnConfig = &$this->m_node->getColumnConfig();

    /* get the heading and search columns */
    $atksearchpostvar = isset($this->m_node->m_postvars["atksearch"]) ? $this->m_node->m_postvars["atksearch"] : null;
    if (!hasFlag($flags, RL_NO_SEARCH)) $this->m_node->setAttribSizes();
    foreach (array_keys($this->m_node->m_attribIndexList) as $r)
    {
      $name = $this->m_node->m_attribIndexList[$r]["name"];
      if (!in_array($name, $suppress))
      {
        /* @var $attribute atkAttribute */
        $attribute = &$this->m_node->m_attribList[$name];
        $attribute->addToListArrayHeader($this->m_node->m_action, $result, $prefix, $flags, $atksearchpostvar, $columnConfig);
      }
    }

    /* actions array can contain multi-record-actions */
    if (count($actions) == 2 && count(array_diff(array_keys($actions), array("actions", "mra"))) == 0)
    {
      $mra = $actions["mra"];
      $actions = $actions["actions"];
    }
    else
      $mra = $this->m_node->hasFlag(NF_NO_DELETE) ? array() : array("delete");

    /* get the rows */
    for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
    {
      $result["rows"][$i] = array("columns" => array(), "actions" => $actions, "mra" => $mra, "record" => &$recordset[$i], "data" => array());
      $result["rows"][$i]["selector"] = $this->m_node->primaryKey($recordset[$i]);
      $result["rows"][$i]["type"]="data";
      $row = &$result["rows"][$i];

      /* locked */
      if (hasFlag($flags, RL_LOCK))
      {
        $result["rows"][$i]["lock"] = $this->m_node->m_lock->isLocked($result["rows"][$i]["selector"], $this->m_node->m_table);
        if (is_array($result["rows"][$i]["lock"]))
        {
          unset($row["actions"]["edit"]);
          unset($row["actions"]["delete"]);
          $row["mra"] = array();
        }
      }

      /* actions / mra */
      $this->m_node->recordActions($row["record"], $row["actions"], $row["mra"]);
      foreach($row["actions"] as $name => $url)
      {
        if (!empty($url) && $this->m_node->allowed($name, $row["record"]))
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
      
      $tmpMRA = array();
      foreach($row["mra"] as $name)
      {
        if ($this->m_node->allowed($name, $row["record"]))
          $tmpMRA[] = $name;
      }
      $row["mra"] = $tmpMRA;
      
      $result["mra"] = array_merge($result["mra"], $row["mra"]);        

      /* columns */
      foreach (array_keys($this->m_node->m_attribIndexList) as $r)
      {
        $name = $this->m_node->m_attribIndexList[$r]["name"];
        if (!in_array($name, $suppress))
        {
          $attribute = &$this->m_node->m_attribList[$name];
          $attribute->addToListArrayRow($this->m_node->m_action, $result, $i, $prefix, $flags);
        }
      }
    }

    if (hasFlag($flags, RL_EXT_SORT) && $columnConfig->hasSubTotals())
    {
      atkimport("atk.recordlist.atktotalizer");
      $totalizer = &new atkTotalizer($this->m_node, $columnConfig);
      $result["rows"] = $totalizer->totalize($result["rows"]);
    }

    if (hasFlag($flags, RL_MRA))
      $result["mra"] = array_values(array_unique($result["mra"]));

    return $result;
  }
  
 /**
   * Function outputs an array with all information necessary to output a recordlist.
   *
   * @param String  $name         The recordlist name
   * @param Array   $recordset    List of records that need to be displayed
   * @param Integer $flags        Recordlist flags
   * @param String  $prefix       Prefix for each column name (used for subcalls)
   * @param Array   $actions      List of default actions for each record
   * @param Array   $suppress     An array of fields that you want to hide
   * @param String  $embedprefix  The prefix for embeded fields
   *
   * The result array contains the following information:
   *  "name"     => the recordlist name
   *  "heading"  => for each visible column an array containing: "title" {, "url"}
   *  "search"   => for each visible column HTML input field(s) for searching
   *  "rows"     => list of rows, per row: "data", "actions", "mra", "record"
   *  "totalraw" => for each totalisable column the sum value field(s) (raw)
   *  "total"    => for each totalisable column the sum value (display)
   *  "mra"      => list of all multi-record actions
   *
   * @return see above
   */
  function listArray($name, &$recordset, $flags=0, $prefix="", $actions=array(), $suppress=array(), $embedprefix="")
  {
    if (!is_array($suppress)) $suppress = array();
    $result = array("name" => $name, "heading" => array(), "search" => array(), "rows" => array(),
                    "totalraw" => array(), "total" => array(), "mra" => array());

    if (hasFlag($this->m_flags, DG_EMBED) && $embedprefix)
    {
      $prefix = $embedprefix."][";
    }

    $columnConfig = &$this->m_node->getColumnConfig();

    /* get the heading and search columns */
    $atksearchpostvar = isset($this->m_node->m_postvars["atksearch"]) ? $this->m_node->m_postvars["atksearch"] : null;
    if (!hasFlag($flags, DG_NO_SEARCH)) $this->m_node->setAttribSizes();
    foreach (array_keys($this->m_node->m_attribIndexList) as $r)
    {
      $name = $this->m_node->m_attribIndexList[$r]["name"];
      if (!in_array($name, $suppress))
      {
        /* @var $attribute atkAttribute */
        $attribute = &$this->m_node->m_attribList[$name];
        $attribute->addToListArrayHeader($this->m_node->m_action, $result, $prefix, $flags, $atksearchpostvar, $columnConfig);
      }
    }

    /* actions array can contain multi-record-actions */
    if (count($actions) == 2 && count(array_diff(array_keys($actions), array("actions", "mra"))) == 0)
    {
      $mra = $actions["mra"];
      $actions = $actions["actions"];
    }
    else
      $mra = $this->m_node->hasFlag(NF_NO_DELETE) ? array() : array("delete");

    /* get the rows */
    for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
    {
      $result["rows"][$i] = array("columns" => array(), "actions" => $actions, "mra" => $mra, "record" => &$recordset[$i], "data" => array());
      $result["rows"][$i]["selector"] = $this->m_node->primaryKey($recordset[$i]);
      $result["rows"][$i]["type"]="data";
      $row = &$result["rows"][$i];

      /* locked */
      if (hasFlag($flags, DG_LOCK))
      {
        $result["rows"][$i]["lock"] = $this->m_node->m_lock->isLocked($result["rows"][$i]["selector"], $this->m_node->m_table);
        if (is_array($result["rows"][$i]["lock"]))
        {
          unset($row["actions"]["edit"]);
          unset($row["actions"]["delete"]);
          $row["mra"] = array();
        }
      }

      /* actions / mra */
      $this->m_node->recordActions($row["record"], $row["actions"], $row["mra"]);
      foreach($row["actions"] as $name => $url)
      {
        if (!empty($url) && $this->m_node->allowed($name, $row["record"]))
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
      
      $tmpMRA = array();
      foreach($row["mra"] as $name)
      {
        if ($this->m_node->allowed($name, $row["record"]))
          $tmpMRA[] = $name;
      }
      $row["mra"] = $tmpMRA;
      
      $result["mra"] = array_merge($result["mra"], $row["mra"]);        

      /* columns */
      foreach (array_keys($this->m_node->m_attribIndexList) as $r)
      {
        $name = $this->m_node->m_attribIndexList[$r]["name"];
        if (!in_array($name, $suppress))
        {
          $attribute = &$this->m_node->m_attribList[$name];
          $attribute->addToListArrayRow($this->m_node->m_action, $result, $i, $prefix, $flags);
        }
      }
    }

    if (hasFlag($flags, DG_EXT_SORT) && $columnConfig->hasSubTotals())
    {
      atkimport("atk.recordlist.atktotalizer");
      $totalizer = &new atkTotalizer($this->m_node, $columnConfig);
      $result["rows"] = $totalizer->totalize($result["rows"]);
    }

    if (hasFlag($flags, DG_MRA))
      $result["mra"] = array_values(array_unique($result["mra"]));

    return $result;
  }   
}
?>