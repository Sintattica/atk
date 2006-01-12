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
    function buildNavigation(&$node, $mode="", $navurls="", $embedded=false)
    {
      if ($navurls=="")
      {
        // Have to determine navurls ourselves.
        $navurls = $this->getNavigationUrls($node, $mode);
      }
      $links=array();
      foreach ($navurls as $key=>$nav)
      {
        $links[] = href($nav["url"], $nav["title"], SESSION_DEFAULT, $embedded);
      }
      return implode("&nbsp;|&nbsp;", $links);
    }

    /**
     * Creates a navigation bar, for browsing through the record pages
     * (if a limit is set, and there are more records)
     * If an owner node is passed along, then we use the atkselector
     * on the owner as the navigator is used on a relation
     * @param obj   &$node   instance of the node we create the navigation bar for
     * @param string $mode   the mode we are in, defaults to ""
     * @param string $filter a filter for the destination node
     * @return string a HTML string for navigating through records
     */
    function getNavigationUrls(&$node, $mode="", $filter="")
    {
      global $ATK_VARS;

      $res = array();
      $limit = (int)$node->m_postvars['atklimit'];

      if (!$filter && isset($node->m_postvars["atkfilter"])) $filter = $node->validateFilter($node->m_postvars["atkfilter"]);

      $count = (int)$node->countDb($filter, $node->m_listExcludes, "", $mode);

      // maximum number of bookmarks to pages.
      $max_bm = 10;

      if (!($limit > 0 && $count > $limit && ceil($count / $limit) > 1)) return array();

      $pages = ceil($count / $limit);
      $curr  = ($node->m_postvars['atkstartat'] / $limit) + 1;
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
                                  "url"=>$_SERVER["PHP_SELF"].
                                          "?atknodetype=".$ATK_VARS["atknodetype"].
                                          "&atkaction=".$action.
                                          (isset($ATK_VARS["atktarget"])?"&atktarget=".rawurlencode($ATK_VARS["atktarget"]):"").
                                          "&atkstartat".(isset($node->m_postvars["limitPostfix"])?$node->m_postvars["limitPostfix"]:"")."=".$newstart);
      }

      for ($i = $begpg; $i <= $endpg; $i++)
      {
         if ($i==$curr) $res[$i] = array("title"=>"<b>$i</b>", "url"=>"");
         else $res[$i] = array("title"=>$i,
                               "url"=>$_SERVER["PHP_SELF"].
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
                              "url"=>$_SERVER["PHP_SELF"].
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

      // TODO: show only those letters that are present in the database.
      // e.g. don't show the letter Q if there aren't any records starting
      // with a Q.
      // for now, we use all letters:
      $available_letters = array('A','B','C','D','E','F','G','H','I','J','K','L','M',
                                 'N','O','P','Q','R','S','T','U','V','W','X','Y','Z');
      $count = count($available_letters);
      for ($i=0;$i<$count;$i++)
      {
        $char = $available_letters[$i];
        if ($selected!="" && strtoupper($selected[0])==$char) $label="<b>$char</b>";
        else $label=$char;

        // We use wildcard search for the index. Should be supported by any database.

        //The searchmode of an index should be used only once, therefore it uses atksinglesearchmode instead of atksearchmode.
        $nav.=href($_SERVER["PHP_SELF"]."?atknodetype=".$node->atkNodeType()."&atkaction=".$node->m_action."&atksearch[".$node->m_index."]=".$char."*&atksinglesearchmode[".$node->m_index."]=wildcard&atkstartat=0",$label);

        // uncomment the following line if 26 letters seems to become too wide.
        //if ($count>13 && ($i+1)==floor($count/2)) $nav.='<br>'; else
        if ($i!=$count-1) $nav.=" ";
      }

      if (isset($node->m_postvars['atksearch']))
        $nav.=" - ".href(dispatch_url("{$node->m_module}.{$node->m_type}", $node->m_action,array("atksearch"=>"")), atktext("view_all", "atk"));

      return $nav;
    }
  }

?>
