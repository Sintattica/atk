<?php

  class atkNavigator
  {
    function buildNavigation(&$node, $mode="", $navurls="")
  	{
  	  if ($navurls=="")
  	  {
  	    // Have to determine navurls ourselves.
  	    $navurls = $this->getNavigationUrls($node, $mode);
  	  }
  	  $links=array();
  	  foreach ($navurls as $key=>$nav)
  	  {
  	    $links[] = href($nav["url"], $nav["title"]);
  	  }
  	  return implode("&nbsp;|&nbsp;", $links);
  	 
  	}
  	
    /**
     * Creates a navigation bar, for browsing through the record pages
     * (if a limit is set, and there are more records)
     * @return a HTML string for navigating through records
     */
    function getNavigationUrls(&$node, $mode="")
    {	     
  	  $res = array();
      $limit = (int)$node->m_postvars['atklimit'];

      $filter = $node->validateFilter($node->m_postvars["atkfilter"]);

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

      if ($curr > 1)
      {
        $newstart = $node->m_postvars['atkstartat'] - $limit;
        $res['previous'] = array("title"=>text("previous"), "url"=>session_url($_SERVER["PHP_SELF"]."?atknodetype=".$node->atkNodeType()."&atkaction=".$node->m_action."&atkstartat=".$newstart));
      }

      for ($i = $begpg; $i <= $endpg; $i++)
      {
         if ($i==$curr) $res[$i] = array("title"=>"<b>$i</b>", "url"=>"");
         else $res[$i] = array("title"=>$i,
                               "url"=>session_url($_SERVER["PHP_SELF"]."?atknodetype=".$node->atkNodeType()."&atkaction=".
                                                  $node->m_action."&atkstartat=".max(0, ($i-1) * $limit)));
      }

      if ($curr < $pages)
      {
        $newstart = $node->m_postvars['atkstartat'] + $limit;
        $res['next'] = array("title"=>text("next"), "url"=>session_url($_SERVER["PHP_SELF"].'?atkstartat='.$newstart));
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
        if (strtoupper($selected[0])==$char) $label="<b>$char</b>";
        else $label=$char;

        // We use wildcard search for the index. Should be supported by any database.

        $nav.=href($_SERVER["PHP_SELF"]."?atknodetype=".$node->atkNodeType()."&atkaction=".$node->m_action."&atksearch[".$node->m_index."]=".$char."*&atksearchmode[".$node->m_index."]=wildcard",$label);

        // uncomment the following line if 26 letters seems to become too wide.
        //if ($count>13 && ($i+1)==floor($count/2)) $nav.='<br>'; else
        if ($i!=$count-1) $nav.=" ";
      }

      return $nav;
    }  
  }

?>