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
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * This class implements the ATK debug console for analysing queries
   * performed in a page.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */
  class atkDebugger
  {
    var $m_isconsole = true;

    function &getInstance()
    {
      static $s_instance = NULL;
      if ($s_instance==NULL)
      {
        global $g_sessionManager;
        if (is_object($g_sessionManager)) // don't bother to create if session has not yet been initialised
        {
          $s_instance = new atkDebugger();
        }
      }
      return $s_instance;
    }

    function atkDebugger()
    {
      $this->m_isconsole = (strpos($_SERVER['SCRIPT_NAME'], "debugger.php")!==false);
      if (!$this->m_isconsole)
      {
        $link = $this->consoleLink('Open console', '', array(), true);
        $data = &$this->getDebuggerData(true);
        $data = array(); // start clean
        global $g_debug_msg;
        $g_debug_msg[] = atkGetTimingInfo()."Debugger initialized. [".$link."]";
      }
    }

    function addStatement($txt)
    {
      $instance = &atkDebugger::getInstance();
      if (is_object($instance))
      {
        return $instance->_addStatement($txt);
      }
      return false;
    }

    function addQuery($query)
    {
      $instance = &atkDebugger::getInstance();
      if (is_object($instance))
      {
        return $instance->_addQuery($query);
      }
      return false;
    }

    function _addStatement($txt)
    {
      if (!$this->m_isconsole)
      {
        $data = &$this->getDebuggerData();
        global $g_debug_msg;
        $data["statements"][] = array("statement"=>$txt, "trace"=>atk_get_trace());
        $txt = str_replace("MB]", "MB] [".$this->consoleLink("trace", "statement", array("stmt_id"=>count($data["statements"])-1), true)."]", $txt);
        $g_debug_msg[] = $txt;
        return true;
      }
      return false;
    }

    function _addQuery($query)
    {
      if (!$this->m_isconsole) // don't add queries executed by the console itself
      {
        $data = &$this->getDebuggerData();

        $data["queries"][] = array("query"=>$query, "trace"=>atk_get_trace());

        atkdebug("[".$this->consoleLink("query&nbsp;details", "query", array("query_id"=>count($data["queries"])-1), true)."] ".$query);

        return true;
      }
      return false;
    }

    function consoleLink($text, $action="", $params=array(), $popup=false, $stackId=NULL)
    {
      if ($stackId == NULL)
      {
        $stackId = atkStackId();
      }
      
      static $s_first = true;
      $res = "";
      $url = atkconfig("atkroot").'debugger.php?atkstackid='.$stackId.'&action='.$action.'&atkprevlevel='.atkLevel().$this->urlParams($params);

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
        $res = "";
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
      $page = &atkinstance("atk.ui.atkpage");
      $theme = &atkinstance("atk.ui.atktheme");
      $page->register_style($theme->stylePath("debugger.css"));
      $data = &$this->getDebuggerData(false, $_REQUEST['atkstackid']);
      $res = $this->consoleControls().'<br/><br/>';
      switch ($_REQUEST["action"])
      {
        case "query": $res.= $this->queryDetails($data["queries"], $_REQUEST["query_id"]); break;
        case "statement": $res.= $this->statementDetails($data["statements"], $_REQUEST["stmt_id"]); break;
        default:
        {
          $res.= $this->renderQueryList($data["queries"]);
          $res.= $this->renderStatementList($data["statements"]);
        }
      }
      $page->addContent($res);
      return $page->render('Console');
    }

    function consoleControls()
    {
      return '<div id="console"><table width="100%" border="0"><tr><td align="left">ATK Debug Console</td><td align="right">'.$this->consoleLink('Console index', '', array(), false, $_REQUEST['atkstackid']).' | <a href="javascript:window.close()">Close console</a></td></tr></table></div>';
    }

    function queryDetails($queries, $id)
    {
      $output = "<h1>Query</h1>";
      $query = $queries[$id]["query"];
      $output.= $this->highlightQuery($query);
      $db = &atkGetDb();
      if (strtolower(substr(trim($query), 0, 6))=="select")
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
      }
      if ($queries[$id]["trace"]!="")
      {
        $output.= '<h1>Backtrace</h1>';
        $output.=$queries[$id]["trace"];
      }
      return $output;
    }

    function statementDetails($stmts, $id)
    {
      $output = "<h1>Debug Statement</h1>";
      $stmt = $stmts[$id]["statement"];
      $output.= "<b>".$stmt."</b>";

      if ($stmts[$id]["trace"]!="")
      {
        $output.= '<h1>Backtrace</h1>';
        $output.=$stmts[$id]["trace"];
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


    function &getDebuggerData($clean=false, $stackId=NULL)
    {
      if ($stackId == NULL)
      {
        $stackId = atkStackId();
      }      
      
      global $g_sessionManager;
      if (is_object($g_sessionManager))
      {
        $session = &$g_sessionManager->getSession();
        //if ($clean) $session['debugger'] = array();  
        $var= &$session["debugger"][$stackId];
        return $var;
      }
      $data = array();
      return $data;
    }

    function renderQueryList($queries)
    {
      $output = 'Number of queries performed: '.count($queries);
      if (count($queries))
      {
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

    function renderStatementList($statements)
    {
      $output = 'Number of debug statements: '.count($statements);
      if (count($statements))
      {
        $output.= '<table border="1" width="100%"><tr><th>#</th><th>Details</th><th>Statement</th></tr>';

        for($i=0, $_i=count($statements); $i<$_i; $i++)
        {

          $detaillink = $this->consoleLink("details", "statement", array("stmt_id"=>$i));
          $output.= '<tr><td valign="top">'.($i+1).'</td><td>'.$detaillink.'</td><td>'.$statements[$i]["statement"].'</td></tr>';
        }

        $output.= '</table>';
        return $output;
      }
    }
  }

?>