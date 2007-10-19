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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * This class creates record navigation for nodes.
   *
   * It can generate paging links, previous/next links and alphabetical
   * indexes.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */
  class atkNavigator
  {
    var $m_displayShowAll = true;

    function buildNavigation(&$node, $mode="", $navurls="", $embedded=false)
    {
      if ($navurls=="")
      {
        // Have to determine navurls ourselves.
        $navurls = $this->getNavigationUrls($node, $mode);
      }

  	  if(count($navurls)==0) return "";

      $links=array();
      foreach ($navurls as $key=>$nav)
      {
        $links[] = href($nav["url"], $nav["title"], SESSION_DEFAULT, $embedded);
      }
  	  return implode("&nbsp;|&nbsp;", $links)."&nbsp;".$this->getSummary($node, $mode);
    }

    function getSummary(&$node, $mode="")
    {
      $count = $this->getCount($node, $mode);
      $curpage = $this->getCurrentPage($node);
      $start = (($curpage-1)*atkconfig("recordsperpage"))+1;
      $end = min($start+atkconfig("recordsperpage")-1, $count);

      atkimport("atk.utils.atkstringparser");
      $str = &new atkStringParser(atktext("recordlistsummary", "atk"));
      return "(".$str->parse(array("start"=>$start, "end"=>$end, "count"=>$count)).") ";
    }

    function getCount(&$node, $mode="", $filter="")
    {
      static $s_count = array();

      $type = $node->atkNodeType();
      if (!isset($s_count[$type])) // rudimentary caching
      {
        if (!$filter && isset($node->m_postvars["atkfilter"])) $filter = $node->validateFilter($node->m_postvars["atkfilter"]);

        $s_count[$type] = (int)$node->countDb($filter, $node->m_listExcludes, "", $mode);
      }
      return $s_count[$type];
    }

    function getCurrentPage(&$node)
    {
      return ($node->m_postvars['atkstartat'] / $this->getLimit($node)) + 1;
    }

    function getLimit(&$node)
    {
      return (int)$node->m_postvars['atklimit'];
    }

    /**
     * Creates a navigation bar, for browsing through the record pages
     * (if a limit is set, and there are more records)
     * If an owner node is passed along, then we use the atkselector
     * on the owner as the navigator is used on a relation
     * @param obj   &$node   instance of the node we create the navigation bar for
     * @param string $mode   the mode we are in, defaults to ""
     * @param string $filter a filter for the destination node
     * @return array of HTML string for navigating through records
     */
    function getNavigationUrls(&$node, $mode="", $filter="")
    {
      global $ATK_VARS;

      $res = array();

      $count = $this->getCount($node, $mode, $filter);

      // maximum number of bookmarks to pages.
      $max_bm = 10;

      $limit = $this->getLimit($node);

      if (!($limit > 0 && $count > $limit && ceil($count / $limit) > 1)) return array();

      $pages = ceil($count / $limit);
      $curr  = $this->getCurrentPage($node);
      $begpg = $curr - floor(($max_bm-1) / 2);
      $endpg = $curr + ceil(($max_bm-1) / 2);

      if ($begpg < 1)
      {
        $begpg = 1;
        $endpg = min($pages, $max_bm);
      }

      if ($endpg > $pages)
      {
        $endpg = $pages;
        $begpg = max(1,$pages - $max_bm + 1);
      }

      // When we are editing a page and make an update, if afterwards we navigate
      // through something we're not updating any more so we set the action to edit
      if ($node->m_action == "update" || $node->m_action=="save") $action ="edit";
      else $action = $node->m_action;

      if ($curr > 1)
      {
        $newstart = $node->m_postvars['atkstartat'] - $limit;
        $res['previous'] = array( "title"=>atktext("previous", "atk"),
                                  "url"=>atkSelf().
                                          "?atknodetype=".$ATK_VARS["atknodetype"].
                                          "&atkaction=".$action.
                                          (isset($ATK_VARS["atktarget"])?"&atktarget=".rawurlencode($ATK_VARS["atktarget"]):"").
                                          "&atkstartat".(isset($node->m_postvars["limitPostfix"])?$node->m_postvars["limitPostfix"]:"")."=".$newstart);
      }

      for ($i = $begpg; $i <= $endpg; $i++)
      {
         if ($i==$curr) $res[$i] = array("title"=>"<b>$i</b>", "url"=>"");
         else $res[$i] = array("title"=>$i,
                               "url"=>atkSelf().
                                        "?atknodetype=".$ATK_VARS["atknodetype"].
                                        "&atkaction=".$action.
                                        "&atkstartat".(isset($node->m_postvars["limitPostfix"])?$node->m_postvars["limitPostfix"]:"")."=".max(0, ($i-1) * $limit).
                                        (isset($ATK_VARS["atkselector"])?"&atkselector=".rawurlencode($ATK_VARS['atkselector']):"").
                                        (isset($ATK_VARS["atktarget"])?"&atktarget=".rawurlencode($ATK_VARS["atktarget"]):""));
      }

      if ($curr < $pages)
      {
        $newstart = $node->m_postvars['atkstartat'] + $limit;
        $res['next'] = array("title"=>atktext("next", "atk"),
                              "url"=>atkSelf().
                                      "?atknodetype=".$ATK_VARS["atknodetype"].
                                      "&atkaction=".$node->m_action.
                                      "&atkstartat".(isset($node->m_postvars["limitPostfix"])?$node->m_postvars["limitPostfix"]:"")."=".$newstart.
                                      (isset($ATK_VARS["atktarget"])?"&atktarget=".rawurlencode($ATK_VARS["atktarget"]):""));
      }
      return $res;
    }

    /**
     * Creates an alphabetical index, for quick lookup of records.
     * @param selected The letter that is currently selected. (Can be
     *                 a string; this function only takes the first char
     *                 of the parameter.
     */
    function buildIndex(&$node, $selected="")
    {
      $nav = "";
      $available_letters = $this->getAvailableIndices($node);
      $count = count($available_letters);

      $atksearch = atkArrayNvl($node->m_postvars,'atksearch',"");
      $atkindex = atkArrayNvl($node->m_postvars,'atkindex',"");
      
       
      for ($i=0;$i<$count;$i++)
      {
        $char = $available_letters[$i];
        if ($atkindex!="" && $selected!="" && strtoupper($selected[0])==$char) $label="<b>$char</b>";
        else $label=$char;

        $nav .= atkHref(dispatch_url("{$node->m_module}.{$node->m_type}", $node->m_action,array("atkindex"=>$char."*","atkstartat"=>0)), $label);

        // uncomment the following line if 26 letters seems to become too wide.
        //if ($count>13 && ($i+1)==floor($count/2)) $nav.='<br>'; else
        if ($i!=$count-1) $nav.=" ";
      }

      if ($this->m_displayShowAll)
      {

        $view_all = 0;
        
        if($atkindex != "")
        {
          $clear = array("atkindex"=>"");
          $text = atktext("clear_index","atk");
          $nav.=" - ".atkHref(dispatch_url("{$node->m_module}.{$node->m_type}", $node->m_action,$clear), $text);
          $view_all += 1;
        }

        if(atk_value_in_array($atksearch))
        {
          $clear = array("atksearch"=>"");
          $text = atktext("clear_search","atk");
          $nav.=" - ".atkHref(dispatch_url("{$node->m_module}.{$node->m_type}", $node->m_action,$clear), $text);
          $view_all += 1;
        }
        
        if($view_all == 2)
        {
          $nav.=" - ".atkHref(dispatch_url("{$node->m_module}.{$node->m_type}", $node->m_action,array("atksearch"=>"","atkindex"=>"")), atktext("view_all", "atk"));
        }
      }
      
      return $nav;
    }

    function getAvailableIndices(&$node)
    {
      $selector = atkArrayNvl($node->m_postvars, 'atkfilter', "");

      $querylist = array();
      $mode = "admin";

      $db = &$node->getDb();
      $query = &$db->createQuery();

      $query->addTable($node->m_table);
      $query->addCondition($selector);
      
      $searching = false;
      
      $query->m_searchmethod = atkArrayNvl($node->m_postvars,'atksearchmethod');

      // First we check if we are about to search and set a few things
      // if we are
      if (!atkReadOptimizer())
      {
        /* there may be search criteria, which we also filter */
        $searchArray = atkArrayNvl($node->m_postvars,'atksearch');
        $smartSearchCriteria = atkArrayNvl($node->m_postvars,'atksmartsearch');
        
        if ( (is_array($searchArray) && count($searchArray)>0) 
              || (is_array($smartSearchCriteria) && count($smartSearchCriteria) > 0))
        {
          $searching = true;
        }
      }
      
      if (!$node->hasFlag(NF_NO_FILTER))
      {
        /* hard filters may be set */
        foreach($node->m_filters as $key => $value)
        {
          $query->addCondition($key."='".$value."'");
        }

        /* fuzzy filters may be set */
        for ($i = 0, $_i = count($node->m_fuzzyFilters); $i < $_i; $i++)
        {
          $query->addCondition($node->m_fuzzyFilters[$i]);
        }
      }
      
      foreach (array_keys($node->m_attribList) as $attribname)
      {
        $p_attrib = &$node->m_attribList[$attribname];
        if (is_object($p_attrib) && $node->isAllowedQueryList($attribname,$node->m_listExcludes))
        {
          $loadmode = $p_attrib->loadType($mode,$searching);
          if (hasFlag($loadmode, ADDTOQUERY)) $querylist[]=$attribname;
        }
      }

      if(!in_array($node->m_index,$querylist)) $querylist[] = $node->m_index;
      for ($i = 0, $_i = count($querylist); $i < $_i; $i++)
      {
        $p_attrib = &$node->m_attribList[$querylist[$i]];
        $p_attrib->addToQuery($query,$node->m_table,"","",1,$mode); // start at level 1
      }
      
      // there may be search criteria, which we also filter
      if (is_array($searchArray) && count($searchArray)>0)
      {
        while (list($key,$value) = each($searchArray))
        {
          if( (($value!="") && ($mode=="admin")) || (($value!="") && in_array($key,$querylist)) )
          {
            $p_attrib = &$node->getAttribute($key);

            if (is_object($p_attrib))
            {
              if (is_array($value) && isset($value[$key])&& count($value)==1)
              {
                $value = $value[$key];
              }

              $searchmode = $node->getSearchMode();
              if(is_array($searchmode))
              {
                $search = $searchmode[$key];
                if ($search=="") $search = atkconfig("search_defaultmode");
                $p_attrib->searchCondition($query, $node->m_table, $value, $search, "");
              }
              else
              {
                $p_attrib->searchCondition($query, $node->m_table, $value, $searchmode, "");
              }
            }
            else
            {
      			  atkdebug("Using default search method for $key"); 
              $condition = "lower(".$node->m_table.".".$key.") LIKE lower('%".escapeSQL($value,true)."%')";
              $query->addSearchCondition($condition);
            }
          }
        }
      }
      
      // there may be smart search criteria which we need to apply
      if (is_array($smartSearchCriteria) && count($smartSearchCriteria) > 0)
      {
        $node->applySmartSearchCriteria($query, $smartSearchCriteria);
      }
      
      //Check indexing by table.name attribute
      if(strpos($node->m_index,'.')!==false) $index = $node->m_index;
      else $index = $node->m_table.".".$node->m_index; //prevent ambigouse column error
      
      $field = "UPPER(".$db->func_substring($index,1,1).")";
      $query->addField($field,"","","",false);
      $query->addGroupBy($field);
      
      $querystring = $query->buildSelect();

      $letter = array();
      if ($querystring)
      {
        $list = $db->getrows($querystring);
        foreach ($list as $l) $letter[] = $l[$field];
      }
      return $letter;
    }    
    
  }

?>
