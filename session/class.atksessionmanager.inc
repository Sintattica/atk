<?php
  /**
   * Tools for session management
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */

  $cookie_params = session_get_cookie_params();
  session_set_cookie_params($cookie_params["lifetime"], dirname($SCRIPT_NAME));
  session_start();
  session_register("g_sessionData");

  define(SESSION_DEFAULT,0); // stay at current stacklevel
  define(SESSION_NEW,1);     // new stack
  define(SESSION_NESTED,2);  // new item on current stack
  define(SESSION_BACK,3);    // move one level down on stack
  define(SESSION_REPLACE,4); // replace current stacklevel

  /* Workaround for a PHP4.0.4 bug */
  $atklevel = trim($atklevel);
  $atkprevlevel = trim($atkprevlevel);
  $atkstackid = trim($atkstackid);

  class atkSessionManager
  {
    var $m_namespace;

    var $m_escapemode = false; // are we escaping?

    function atkSessionManager($namespace)
    {
      $this->m_namespace = $namespace;
      atkdebug("creating sessionManager (namespace: $namespace)");
    }

    function session_read(&$postvars)
    {
      $this->globalscope($postvars);
      $this->stackscope($postvars);
    }

    function globalVar($var, $value="", $no_namespace=false)
    {
      global $g_sessionData;

      if ($value=="") $value = $GLOBALS[$var];

      if ($no_namespace) $g_sessionData["globals"][$var] = $value;
      else $g_sessionData[$this->m_namespace]["globals"][$var] = $value;

    }

    function getValue($var, $namespace="")
    {
      global $g_sessionData;
      if ($namespace=="globals") return $g_sessionData["globals"][$var];
      else if ($namespace!="") return $g_sessionData[$namespace]["globals"][$var];
      return $g_sessionData[$this->m_namespace]["globals"][$var];
    }

    function stackVar($var, $value="")
    {
      if (!$this->m_escapemode)
      {
        global $g_sessionData;

        $currentitem = &$g_sessionData[$this->m_namespace]["stack"][atkStackID()][atkLevel()];

        if (is_array($currentitem))
        {
          if ($value=="")
          {
            if ($GLOBALS[$var]!="")
            {
              $currentitem[$var] = $GLOBALS[$var];
            }
          }
          else
          {
            $currentitem[$var] = $value;
          }
          if (!is_array($currentitem["defined_stackvars"])||!in_array($var, $currentitem["defined_stackvars"]))
          {
            $currentitem["defined_stackvars"][] = $var;
          }
          // We always return the current value..
          return $currentitem[$var];
        }
      }
    }

    function pageVar($var, $value="")
    {
      if (!$this->m_escapemode)
      {
        global $g_sessionData;

        $currentitem = &$g_sessionData[$this->m_namespace]["stack"][atkStackID()][atkLevel()];

        if ($value=="")
        {
          if ($GLOBALS[$var]!="")
          {
            $currentitem[$var] = $GLOBALS[$var];
          }
        }
        else
        {
          $currentitem[$var] = $value;
        }
        if (!is_array($currentitem["defined_pagevars"])||!in_array($var, $currentitem["defined_pagevars"]))
        {
          $currentitem["defined_pagevars"][] = $var;
        }
        // We always return the current value..
        return $currentitem[$var];
      }
    }

    function globalscope(&$postvars)
    {
      global $g_sessionData;

      $current = &$g_sessionData[$this->m_namespace]["globals"];
      if (!is_array($current))
      {
        $current = array();
      }

      // Posted vars always overwrite anything in the current session..
      foreach($current as $var => $value)
      {
        if (isset($postvars[$var])&&$postvars[$var]!="")
        {
          $current[$var] = $postvars[$var];
        }
      }

      foreach($current as $var => $value)
      {
        $postvars[$var] = $value;
      }

    }

    function stackscope(&$postvars)
    {
      global $g_sessionData, $atklevel, $g_sessionVars, $g_pageVars, $HTTP_GET_VARS;

      // Mental note: We have an atkLevel() function for retrieving the atklevel,
      // but we use the global var itself here, because it gets modified in
      // the stackscope function.

      if (!isset($atklevel)||$atklevel=="") $atklevel=0;

      atkdebug("ATKLevel: ".$atklevel);

      $this->_verifyStackIntegrity();

      // New stack, new stackid, if level = -1.
      $stackid = atkStackID(($atklevel==-1));

      $stack = &$g_sessionData[$this->m_namespace]["stack"][$stackid];

      if ($postvars["atkescape"]!="")
      {
        $this->m_escapemode = true;
        atkdebug("ATK session escapemode");

        $currentitem = &$stack[count($stack)-1];

        atkdebug("Saving formdata in session");

        $atkformdata = array();
        foreach(array_keys($postvars) as $varname)
        {
          // only save formdata itself, hence no $atk.. variables
          if (substr($varname,0,3)!="atk")
          {
            $atkformdata[$varname] = $postvars[$varname];
          }
        }
        $currentitem["atkformdata"] = $atkformdata;

        // also remember getvars that were passed in the url
        foreach(array_keys($HTTP_GET_VARS) as $var)
        {
          if (isset($postvars[$var])&&$postvars[$var]!="")
          {
            $currentitem[$var] = $postvars[$var];
          }
        }

      }
      else
      {

        if ($atklevel==-1||!is_array($stack))
        {
          atkdebug("Cleaning stack");
          $stack = array();
          $atklevel = 0;
        }
        else if ($atklevel==-2)
        {
          // Replace top level.
          array_pop($stack);

          // Note that the atklevel is now -2. This is actually wrong. We are at
          // some level in the stack. We can determine the real level by
          // counting the stack.
          $atklevel = count($stack);
        }

        $currentitem = $stack[$atklevel];

        if (!isset($currentitem)||$currentitem=="")
        {
          atkdebug("New level on session stack");
          // new level.. always based on the previous level
          //$newstackitem = array();
          //$currentitem = $stack[count($stack)-1];
          $copieditem = $stack[count($stack)-1];

          if (is_array($copieditem))
          {
            foreach($copieditem as $key=>$value)
            {
              if(in_array($key,$g_sessionVars)||
                 (is_array($copieditem["defined_stackvars"])
                  &&in_array($key,$copieditem["defined_stackvars"])))
              {
                $currentitem[$key] = $value;
              }
            }
            $currentitem["defined_stackvars"]= $copieditem["defined_stackvars"];
          }

          // Posted vars always overwrite anything in the current session..
          foreach(array_merge($g_pageVars, $g_sessionVars) as $var)
          {
            if (isset($postvars[$var])&&$postvars[$var]!="")
            {
              if ($postvars[$var]=="clear")
              {
                $currentitem[$var] = "";
              }
              else
              {
                $currentitem[$var] = $postvars[$var];
              }
            }
          }

          array_push($stack, $currentitem);
        }
        else
        {
          // Stay at the current level..
          // If we are getting back from a higher level, we may now delete everything above
          $deletecount = (count($stack)-1)-$atklevel;
          for ($i=0;$i<$deletecount;$i++)
          {
            atkdebug("popped an item out of the stack");
            array_pop($stack);
          }

          foreach($g_pageVars as $var)
          {
            if (isset($postvars[$var])&&count($postvars[$var])>0)
            {
              $currentitem[$var] = $postvars[$var];
            }
          }

          // page vars must overwrite the current stack..
          $stack[$atklevel] = $currentitem;

          // session vars need not be remembered..
          foreach($g_sessionVars as $var)
          {
            if (isset($postvars[$var])&&count($postvars[$var])>0)
            {
              $currentitem[$var] = $postvars[$var];
            }
          }

        }

        if (is_array($currentitem["atkformdata"]))
        {
          atkdebug("Session formdata present");
          // atkformdata will be present in postvars.. editForm() has to read them..
        }

        if (is_array($currentitem))
        {
          foreach($currentitem as $var => $value)
          {
            $postvars[$var] = $value;
          }
        }
      } // end if atkescape

    }

    function stackTrace()
    {
      global $g_sessionData, $g_layout;
      $res = "";
      $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];

      // we only show the last 4 elements of the stack
      if (count($stack)>4) $res = "... | ";

      for ($i=max(0,count($stack)-4);$i<count($stack);$i++)
      {
        $mod = getNodeModule($stack[$i]["atknodetype"]);
        $node = getNodeType($stack[$i]["atknodetype"]);
        $title = text($g_layout->title($mod, $node, $stack[$i]["atkaction"]));
        if ($i<count($stack)-1)
        {
          $res.='<a href="'.session_url('dispatch.php?atklevel='.$i).'">'.$title.'</a> | ';
        }
        else
        {
          $res.=$title;
        }
      }
      return $res;
    }

    /**
     * Verify the integrity of the session stack, in the case a user
     * opens links in a new window, which could confuse the session manager.
     * In the case we detect this, we fork the session stack so both windows
     * have their own stacks.
     */
    function _verifyStackIntegrity()
    {
      global $g_sessionData, $atkstackid, $atklevel, $atkprevlevel;
      $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];
      if (!is_array($stack)) $prevlevelfromstack = 0;
      else $prevlevelfromstack = count($stack)-1;

      if ($atkprevlevel!=$prevlevelfromstack)
      {
        // What we think we came from (as indicated in the url by atkprevlevel)
        // and what the REAL situation on the stack was when we got here (prevlevelfromstack)
        // is different. Let's fork the stack.
        atkdebug("Multiple windows detected: levelstack forked (atkprevlevel=$atkprevlevel, real: $prevlevelfromstack)");
        $newid = atkStackID(true);
        
        // We must also make this stack 'ok' with the atkprevlevel.
        // (there may be more levels on the stack than we should have, because
        // we forked from another window which might already be at a higher
        // stack level).
        $deletecount = (count($stack)-1)-$atkprevlevel;
        for ($i=0;$i<$deletecount;$i++)
        {
          atkdebug("popped an item out of the forked stack");
          array_pop($stack);
        }        
        
        $g_sessionData[$this->m_namespace]["stack"][$newid] = $stack;        
        return;
      }
    }


  }

  /**
    Calling this function will run atk session management.
    Call this function in every file that you want to use atk sessions.
  */
  function atksession($namespace = "default")
  {
    global $HTTP_POST_VARS, $HTTP_GET_VARS, $ATK_VARS, $g_sessionManager;

    $g_sessionManager = new atkSessionManager($namespace);

    if (count($HTTP_POST_VARS)>0)
    {
      atkDataDecode($HTTP_POST_VARS);
      $ATK_VARS = $HTTP_POST_VARS;
    }
    else
    {
      atkDataDecode($HTTP_GET_VARS);
      $ATK_VARS = $HTTP_GET_VARS;
    }

    $g_sessionManager->session_read($ATK_VARS);
  }

  function sessionStore($var, $value)
  {
    global $g_sessionManager;
    $g_sessionManager->globalVar($var, $value);
  }

  function atkStackID($new = false)
  {
    global $atkstackid;
    if (!isset($atkstackid)||$atkstackid==""||$new)
    {
      // No stack id yet, or forced creation of a new one.
      $atkstackid = uniqid("");
    }
    return $atkstackid;
  }

  function atkLevel()
  {
    global $atklevel;
    if (!isset($atklevel)||$atklevel=="")
    {
      $atklevel = 0; // assume bottom level.
    }
    return $atklevel;
  }

  function sessionLoad($var, $namespace="")
  {
    global $g_sessionManager;
    return $g_sessionManager->getValue($var, $namespace);
  }

?>
