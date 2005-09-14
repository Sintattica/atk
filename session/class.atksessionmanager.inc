<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage session
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal some session initialization.
   */
  $cookie_params = session_get_cookie_params();
  
  $config_identifier = atkconfig("identifier");
  if (empty($config_identifier)) $config_identifier = "default";
  
  $cookiepath = atkconfig("application_root");
  session_set_cookie_params($cookie_params["lifetime"],$cookiepath);
  
  // set cache expire
  session_cache_expire(atkconfig("session_cache_expire"));
  
  // set the cache limiter (used for caching)
  session_cache_limiter(atkconfig("session_cache_limiter"));
  
  // If somehow the sessionid is unclean (searchengine bots have been known to mangle sessionids)
  // we don't have a session...
  if (isSessionIdValid())
  {   
    session_name(atkconfig("identifier"));
    session_start();    
  
  // backwardscompatibility hacks. g_sessionData and g_sessionData are obsolete actually.
  // You can use $session = &atkSessionManager::getSession() now, and you'll have a
  // session enabled, multi-app array in which you can store whatever you like.
  // There are old applications however that still use $g_sessionData, so I'll 
  // leave it in place for now.
    $g_sessionData = &$_SESSION[$config_identifier];
  }  
  define("SESSION_DEFAULT",0); // stay at current stacklevel
  define("SESSION_NEW",1);     // new stack
  define("SESSION_NESTED",2);  // new item on current stack
  define("SESSION_BACK",3);    // move one level down on stack
  define("SESSION_REPLACE",4); // replace current stacklevel
  
  if (isset($_REQUEST["atklevel"])) 	 $atklevel 		= trim($_REQUEST["atklevel"]);
  if (isset($_REQUEST["atkprevlevel"]))$atkprevlevel= trim($_REQUEST["atkprevlevel"]);
  if (isset($_REQUEST["atkstackid"]))  $atkstackid 	= trim($_REQUEST["atkstackid"]);      

  /**
   * The atk session manager. 
   *
   * Any file that wants to make use of ATK sessions, should have a call to
   * atksession() in the top php file (all ATK default files already have
   * this).
   * After the session has been initialised with atksession(), the session
   * manager can be used using the global variable $g_sessionManager.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage session
   *
   */
  class atkSessionManager
  {
    /**
     * @access private
     * @var String
     */
    var $m_namespace;

    /**
     * @access private
     * @var boolean
     */
    var $m_escapemode = false; // are we escaping?

    /** 
     * @access private
     * @var boolean
     */
    var $m_usestack = true; // should we use a session stack?

    /**
     * Default constructor.
     * @param String $namespace If multiple scripts/applications are
     *                          installed on thesame url, they can each use
     *                          a different namespace to make sure they 
     *                          don't share session data.
     * @param boolean $usestack Tell the sessionmanager to use the session
     *                          stack manager (back/forth navigation in
     *                          screens, remembering vars over multiple
     *                          pages etc). This comes with a slight
     *                          performance impact, so scripts not using
     *                          the stack should pass false here.
     */
    function atkSessionManager($namespace, $usestack=true)
    {
      $this->m_namespace = $namespace;
      $this->m_usestack = $usestack;
      // added in 5.3 but not working
      // session_regenerate_id();
      atkdebug("creating sessionManager (namespace: $namespace)");
    }
    
    /**
     * Get the name of the current session (as was passed to atksession())
     * @return String namespace
     */
    function getNameSpace()
    {
      return $this->m_namespace;
    }

    /**
     * Read session variables from the stack and the global scope.
     * @access private
     * @param array $postvars Any variables passed in the http request.
     */
    function session_read(&$postvars)
    {
      $this->_globalscope($postvars);
      if ($this->m_usestack)
      {
        $this->_stackscope($postvars);
      }
    }

    /**
     * Register a global variable. 
     * 
     * Saves a value in the current namespace. 
     * @param String $var The name of the variable to save.
     * @param mixed $value The value of the variable to save. If omitted,
     *                     the value is retrieved from the http request.
     * @param boolean $no_namespace If set to false, the variable is saved
     *                              in the current namespace. If set to true,
     *                              the variable is available in all
     *                              namespaces.
     */
    function globalVar($var, $value="", $no_namespace=false)
    {
      global $g_sessionData;

      if ($value=="" && isset($_REQUEST[$var])) $value = $_REQUEST[$var];      

      if ($no_namespace) $g_sessionData["globals"][$var] = $value;
      else $g_sessionData[$this->m_namespace]["globals"][$var] = $value;
            
    }

    /**
     * Retrieve the value of a session variable.
     *
     * @param String $var The name of the variable to retrieve.
     * @param String $namespace The namespace from which to retrieve the
     *                          variable, or "globals" if the global value
     *                          needs to be retrieved.
     * @return mixed The retrieved value.
     */
    function getValue($var, $namespace="")
    {
      global $g_sessionData;            
      if ($namespace=="globals") return $g_sessionData["globals"][$var];
      else if ($namespace!="") return $g_sessionData[$namespace]["globals"][$var];
      if (isset($g_sessionData[$this->m_namespace]["globals"][$var]))
      {
        return $g_sessionData[$this->m_namespace]["globals"][$var];
      }
      return NULL;
    }

    /**
     * Store a variable in the session stack. The variable is available in the
     * current page (even after reload), and any screen that is deeper on the
     * session stack.
     * The method can be used to transparantly both store and retrieve the value.
     * If a value gets passed in a url, the following statement is useful:
     * <code>
     *   $view = $g_sessionManager->stackVar("view");
     * </code>
     * This statement makes sure that $view is always filled. If view is passed
     * in the url, it is stored as the new default stack value. If it's not 
     * passed in the url, the last known value is retrieved from the session.
     *
     * @param String $var The name of the variable to store.
     * @param mixed $value The value to store. If omitted, the session manager
     *                     tries to read the value from the http request.
     * @return mixed The current value in the session stack. 
     */
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
            if ($_REQUEST[$var]!="")
            {                          
              $currentitem[$var] = $_REQUEST[$var];
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

    /**
     * Store a variable in the session stack. The variable is available only in 
     * the current page (even after reload). In contrast with stackVar(), the 
     * variable is invisible in deeper screens.
     *
     * The method can be used to transparantly both store and retrieve the value.
     * If a value gets passed in a url, the following statement is useful:
     * <code>
     *   $view = $g_sessionManager->pageVar("view");
     * </code>
     * This statement makes sure that $view is always filled. If view is passed
     * in the url, it is stored as the new default stack value. If it's not 
     * passed in the url, the last known value is retrieved from the session.
     *
     * @param String $var The name of the variable to store.
     * @param mixed $value The value to store. If omitted, the session manager
     *                     tries to read the value from the http request.
     * @return mixed The current value in the session stack. 
     */
    function pageVar($var, $value="")
    {
      if (!$this->m_escapemode)
      {
        global $g_sessionData;

        $currentitem = &$g_sessionData[$this->m_namespace]["stack"][atkStackID()][atkLevel()];

        if ($value=="")
        {
          if (isset($_REQUEST[$var]) && !empty($_REQUEST[$var]))
          {
            $currentitem[$var] = $_REQUEST[$var];
          }
        }
        else
        {
          $currentitem[$var] = $value;
        }
        if (!isset($currentitem["defined_pagevars"])||!is_array($currentitem["defined_pagevars"])||!in_array($var, $currentitem["defined_pagevars"]))
        {
          $currentitem["defined_pagevars"][] = $var;
        }
        // We always return the current value..
        return $currentitem[$var];
      }
    }

    /**
     * Process the global variable scope.
     * @access private
     * @param array $postvars The http request variables.
     */
    function _globalscope(&$postvars)
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

    /**
     * Process the variable stack scope (pagevars, stackvars).
     * @access private
     * @param array $postvars The http request variables.
     */
    function _stackscope(&$postvars)
    {
      global $g_sessionData, $atklevel;
      
      // session vars are valid until they are set to something else. if you go a session level higher,
      // the next level will still contain these vars (unless overriden in the url)
      $sessionVars = array("atknodetype","atkfilter","atkselector","atkaction","atkpkret");
      
      // pagevars are valid on a page. if you go a session level higher, the pagevars are no longer
      // visible until you return.
      $pageVars = array("atksearch","atksearchmode","atkorderby","atkstartat","atktarget","atkformdata","atktree","atksuppress","atktab");

      // Mental note: We have an atkLevel() function for retrieving the atklevel,
      // but we use the global var itself here, because it gets modified in
      // the stackscope function.

      if (!isset($atklevel)||$atklevel=="") $atklevel=0;

      atkdebug("ATKLevel: ".$atklevel);

      $this->_verifyStackIntegrity();

      // New stack, new stackid, if level = -1.
      $stackid = atkStackID(($atklevel==-1));            

      $stack = &$g_sessionData[$this->m_namespace]["stack"][$stackid];

      if (isset($postvars["atkescape"]) && $postvars["atkescape"]!="")
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
        // this *may not be* $_REQUEST, because then the posted vars
        // will be overwritten, which may not be done in escape mode, 
        // I wonder if the next few lines are necessary at all, but
        // I think I needed them once, so I'll leave it in place. 
        foreach(array_keys($_GET) as $var) 
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

        if (isset($stack[$atklevel])) $currentitem = $stack[$atklevel];              
        
        if (!isset($currentitem)||$currentitem=="")
        {
          atkdebug("New level on session stack");
          // Initialise
          $currentitem = array();
          // new level.. always based on the previous level
          //$newstackitem = array();
          //$currentitem = $stack[count($stack)-1];
          if (isset($stack[count($stack)-1])) $copieditem = $stack[count($stack)-1];

          if (isset($copieditem) && is_array($copieditem))
          {
            foreach($copieditem as $key=>$value)
            {
              if(in_array($key, $sessionVars)||
                 (isset($copieditem["defined_stackvars"])&&is_array($copieditem["defined_stackvars"])
                  &&in_array($key,$copieditem["defined_stackvars"])))
              {
                $currentitem[$key] = $value;
              }
            }
            if (isset($copieditem["defined_stackvars"]))
            {
              $currentitem["defined_stackvars"]= $copieditem["defined_stackvars"];
            }
          }                  

          // Posted vars always overwrite anything in the current session..
          foreach(array_merge($pageVars, $sessionVars) as $var)
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

          foreach($pageVars as $var)
          {          
            if (isset($postvars[$var])&&count($postvars[$var])>0)
            {
              $currentitem[$var] = $postvars[$var];
            }
          }

          // page vars must overwrite the current stack..
          $stack[$atklevel] = $currentitem;                    

          // session vars need not be remembered..
          foreach ($sessionVars as $var)
          {          
            if (isset($postvars[$var])&&count($postvars[$var])>0)
            {
              $currentitem[$var] = $postvars[$var];
            }
          }                    

        }

        if (isset($currentitem["atkformdata"]) && is_array($currentitem["atkformdata"]))
        {
          atkdebug("Session formdata present");
          // atkformdata will be present in postvars.. editForm() has to read them..
        }                

        if (is_array($currentitem))
        {
          foreach($currentitem as $var => $value)
          {
            // don't override what was passed in the url.
            if (!isset($postvars[$var]))
            {
              $postvars[$var] = $value;            
            }
          }
        }        
      } // end if atkescape      

    }

    /**
     * Retrieve a trace of the current session stack.
     * @return array Array containing the title and url for each stacklevel.
     *               The url can be used to directly move back on the session
     *               stack.
     */
    function stackTrace()
    {
      global $g_sessionData;
      
      $ui = &atknew("atk.ui.atkui");   
      
      $res = array();
      $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];

      for ($i=0;$i<count($stack);$i++)
      {
        $mod = getNodeModule($stack[$i]["atknodetype"]);
        $node = getNodeType($stack[$i]["atknodetype"]);
        $title = text($ui->title($mod, $node, $stack[$i]["atkaction"]));
        if ($i<count($stack)-1)
        {
          $res[]=array("url"=>session_url('dispatch.php?atklevel='.$i),
                       "title"=>$title);
        }
        else
        {
          $res[]=array("url"=>"", "title"=>$title);
        }
      }
      return $res;
    }
    
    /**
     * Gets the node and the descriptor for the current item
     * and returns a trace of that.
     *
     * So for instance, if we were adding a grade to a student,
     * it would show:
     * Student [ Teknoman ] - Grade [ A+ ]
     * @return String The descriptortrace
     */
    function descriptorTrace()
    {
      global $g_sessionData;
      
      $ui = &atknew("atk.ui.atkui");   
     
      $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];
      $res = array();
      for ($i=0;$i<count($stack)-1;$i++)
      {
        if (isset($stack[$i]["descriptor"]))
        {
          $node = explode(".",$stack[$i]["atknodetype"]);
          $res[] = atktext($node[1],$node[0])." [ ".$stack[$i]["descriptor"]." ] ";
        }
      }
      return $res;
    }

    /**
     * Verify the integrity of the session stack.
     *
     * Fixes the stack in case a user opens links in a new window, which would
     * normally confuse the session manager. In the case we detect a new
     * window, we fork the session stack so both windows have their own
     * stacks.
     * @access private
     */
    function _verifyStackIntegrity()
    {
      global $g_sessionData, $atkstackid, $atklevel, $atkprevlevel;
      $stack = "";
      
      if (isset($g_sessionData[$this->m_namespace]["stack"][atkStackID()]))
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
    
    /**
     * Get direct access to the php session. 
     * 
     * The advantage of using atkSessionManager::getSession over php's 
     * $_SESSION directly, is that this method is application aware.
     * If multiple applications are stored on the same server, and each has
     * a unique $config_identifier set, the session returned by this method
     * is specific to only the current application, whereas php's $_SESSION
     * is global on the url where the session cookie was set.
     * @static
     * @return array The application aware php session.
     */
    function &getSession()
    {      
      return $_SESSION[atkconfig("identifier")];
    }

  }

  /**
   * Initializes the session.
   *
   * After the session has been initialised with atksession(), the session
   * manager can be used using the global variable $g_sessionManager.
   * Call this function in every file that you want to use atk sessions.
   *
   * @param String $namespace If multiple scripts/applications are
   *                          installed on thesame url, they can each use
   *                          a different namespace to make sure they 
   *                          don't share session data.
   * @param boolean $usestack Tell the sessionmanager to use the session
   *                          stack manager (back/forth navigation in
   *                          screens, remembering vars over multiple
   *                          pages etc). This comes with a slight
   *                          performance impact, so scripts not using
   *                          the stack should pass false here.
   */
  function atksession($namespace = "default", $usestack=true)
  {
    global $ATK_VARS, $g_sessionManager;
    global $atkprevlevel;

    $g_sessionManager = new atkSessionManager($namespace, $usestack);

    atkDataDecode($_REQUEST);
    $ATK_VARS = $_REQUEST;   
        
    $g_sessionManager->session_read($ATK_VARS);
    
    // Escape check
    if (isset($ATK_VARS["atkescape"]) && $ATK_VARS["atkescape"]!="")
    {
      atkimport("atk.atknode");
      atkNode::redirect(atkurldecode($ATK_VARS["atkescape"]));

      $output = &atkOutput::getInstance();
      $output->outputFlush();
      exit;
    }      
    // Back check
    else if (isset($ATK_VARS["atkback"]) && $ATK_VARS["atkback"]!="")
    {
      // When we go back, we go one level deeper than the level we came from.
      atkimport("atk.atknode");
      atkNode::redirect(session_url($_SERVER["PHP_SELF"]."?atklevel=".($atkprevlevel-1)));
      $output = &atkOutput::getInstance();
      $output->outputFlush();
      exit;
    }

  }

  /**
   * Store a variable in the current namespace.
   * @deprecated Use atkSessionManager::getSession() instead, and store
   *             the variable directly in the application session, or
   *             use globalVar() to store a variable in the current
   *             namespace.
   */
  function sessionStore($var, $value)
  {
    global $g_sessionManager;    
    $g_sessionManager->globalVar($var, $value);
  }
  
  /**
   * Load a variable from a namespace.
   * @deprecated Use atkSessionManager::getSession() instead, and load
   *             the variable directly from the application session, or
   *             use getValue() to retrieve a variable from a given namespace.
   */
  function sessionLoad($var, $namespace="")
  {
    global $g_sessionManager;
    return $g_sessionManager->getValue($var, $namespace);
  }

  /**
   * @internal Used by the session manager to retrieve a unique id for the 
   *           current atk stack.
   */
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

  /**
   * Retrieve the current atkLevel of the session stack.
   * 
   * Level 0 is the 'entry screen' of a stack. Any screen deeper from the
   * entry screen (following an edit link for example) has its atklevel
   * increased by 1. This method is useful for checking if a 'back' button
   * should be displayed. A backbutton will work for any screen whose 
   * atklevel is bigger than 0.
   *
   * @return int The current atk level.
   */
  function atkLevel()
  {
    global $atklevel;
    if (!isset($atklevel)||$atklevel=="")
    {
      $atklevel = 0; // assume bottom level.
    }
    return $atklevel;
  }
    
  // Below added in 5.3 but not working right!!
  
  // Function created by timo at frenay dot net taken from: 
  // http://nl.php.net/manual/en/function.session-regenerate-id.php
  
  if (!function_exists('session_regenerate_id')) 
  {
    /**
     * Generates a php combined lcg
     * @return float php combined lcg
     *
     */
    function php_combined_lcg() 
    {
      $tv = gettimeofday();
      $lcg['s1'] = $tv['sec'] ^ (~$tv['usec']);
      $lcg['s2'] = posix_getpid();

      $q = (int) ($lcg['s1'] / 53668);
      $lcg['s1'] = (int) (40014 * ($lcg['s1'] - 53668 * $q) - 12211 * $q);
      if ($lcg['s1'] < 0)
        $lcg['s1'] += 2147483563;

      $q = (int) ($lcg['s2'] / 52774);
      $lcg['s2'] = (int) (40692 * ($lcg['s2'] - 52774 * $q) - 3791 * $q);
      if ($lcg['s2'] < 0)
        $lcg['s2'] += 2147483399;

      $z = (int) ($lcg['s1'] - $lcg['s2']);
      if ($z < 1) 
        $z += 2147483562;
     

      return $z * 4.656613e-10;
    }

    /**
     * Changes the currect session id into a new one
     * this is done for session hijacking
     * return bool true
     *
     */
    function session_regenerate_id() 
    {
      $tv = gettimeofday();
      $buf = sprintf("%.15s%ld%ld%0.8f", $_SERVER['REMOTE_ADDR'], $tv['sec'], $tv['usec'], php_combined_lcg() * 10);
      session_id(md5($buf));
      if (ini_get('session.use_cookies'))
        setcookie(atkconfig("identifier"), session_id(), NULL, '/');
      return TRUE;
    }
  }
    
  /**
   * Checks wether or not the sessionid that was passed along is valid
   * A session id can become invalid because of tampering and would otherwise
   * cause a php error.
   * A valid session id consists of 32 alphanumeric characters.
   * Mind you, an empty sessionid is also valid, 
   * simply because there is no session yet
   * @return bool Wether or not the sessionid is valid
   */
  function isSessionIdValid()
  {           
  	$name = atkconfig("identifier");
  	$sessionid = "";
    global ${$name};

    if (isset($_COOKIE[$name])&&$_COOKIE[$name]) 
    {
      $sessionid = $_COOKIE[$name];
    }
    else 
    {
      $sessionid = ${$name};
    }
    
    if ($sessionid=="") return true;
      
    for ($position=0; $position<32; $position++)
    {
      if (!eregi('[a-zA-Z0-9]', $sessionid[$position])) return false;
    }    
    return true;
  }

?>
