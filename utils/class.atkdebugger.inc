<?php

  class atkDebugger
  {        
    var $m_isconsole = true;
    
    function &getInstance($isconsole=false)
    {
      static $s_instance = NULL;
      if ($s_instance==NULL)
      {
        $s_instance = new atkDebugger($isconsole);
      }
      return $s_instance;
    }
    
    function atkDebugger($isconsole=false)
    {
      $this->m_isconsole = $isconsole;
      if (!$isconsole)
      {     
        $link = $this->consoleLink('Open console', '', array(), true);
        atkdebug($res."Debugger initialized. [".$link."]");
        $data = &$this->getDebuggerData();
        $data = array(); // start clean        
      }
      else
      {
        atkdebug("Console started");
      }
    }
    
    function addQuery($query)
    {      
      if (!$this->m_isconsole) // don't add queries executed by the console itself
      {
        $data = &$this->getDebuggerData();
        
        atkdebug("[".$this->consoleLink("query&nbsp;details", "query", array("query_id"=>count($data["queries"])), true)."] ".$query);
                
        $data["queries"][] = array("query"=>$query, "trace"=>atk_get_trace());
      }
      
    }
    
    function consoleLink($text, $action="", $params=array(), $popup=false)
    {
      static $s_first = true;
      $res = "";
      $url = atkconfig("atkroot").'debugger.php?action='.$action.$this->urlParams($params);
      
      if ($popup)
      {
        if ($s_first)
        {
          $res.= '<script type="text/javascript" language="JavaScript" src="'.atkconfig("atkroot").'atk/javascript/newwindow.js"></script>';
          $s_first = false;
        }
        $res.= '<a href="javascript:NewWindow(\''.$url.'\', \'atkconsole\', 800, 600, \'yes\', \'yes\')">'.$text.'</a>';
      }
      else
      {
        $res.= '<a href="'.$url.'">'.$text.'</a>';
      }
      return $res;
    }
    
    function urlParams($params)
    {
      if (count($params))
      {          
        foreach($params as $key=>$value)
        {
          $res.='&'.$key."=".rawurlencode($value);
        }
        return $res;
      }
      return "";
    }
    
    function renderConsole()
    {
      atkimport("atk.ui.atkpage");
      $page = &atkPage::getInstance();
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();      
      $page->register_style($theme->stylePath("debugger.css"));
      $data = &$this->getDebuggerData();
      $res = $this->consoleControls().'<br/><br/>';
      switch ($_REQUEST["action"])
      {
        case "query": $res.= $this->queryDetails($data["queries"], $_REQUEST["query_id"]); break;
        default: $res.= $this->renderQueryList($data["queries"]);
      }
      $page->addContent($res);
      return $page->render('Console');
    }
    
    function consoleControls()
    {
      return '<div id="console"><table width="100%" border="0"><tr><td align="left">ATK Debug Console</td><td align="right">'.$this->consoleLink('Console index').' | <a href="javascript:window.close()">Close console</a></td></tr></table></div>';
    }
    
    function queryDetails($queries, $id)
    {
      $output = "<h1>Query</h1>";
      $query = $queries[$id]["query"];
      $output.= $this->highlightQuery($query);
      $db = &atkGetDb();
      if (strtolower(substr($query, 0, 6))=="select")
      {
        $output.= '<h1>Resultset</h1>';              
        $result = $db->getrows($query);
        if (count($result))
        {
          $output.=$this->arrToTable($result, $_REQUEST["full"], $id);
        }
        else
        {
          $output.= "Query returned no rows";
        }      
        $output.= '<h1>Explain plan</h1>';
        $result = $db->getrows("EXPLAIN ".$query);
        $output.=$this->arrToTable($result);
        if ($queries[$id]["trace"]!="")
        {
          $output.= '<h1>Backtrace</h1>';
          $output.=$queries[$id]["trace"];
        }
      }      
      return $output;
    }
    
    function arrToTable($result, $full=true, $id="")
    {
      if (count($result))
      {
        $cols = array_keys($result[0]);
        $data = '<table border="1"><tr>';
        foreach ($cols as $col)
        {
          $data.= '<th>'.$col.'</th>';
        }
        $data.= '</tr>';
        for($i=0, $_i=count($result); $i<$_i && ($i<10||$full); $i++)
        {
          $data.= '<tr><td>'.implode('</td><td>', $result[$i]).'</td></tr>';
        }
        $data.= '</table>';
        if ($i!=$_i)
        {
          $data.= ($_i-$i).' more results. '.$this->consoleLink('Full result', 'query', array('query_id'=>$id, 'full'=>1));
        }
        return $data;
      }
      return "";
    }
    
    function highlightQuery($query)
    {
      $query = strtolower($query);
      $query = str_replace('select', '<b>SELECT</b>', $query);
      $query = str_replace('distinct', '<b>DISTINCT</b>', $query);
      $query = str_replace('where', '<b>WHERE</b>', $query);
      $query = str_replace('from', '<b>FROM</b>', $query);
      $query = str_replace('order by', '<b>ORDER BY</b>', $query);
      $query = str_replace('group by', '<b>GROUP BY</b>', $query);
      $query = str_replace('left join', '<b>LEFT</b> join', $query);
      $query = str_replace('join', '<b>JOIN</b>', $query);
      $query = str_replace('update ', '<b>UPDATE</b> ', $query);
      $query = str_replace(' set ', ' <b>SET</b> ', $query);
      $query = str_replace('delete from', '<b>DELETE FROM</b>', $query);
      return '<span class="query">'.nl2br($query).'</span>';
    }
    
    
    function &getDebuggerData()
    {
      global $g_sessionManager;
      if (is_object($g_sessionManager))
      {
        $session = &$g_sessionManager->getSession(); 
        return $session["debugger"];
      }      
      $data = array();
      return $data;
    }
    
    function renderQueryList($queries)
    {
      $output = 'Number of queries performed: '.count($queries);
      $output.= '<table border="1" width="100%"><tr><th>#</th><th>Details</th><th>Query</th></tr>';
      
      for($i=0, $_i=count($queries); $i<$_i; $i++)
      {
        $query = $queries[$i]["query"];
        if ($query=="")
        {
          $detaillink = 'EMPTY QUERY!';
        }
        else        
        {
          $detaillink = $this->consoleLink("details", "query", array("query_id"=>$i));
        }
        $output.= '<tr><td valign="top">'.($i+1).'</td><td>'.$detaillink.'</td><td>'.$this->highlightQuery($query).'</td></tr>';
      }
      
      $output.= '</table>';
      return $output;
    }
  }

?>