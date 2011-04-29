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
   * @version $Revision: 6313 $
   * $Id$
   */

  /**
   * @internal initialization
   */
  $config_identifier = atkconfig("identifier");
  if (empty($config_identifier)) $config_identifier = "default";

  if (atkconfig('session_init',true) && atksession_init())
  {
    // backwardscompatibility hacks. g_sessionData and g_sessionData are obsolete actually.
    // You can use $session = &atkSessionManager::getSession() now, and you'll have a
    // session enabled, multi-app array in which you can store whatever you like.
    // There are old applications however that still use $g_sessionData, so I'll
    // leave it in place for now.
    $GLOBALS['g_sessionData'] = & $_SESSION[atkconfig('identifier')];
  }

  define("SESSION_DEFAULT",0); // stay at current stacklevel
  define("SESSION_NEW",1);     // new stack
  define("SESSION_NESTED",2);  // new item on current stack
  define("SESSION_BACK",3);    // move one level down on stack
  define("SESSION_REPLACE",4); // replace current stacklevel
  define("SESSION_PARTIAL",5); // same as replace, but ignore atknodetype and atkaction

  if (isset($_REQUEST["atklevel"]))    $atklevel    = trim($_REQUEST["atklevel"]);
  if (isset($_REQUEST["atkprevlevel"]))$atkprevlevel= trim($_REQUEST["atkprevlevel"]);
  if (isset($_REQUEST["atkstackid"]))  $atkstackid  = trim($_REQUEST["atkstackid"]);

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
   */
  class atkSessionManager
  {
    /**
     * @access private
     * @var String
     */
    private $m_namespace;

    /**
     * @access private
     * @var boolean
     */
    private $m_escapemode = false; // are we escaping?

    /**
     * @access private
     * @var boolean
     */
    private $m_usestack = true; // should we use a session stack?

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
    public function atkSessionManager($namespace, $usestack=true)
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
    public function getNameSpace()
    {
      return $this->m_namespace;
    }

    /**
     * Read session variables from the stack and the global scope.
     * @access private
     * @param array $postvars Any variables passed in the http request.
     */
    public function session_read(&$postvars)
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
    public function globalVar($var, $value="", $no_namespace=false)
    {
      global $g_sessionData;

      if ($value=="" && isset($_REQUEST[$var])) $value = $_REQUEST[$var];

      if ($no_namespace) $g_sessionData["globals"][$var] = $value;
      else               $g_sessionData[$this->m_namespace]["globals"][$var] = $value;

      return $value;
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
    public function getValue($var, $namespace="")
    {
      global $g_sessionData;
      if ($namespace=="globals")
        return isset($g_sessionData["globals"][$var]) ? $g_sessionData["globals"][$var] : null;
      else if ($namespace!="")
        return isset($g_sessionData[$namespace]["globals"][$var]) ? $g_sessionData[$namespace]["globals"][$var] : null;
      else
        return isset($g_sessionData[$this->m_namespace]["globals"][$var]) ? $g_sessionData[$this->m_namespace]["globals"][$var] : null;
    }

    /**
     * Store a variable in the session stack. The variable is available in the
     * current page (even after reload), and any screen that is deeper on the
     * session stack.
     *
     * The method can be used to transparantly both store and retrieve the value.
     * If a value gets passed in a url, the following statement is useful:
     *
     * <code>
     *   $view = $g_sessionManager->stackVar("view");
     * </code>
     *
     * This statement makes sure that $view is always filled. If view is passed
     * in the url, it is stored as the new default stack value. If it's not
     * passed in the url, the last known value is retrieved from the session.
     *
     * Also note that if you set a stackvar to A in level 0, then in level 1
     * reset it to B, when you return to level 0, the stackvar will still be A.
     * However for level 1 and deeper it will be B.
     *
     * @param String $var   The name of the variable to store.
     * @param mixed  $value The value to store. If omitted, the session manager
     *                      tries to read the value from the http request.
     * @param int    $level Get/Set var on this level, will be current level by
     *                      default.
     * @return mixed The current value in the session stack.
     */
    public function stackVar($var, $value = '', $level = null)
    {
      if ($this->m_escapemode) return null;

      // If no level is supplied we use the var from the current level
      if ($level === null)
        $level = atkLevel();

      $sessionData = &$this->getSession();
      $currentitem = &$sessionData[$this->m_namespace]["stack"][atkStackID()][$level];
      if (!is_array($currentitem)) return null;

      if ($level === atkLevel() && $value==="" && atkarraynvl($_REQUEST, $var, "")!=="")
      {
        // Only read the value of the stack var from the request if this is the first
        // call to stackVar for this var in this request without an explicit value. If
        // we would this for every call without an explicit value we would overwrite values
        // that are set somewhere between those calls.
        static $requestStackVars = array();
        if (!in_array($var, $requestStackVars))
        {
          $value = $_REQUEST[$var];
          $requestStackVars[] = $var;
        }
      }

      if ($value!=="")
      {
        $currentitem[$var] = $value;
      }

      if (!is_array(atkArrayNvl($currentitem, "defined_stackvars")) || !in_array($var, $currentitem["defined_stackvars"]))
      {
        $currentitem["defined_stackvars"][] = $var;
      }
      // We always return the current value..
      return atkarraynvl($currentitem, $var);
    }

    /**
     * Store a global variable for the current stack in the session.
     * Unlike stackvars, this variable occurs only once for a given stack.
     *
     * For example with a stackvar, if you store a variable x in level 0,
     * then in level 1 you modify that variable, the variable in level 0
     * will not be modified. With a globalStackVar it will.
     *
     * @param string $var   Variable name
     * @param mixed  $value Variable value
     * @return mixed Value of the global stackvar
     */
    public function globalStackVar($var, $value="")
    {
      if (!$var || $this->m_escapemode) return null;

      $sessionData = &$this->getSession();
      $top_stack_level = &$sessionData[$this->m_namespace]['globals']['#STACK#'][atkStackID()];
      if (!is_array($top_stack_level)) $top_stack_level = array();

      if ($value==="")
      {
        if (atkarraynvl($_REQUEST, $var, "")!=="")
        {
          $value = $_REQUEST[$var];
        }
        else if ($this->stackVar($var))
        {
          $value = $this->stackVar($var);
        }
      }

      if ($value!=="")
      {
        $top_stack_level[$var] = $value;
      }
      return atkarraynvl($top_stack_level, $var);
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
    public function pageVar($var, $value="")
    {
      if (!$this->m_escapemode)
      {
        global $g_sessionData;

        $currentitem = &$g_sessionData[$this->m_namespace]["stack"][atkStackID()][atkLevel()];

        if ($value=="")
        {
          if (isset($_REQUEST[$var]))
          {
            atkdebug("Setting current item");
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
        if (isset($currentitem[$var]))
        {
          return $currentitem[$var];
        }
        return "";
      }
    }

    /**
     * Process the global variable scope.
     * @access private
     * @param array $postvars The http request variables.
     */
    protected function _globalscope(&$postvars)
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
     * Update the last modified timestamp for the curren stack.
     */
    protected function _touchCurrentStack()
    {
      global $g_sessionData;
      $g_sessionData[$this->m_namespace]["stack_stamp"][atkStackId()] = time();
    }

    /**
     * Removes any stacks which have been inactive for a period >
     * atkconfig('session_max_stack_inactivity_period').
     */
    protected function _removeExpiredStacks()
    {
      global $g_sessionData;

      $maxAge = atkconfig('session_max_stack_inactivity_period', 0);
      if ($maxAge <= 0)
      {
        atkwarning(__METHOD__.': removing expired stacks disabled, enable by setting $config_session_max_stack_inactivity_period to a value > 0');
        return;
      }

      $now = time();
      $stacks = &$g_sessionData[$this->m_namespace]['stack'];
      $stackStamps = $g_sessionData[$this->m_namespace]["stack_stamp"];
      $stackIds = array_keys($stacks);
      $removed = false;

      foreach ($stackIds as $stackId)
      {
        // don't remove the current stack or stacks that are, for some reason, not stamped
        if ($stackId == atkStackId() || !isset($stackStamps[$stackId]))
        {
          continue;
        }

        $stamp = $stackStamps[$stackId];
        $age = $now - $stamp;

        if ($age > $maxAge)
        {
          atkdebug(__METHOD__.': removing expired stack "'.$stackId.'" (age '.$age.'s)');
          unset($stacks[$stackId]);
          unset($stackStamps[$stackId]);
          $removed = true;
        }
      }

      if (!$removed)
      {
        atkdebug(__METHOD__.': no expired stacks, nothing removed');
      }
    }

    /**
     * Process the variable stack scope (pagevars, stackvars).
     * @access private
     * @param array $postvars The http request variables.
     */
    protected function _stackscope(&$postvars)
    {
      global $g_sessionData, $atklevel;

      // session vars are valid until they are set to something else. if you go a session level higher,
      // the next level will still contain these vars (unless overriden in the url)
      $sessionVars = array("atknodetype","atkfilter","atkaction","atkpkret",
                           "atkcontroller", "atkwizardpanelindex", "atkwizardaction",
                           'atkstore', 'atkstore_key');

      // pagevars are valid on a page. if you go a session level higher, the pagevars are no longer
      // visible until you return. if the name ends in a * the pagevar is treated as an array that
      // needs to be merged recursive with new postvar values
      $pageVars = array("atkdg*","atkdgsession","atksearch","atkselector",
                        "atksearchmode","atkorderby","atkstartat",
                        "atklimit","atktarget","atkformdata","atktree",
                        "atksuppress","atktab","atksmartsearch","atkindex");

      // lockedvars are session or page vars that will not be overwritten in partial mode
      // e.g., the values that are already known in the session will be used
      $lockedVars = array('atknodetype', 'atkaction', 'atkselector');

      // Mental note: We have an atkLevel() function for retrieving the atklevel,
      // but we use the global var itself here, because it gets modified in
      // the stackscope function.

      if (!isset($atklevel)||$atklevel=="") $atklevel=0;

      atkdebug("ATKLevel: ".$atklevel);

      if ($this->_verifyStackIntegrity() && $atklevel == -1)
      {
        // New stack, new stackid, if level = -1.
        $stackid = atkStackID($atklevel == -1);
      }
      else
      {
        $stackid = atkStackID();
      }

      $stack = &$g_sessionData[$this->m_namespace]["stack"][$stackid];

      // garbage collect
      $this->_touchCurrentStack();
      $this->_removeExpiredStacks();

      // Prevent going more than 1 level above the current stack top which
      // causes a new stackitem to be pushed onto the stack at the wrong
      // location.
      if ($atklevel > count($stack))
      {
        atkdebug("Requested ATKLevel (".$atklevel.") too high for stack, lowering to ".count($stack));
        $atklevel = count($stack);
      }

      if (isset($postvars["atkescape"]) && $postvars["atkescape"]!="")
      {
        $this->m_escapemode = true;
        atkdebug("ATK session escapemode");

        $currentitem = &$stack[count($stack)-1];

        atkdebug("Saving formdata in session");

        unset($currentitem['atkreject']); // clear old reject info

        $atkformdata = array();
        foreach(array_keys($postvars) as $varname)
        {
          // Only save formdata itself, hence no $atk.. variables.
          // Except atktab because it could be changed in the page load.
          // but why don't we save all page vars here? What is the reason for
          // not doing this? TODO: Ask Ivo.
          if (substr($varname,0,3)!="atk" || $varname == 'atktab')
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

        // finally, reset atkescape to prevent atk from keeping escaping upon return
        unset($currentitem['atkescape']);
      }
      else
      {
        // partial mode?
        $partial = false;

        if ($atklevel==-1||!is_array($stack)) // SESSION_NEW
        {
          atkdebug("Cleaning stack");
          $stack = array();
          $atklevel = 0;
        }
        else if ($atklevel==-2) // SESSION_REPLACE
        {
          // Replace top level.
          array_pop($stack);

          // Note that the atklevel is now -2. This is actually wrong. We are at
          // some level in the stack. We can determine the real level by
          // counting the stack.
          $atklevel = count($stack);
        }
        else if ($atklevel == -3) // SESSION_PARTIAL
        {
          $partial = true;

          // Note that the atklevel is now -3. This is actually wrong. We are at
          // some level in the stack. We can determine the real level by
          // counting the stack.
          $atklevel = count($stack) - 1;
        }

        if (isset($stack[$atklevel])) $currentitem = $stack[$atklevel];

        if (!isset($currentitem)||$currentitem=="")
        {
          atkdebug("New level on session stack");
          // Initialise
          $currentitem = array();
          // new level.. always based on the previous level
          if (isset($stack[count($stack)-1])) $copieditem = $stack[count($stack)-1];

          if (isset($copieditem) && is_array($copieditem))
          {
            foreach($copieditem as $key=>$value)
            {
              if (in_array($key, $sessionVars) ||
                  (isset($copieditem["defined_stackvars"]) &&
                   is_array($copieditem["defined_stackvars"]) &&
                   in_array($key, $copieditem["defined_stackvars"])))
              {
                $currentitem[$key] = $value;
              }
            }

            if (isset($copieditem["defined_stackvars"]))
            {
              $currentitem["defined_stackvars"] = $copieditem["defined_stackvars"];
            }
          }

          // Posted vars always overwrite anything in the current session..
          foreach(array_merge($pageVars, $sessionVars) as $var)
          {
            $recursive = $var{strlen($var) - 1} == '*';
            $var = $recursive ? substr($var, 0, -1) : $var;

            if (isset($postvars[$var]) && $postvars[$var]!="")
            {
              if ($postvars[$var]=="clear")
              {
                $currentitem[$var] = "";
              }
              else
              {
                if ($recursive && is_array($currentitem[$var]) && is_array($postvars[$var]))
                {
                  $currentitem[$var] = array_merge_recursive($currentitem[$var], $postvars[$var]);
                }
                else
                {
                  $currentitem[$var] = $postvars[$var];
                }
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
            $recursive = $var{strlen($var) - 1} == '*';
            $var = $recursive ? substr($var, 0, -1) : $var;

            if (isset($postvars[$var]) && count($postvars[$var])>0 && (!$partial || !in_array($var, $lockedVars)))
            {
              if ($recursive && is_array($currentitem[$var]) && is_array($postvars[$var]))
              {
                $currentitem[$var] = atk_array_merge_recursive($currentitem[$var], $postvars[$var]);
              }
              else
              {
                $currentitem[$var] = $postvars[$var];
              }
            }
          }

          // page vars must overwrite the current stack..
          $stack[$atklevel] = &$currentitem;

          // session vars need not be remembered..
          foreach ($sessionVars as $var)
          {
            if (isset($postvars[$var]) && count($postvars[$var])>0 && (!$partial || !in_array($var, $lockedVars)))
            {
              $currentitem[$var] = $postvars[$var];
            }
          }
        }

        if (isset($currentitem["atkformdata"]) && is_array($currentitem["atkformdata"]))
        {
          atkdebug("Session formdata present");
          foreach($currentitem['atkformdata'] as $var => $value)
          {

            // don't override what was passed in the url.
            if (!isset($postvars[$var]))
            {
              $postvars[$var] = $value;
            }
            else if (is_array($postvars[$var]) && is_array($value))
            {
              // Formdata that was posted earlier needs to be merged with the current
              // formdata. We use a custom array_merge here to preserve key=>value pairs.
              $postvars[$var] = atk_array_merge_keys($value, $postvars[$var]);
            }
          }

          // We leave atkformdata in the current stack entry untouched so that
          // when the stack might be forked of whatsoever the form data is still
          // present. However, this data should not be directly accessed in the node!
        }

        if (is_array($currentitem))
        {
          foreach($currentitem as $var => $value)
          {
            $recursive = in_array("{$var}*", $pageVars);

            // don't override what was passed in the url except for
            // recursive mergeable pagevars
            if ($recursive || !isset($postvars[$var]))
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
    public function stackTrace()
    {
      global $g_sessionData;

      $ui = &atkinstance("atk.ui.atkui");

      $res = array();
      $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];

      for ($i=0;$i<count($stack);$i++)
      {
        if (!isset($stack[$i]["atknodetype"])) continue;

        $node = $stack[$i]["atknodetype"];
        $module = getNodeModule($node);
        $type = getNodeType($node);
        $action = $stack[$i]["atkaction"];
        $title = $ui->title($module, $type, $action);
        $descriptor = atkArrayNvl($stack[$i], "descriptor", "");

        $entry = array(
          'url' => '',
          'title' => $title,
          'descriptor' => $descriptor,
          'node' => $node,
          'nodetitle' => atktext($type, $module, $type),
          'action' => $action,
          'actiontitle' => atktext($action, $module, $type)
        );

        if ($i<count($stack)-1)
        {
          $entry['url'] = session_url(getDispatchFile().'?atklevel='.$i);
        }

        $res[] = $entry;
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
    public function descriptorTrace()
    {
      global $g_sessionData;

      $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];
      $res = array();

      $stackcount = count($stack);
      for ($i=0;$i<$stackcount;$i++)
      {
        if (isset($stack[$i]["descriptor"]) || $i == ($stackcount-1))
        {
          if($stack[$i]["atknodetype"] != "")
          {
            $node = &getNode($stack[$i]["atknodetype"]);
            $module = getNodeModule($stack[$i]["atknodetype"]);
            $nodename = getNodeType($stack[$i]["atknodetype"]);
          }

          if (is_object($node))
          {
            $ui = &atkinstance("atk.ui.atkui");
            $txt = $ui->title($module, $nodename);
          }
          else
          {
            $txt = atktext($nodename,$module);
          }

          $res[] = $txt.(isset($stack[$i]["descriptor"])?" [ {$stack[$i]['descriptor']} ] ":"");
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
     *
     * @access private
     *
     * @return boolean stack integrity ok? (false means we created a new stack)
     */
    protected function _verifyStackIntegrity()
    {
      global $g_sessionData, $atklevel, $atkprevlevel;
      $stack = "";

      if (isset($g_sessionData[$this->m_namespace]["stack"][atkStackID()]))
        $stack = $g_sessionData[$this->m_namespace]["stack"][atkStackID()];
      if (!is_array($stack)) $prevlevelfromstack = 0;
      else $prevlevelfromstack = count($stack)-1;

      $oldStackId = atkStackID();

      if ($atkprevlevel!=$prevlevelfromstack)
      {
        // What we think we came from (as indicated in the url by atkprevlevel)
        // and what the REAL situation on the stack was when we got here (prevlevelfromstack)
        // is different. Let's fork the stack.
        // @TODO: If an error occurs and forking is required, the rejection info is not forked right, since it is currently stored
        //        in session['atkreject'] and not directly in the stack. See also atk/handlers/class.atkactionhandler.inc.
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

        // Copy the global stackvars for the stack too.
        if (isset($g_sessionData[$this->m_namespace]['globals']['#STACK#'][$oldStackId])) {
          $g_sessionData[$this->m_namespace]['globals']['#STACK#'][$newid] =
            $g_sessionData[$this->m_namespace]['globals']['#STACK#'][$oldStackId];
        }

        return false;
      }

      return true;
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
    static public function &getSession()
    {
      if (!isset($_SESSION[atkconfig("identifier")]) || !is_array($_SESSION[atkconfig("identifier")]))
        $_SESSION[atkconfig("identifier")] = array();
      return $_SESSION[atkconfig("identifier")];
    }

    /**
     * Calculate a new session level based on current level and
     * a passed sessionstatus.
     * @param var $sessionstatus the session flags
     *            (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *             SESSION_NESTED|SESSION_BACK)
     * @param int $levelskip how many levels to skip when we use SESSION_BACK,
     *            default 1
     * @static
     * @return int the new session level
     */
    static public function newLevel($sessionstatus=SESSION_DEFAULT, $levelskip=null)
    {
      $currentlevel = atkLevel();

      $newlevel = -1;

      switch ($sessionstatus)
      {
        case SESSION_NEW:
        {
          $newlevel = -1;
          break;
        }
        case SESSION_REPLACE:
        {
          $newlevel = -2;
          break;
        }
        case SESSION_PARTIAL:
        {
          $newlevel = -3;
          break;
        }
        case SESSION_NESTED:
        {
          $newlevel = $currentlevel+1;
          break;
        }
        case SESSION_BACK:
        {
          if ($levelskip === null)
            $levelskip = 1;

          $newlevel = max(0, $currentlevel-$levelskip);
          break;
        }
        default:
        {
          $newlevel = $currentlevel;
        }
      }
      return $newlevel;
    }

    /**
     * Calculate old session level based on current level and
     * a passed sessionstatus.
     * @param var $sessionstatus the session flags
     *            (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *             SESSION_NESTED|SESSION_BACK)
     * @param int $levelskip how many levels to skip when we use SESSION_REPLACE,
     * @static
     * @return int the new session level
     */
    static public function oldLevel($sessionstatus = SESSION_DEFAULT, $levelskip = null)
    {
      $level = atkLevel();
      if ($sessionstatus == SESSION_REPLACE && $levelskip !== null)
      {
        $level = $level - $levelskip;
      }

      return max($level, 0);
    }

    /**
     * Adds session information to a form
     *
     * @param int $sessionstatus the session flags
     *            (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *             SESSION_NESTED|SESSION_BACK)
     * @param int $returnbehaviour When SESSION_NESTED is used, this is used to
     *            indicate where to return to.
     * @param string $fieldprefix
     * @return string the HTML formcode with the session info
     */
    public function formState($sessionstatus=SESSION_DEFAULT, $returnbehaviour=NULL, $fieldprefix='')
    {
      global $g_stickyurl;

      $res = "";

      $newlevel = atkSessionManager::newLevel($sessionstatus);

      if ($newlevel!=0)
      {
        $res= '<input type="hidden" name="atklevel" value="'.$newlevel.'" />';
      }
      $res.='<input type="hidden" name="atkprevlevel" value="'.atkLevel().'" />';

      if ($sessionstatus!=SESSION_NEW)
      {
        $res.='<input type="hidden" name="atkstackid" value="'.atkStackID().'" />';
      }

      if (!is_null($returnbehaviour))
      {
        $res.= '<input type="hidden" name="'.$fieldprefix.'atkreturnbehaviour" value="'.$returnbehaviour.'" />';
      }

      $res .= '<input type="hidden" name="'.session_name().'" value="'.session_id().'" />';
      $res .= '<input type="hidden" name="atkescape" value="" autocomplete="off" />';

      for ($i=0;$i<count($g_stickyurl);$i++)
      {
        $value = $GLOBALS[$g_stickyurl[$i]];
        if ($value!="")
        {
          $res.="\n".'<input type="hidden" name="'.$g_stickyurl[$i].'" value="'.$value.'" />';
        }
      }
      return $res;
    }


    /**
     * Gets the session vars
     * @param var $sessionstatus the session flags
     *                           (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *                            SESSION_NESTED|SESSION_BACK)
     * @param int $levelskip      the amount of levels to skip if we go back
     * @param string $url         the URL
     * @return array the vars of the session
     */
    static public function sessionVars($sessionstatus=SESSION_DEFAULT, $levelskip=null, $url="")
    {
      global $g_stickyurl;

      $newlevel = atkSessionManager::newLevel($sessionstatus, $levelskip);
      $oldlevel = atkSessionManager::oldLevel($sessionstatus, $levelskip);

      $vars="";
     // atklevel is already set manually, we don't append it..
      if ($newlevel!=0 && !strpos($url,"atklevel=")>0)
      {
        $vars.= "atklevel=".$newlevel."&";
      }
      $vars.= "atkprevlevel=".$oldlevel;
      if ($sessionstatus!=SESSION_NEW)
      {
        $vars.="&atkstackid=".atkStackID();
      }
      $vars.= "&".SID;

      for ($i=0;$i<count($g_stickyurl);$i++)
      {
        $value = $GLOBALS[$g_stickyurl[$i]];
        if ($value!="")
        {
          if (substr($vars,-1)!="&") $vars.="&";
          $vars.=$g_stickyurl[$i]."=".$value;
        }
      }
      return $vars;
    }

    /**
     * Makes a session-aware URL.
     *
     * @param string $url         the url to make session-aware
     * @param var $sessionstatus  the session flags
     *                            (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *                             SESSION_NESTED|SESSION_BACK)
     * @param int $levelskip      the amount of levels to skip if we go back
     * @static
     * @return string the session aware URL
     */
    static public function sessionUrl($url,$sessionstatus=SESSION_DEFAULT, $levelskip=null)
    {
      if (strpos($url,"?")!==false)
      {
        $start = "&";
      }
      else
      {
        $start = "?";
      }

      $url.=$start;

      $url.=atkSessionManager::sessionVars($sessionstatus, $levelskip, $url);

      return $url;
    }


    /**
     * Makes a session-aware href url.
     * When using hrefs in the editform, you can set saveform to true. This will save your
     * form variables in the session and restore them whenever you come back.
     *
     * @param string $url         the url to make session aware
     * @param string $name        the name to display (will not be escaped!)
     * @param var $sessionstatus  the session flags
     *                            (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *                             SESSION_NESTED|SESSION_BACK)
     * @param bool $saveform      wether or not to save the form
     * @param string $extraprops  extra props you can add in the link such as
     *                            'onChange="doSomething()"'
     * @static
     * @return string the HTML link for the session aware URI
     */
    static public function href($url,$name="",$sessionstatus=SESSION_DEFAULT, $saveform=false, $extraprops="")
    {
      if ($saveform)
      {
        $str = 'atkSubmit("'.atkurlencode(atkSessionManager::sessionUrl($url,$sessionstatus)).'");';
        return "<a href=\"javascript:void(0)\" onclick=\"".atk_htmlentities($str)."\" ".$extraprops.">".$name."</a>";        
      }
      else
      {
        $str = atkSessionManager::sessionUrl($url,$sessionstatus);
        return "<a href=\"".atk_htmlentities($str)."\" ".$extraprops.">".$name."</a>";
      }
    }
  }

  /**
   * Initialize and start a PHP session.
   * Without this smart debugging and the securitymanager will not function correctly.
   */
  function atksession_init()
  {
    if (php_sapi_name() == 'cli')
    {
      return false; // command-line
    }

    $cookie_params = session_get_cookie_params();
    $cookiepath   = atkconfig("application_root");
    $cookiedomain = (atkconfig("cookiedomain") != "") ? atkconfig("cookiedomain") : NULL;
    session_set_cookie_params($cookie_params["lifetime"], $cookiepath, $cookiedomain);

    // set cache expire (if function exists, or show upgrade hint if not)
    if (function_exists("session_cache_expire")) session_cache_expire(atkconfig("session_cache_expire"));
    else atkdebug("session_cache_expire function does not exist, please upgrade to the latest stable php version (at least 4.2.x)", DEBUG_WARNING);

    // set the cache limiter (used for caching)
    session_cache_limiter(atkconfig("session_cache_limiter"));

    // If somehow the sessionid is unclean (searchengine bots have been known to mangle sessionids)
    // we don't have a session...
    if (isValidSessionId())
    {
      $sessionname = atkconfig("session_name");
      if (!$sessionname) $sessionname = atkconfig('identifier');
      session_name($sessionname);
      session_start();
      return true;
    } else atkwarning("Not a valid session!");
    return false;
  }

  /**
   * Initializes the sessionmanager.
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
    global $ATK_VARS, $g_sessionManager, $g_sessionData, $atkprevlevel;

    $g_sessionManager = atknew('atk.session.atksessionmanager', $namespace, $usestack);

    atkDataDecode($_REQUEST);
    $ATK_VARS = array_merge($_GET, $_POST);
    atkDataDecode($ATK_VARS);
    if (array_key_exists('atkfieldprefix',$ATK_VARS) && $ATK_VARS['atkfieldprefix'] != '') $ATK_VARS = $ATK_VARS[$ATK_VARS['atkfieldprefix']];

    $g_sessionManager->session_read($ATK_VARS);

    // Escape check
    if (isset($_REQUEST["atkescape"]) && $_REQUEST["atkescape"]!="")
    {
      atkimport("atk.atknode");
      atkNode::redirect(atkurldecode($_REQUEST["atkescape"]));
      atkOutput::getInstance()->outputFlush();
      exit;
    }
    // Nested URL check
    else if (isset($_REQUEST["atknested"]) && $_REQUEST["atknested"] != "")
    {
      atkimport("atk.atknode");
      atkNode::redirect(session_url($_REQUEST["atknested"], SESSION_NESTED));
      atkOutput::getInstance()->outputFlush();
      exit;
    }
    // Back check
    else if (isset($ATK_VARS["atkback"]) && $ATK_VARS["atkback"]!="")
    {
      // When we go back, we go one level deeper than the level we came from.
      atkimport("atk.atknode");
      atkNode::redirect(session_url(atkSelf()."?atklevel=".($atkprevlevel-1)));
      atkOutput::getInstance()->outputFlush();
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
    if (!isset($g_sessionManager)) throw new Exception("Trying to store '$var' in session, but no sessionmanager present! Did you start a session (atksession())?");
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
    if (!isset($g_sessionManager)) throw new Exception("Trying to load '$var' (namespace: '$namespace') from session, but no sessionmanager present! Did you start a session (atksession())?");
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

  function atkPrevLevel()
  {
    global $atkprevlevel;
    if (!isset($atkprevlevel)||$atkprevlevel=="")
    {
      $atkprevlevel = 0; // assume bottom level.
    }
    return $atkprevlevel;
  }

  /**
   * Checks wether or not the sessionid that was passed along is valid
   * A session id can become invalid because of tampering and would otherwise
   * cause a php error.
   * A valid session id consists only of alphanumeric characters.
   * Mind you, an empty sessionid is also valid,
   * simply because there is no session yet
   * @return bool Wether or not the sessionid is valid
   */
  function isValidSessionId()
  {
    $name = atkconfig("session_name");
    if (empty($name))
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

    if (!preg_match('/^[a-z0-9:-]+$/i', $sessionid)) return false;

    return true;
  }

  /**
   * Returns the sessionmanager.
   * @return atkSessionManager Session manager
   */
  function &atkGetSessionManager()
  {
    global $g_sessionManager;
    return $g_sessionManager;
  }

?>
