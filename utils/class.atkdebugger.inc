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
    var $m_redirectUrl = null;

    function &getInstance()
    {
      static $s_instance = NULL;
      if ($s_instance==NULL)
      {
        if (!atkGetSessionManager()) atkwarning("Instantiating debugger without sessionmanager, debugger will not do anything until session is started. Also, the debugging info already in the session will not be cleaned, this could lead to monster sessions over time!");
        $s_instance = new atkDebugger();
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
      if (atkGetSessionManager())
      {
        $instance = &atkDebugger::getInstance();
        if (is_object($instance))
        {
          return $instance->_addStatement($txt);
        }
      }
      return false;
    }

    function addQuery($query)
    {
      if (atkconfig('debug') > 2)
      {
        if (atkGetSessionManager())
        {
          $instance = &atkDebugger::getInstance();
          if (is_object($instance))
          {
            return $instance->_addQuery($query);
          }
        }
      }
      else
      {
        atkdebug($query);
        return true;
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
      $url = atkconfig("application_dir").'debugger.php?atkstackid='.$stackId.'&action='.$action.'&atkprevlevel='.atkLevel().$this->urlParams($params);

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

      $sessionmanager = atkGetSessionManager();
      if (is_object($sessionmanager))
      {
        $session = &$sessionmanager->getSession();
        if ($clean) $session['debugger'] = array();
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

    /**
     * Renders error messages for the user.
     *
     * @return string error messages string
     * @private
     */
    function renderPlainErrorMessages()
    {
      global $g_error_msg;
      $output = '<br><div style="font-family: monospace; font-size: 11px; color: #FF0000" align="left">error: '.implode("<br>\nerror: ",$g_error_msg).'</div>';
      return $output;
    }

    /**
     * Render debug block for the current debug information.
     *
     * @return string debug block string
     * @private
     */
    function renderDebugBlock($expanded)
    {
      global $g_debug_msg, $g_error_msg, $g_startTime;

      $time = strftime("%H:%M:%S", $g_startTime);
      $duration = sprintf("%02.05f", getmicrotime() - $g_startTime);
      $usage = function_exists("memory_get_usage") ? sprintf("%02.02f", (memory_get_usage() / 1024 / 1024)) : "? ";
      $method = $_SERVER['REQUEST_METHOD'];
      $protocol = empty($_SERVER['HTTPS']) || strtolower($_SERVER['HTTPS']) == 'off' ? 'http' : 'https';
      $url = $protocol.'://'.$_SERVER['HTTP_HOST'].($_SERVER['SERVER_PORT'] != 80 ? ':'.$_SERVER['SERVER_PORT'] : '').$_SERVER['REQUEST_URI'];

      $label = "[{$time}h / {$duration}s / {$usage}MB] $method $url";

      $output = '
        <div class="atkDebugBlock'.(count($g_error_msg) > 0 ? " atkDebugBlockContainsErrors" : "").' atkDebug'.($expanded ? 'Expanded' : 'Collapsed').'">
          <div class="atkDebugToggle" onclick="ATK.Debug.toggle(this)">
           '.$label.'
          </div>
          <div class="atkDebugData">
            '.(count($g_debug_msg) > 0 ? '<div class="atkDebugLine">'.implode($g_debug_msg, '</div><div class="atkDebugLine">').'</div>' : '').'
          </div>
        </div>';

      return $output;
    }

    /**
     * Set redirect URL.
     */
    function setRedirectUrl($url, $force=false)
    {
      // normally we only save the first redirect url, but using the force
      // parameter you can force setting another redirect url
      if ($this->m_redirectUrl === null || $force)
      {
        $this->m_redirectUrl = $url;
      }
    }

    /**
     * Renders the redirect link if applicable.
     */
    function renderRedirectLink()
    {
      if ($this->m_redirectUrl == null) return '';

      $output = '
        <div class="atkDebugRedirect">
           Non-debug version would have redirected to <a href="'.$this->m_redirectUrl.'">'.$this->m_redirectUrl.'</a>
        </div>';

      return $output;
    }

    /**
     * Renders the debug and error messages to a nice HTML string.
     *
     * @return string html string
     */
    function renderDebugAndErrorMessages()
    {
      global $ATK_VARS, $g_debug_msg, $g_error_msg, $g_startTime;

      // check if this is an Ajax request
      $isPartial = isset($ATK_VARS['atkpartial']);

      // only display error messages
      if (count($g_error_msg) > 0 && atkconfig('display_errors') && atkconfig('debug') <= 0 && !$isPartial)
      {
        return $this->renderPlainErrorMessages();
      }

      // no debug messages or error messages to output
      else if (atkconfig('debug') <= 0 || (count($g_debug_msg) == 0 && count($g_error_msg) == 0))
      {
        return '';
      }

      $expanded = !$isPartial;
      if ($expanded && @$_COOKIE['atkdebugstate'] == 'collapsed')
      {
        $expanded = false;
      }

      // render debug block
      $block = $this->renderDebugBlock($expanded);

      if ($isPartial)
      {
        atkimport('atk.utils.atkjson');

        $output =
          '<script type="text/javascript">
            ATK.Debug.addContent('.atkJSON::encode($block).');
           </script>';
      }
      else
      {
        $ui = &atkinstance('atk.ui.atkui');
        $stylesheet = $ui->stylePath('atkdebug.css');
        $script = atkconfig('atkroot').'atk/javascript/class.atkdebug.js';

        $redirect = $this->renderRedirectLink();

        $output = '
          <script type="text/javascript" src="'.$script.'"></script>
          <script type="text/javascript">
            ATK.Debug.registerStylesheet("'.$stylesheet.'");
          </script>
          <div id="atk_debugging_div">
            '.$redirect.'
            '.$block.'
          </div>';
      }

      return $output;
    }

    /**
     * Gets the microtime
     * @static
     * @return int the microtime
     */
    function getMicroTime()
    {
      list($usec, $sec) = explode(" ",microtime());
      return ((float)$usec + (float)$sec);
    }

    function elapsed()
    {
      global $g_startTime;

      static $offset=null, $previous=null;

      if ($offset === null)
      {
        $offset = $g_startTime;
        $previous = $offset;
      }

      $new = atkDebugger::getMicroTime();
      $res = "+".sprintf("%02.05f",$new-$offset)."s / ".sprintf("%02.05f", $new-$previous)."s";
      $previous = $new;
      return $res;
    }
  }
?>
