<?php
  /**
   * Tools for session management
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * $Id$
   * $Log$
   * Revision 1.23  2001/10/08 11:40:11  ivo
   * Added 2 new functions to session API: sessionLoad and sessionStore.
   *
   * Revision 1.22  2001/07/25 10:33:45  maurice
   * Created a workaround for a bug in php4.0.4
   *
   * Revision 1.21  2001/07/18 16:10:18  ivo
   * New feature: atkToolbar. Works in IE4 (or newer) only.
   *
   * Revision 1.20  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 1.19  2001/06/19 19:29:20  ivo
   * pagevar funktie toegevoegd
   *
   * Revision 1.18  2001/06/19 12:58:09  peter
   * Session cookie parameters changed so the session cookie will not only
   * be domain based but also domain/path based!
   *
   * Revision 1.17  2001/06/19 11:41:14  ivo
   * Global vars can now be namespace-global or completely global.
   *
   * Revision 1.16  2001/06/14 13:48:10  ivo
   * New cool feature: namespaces for sessionmanagement.
   *
   * Revision 1.15  2001/05/30 07:05:43  ivo
   * Removed is_null funtion calls (because they require PHP4.0.4)
   * Moved session management initialisation code to a function.
   *
   * Revision 1.14  2001/05/18 07:49:42  ivo
   * Fixes to sessionmanagement.
   *
   * Revision 1.13  2001/05/17 12:09:11  ivo
   * Improvements to session mangement: search values and page numbers are
   * now remembered if you come back to the admin page.
   *
   * Revision 1.12  2001/05/17 10:27:12  ivo
   * Fixes to session management, new AF_LARGE feature for manytoonerelation.
   * Improved handling of GET_VARS and POST_VARS (requires a new dispatcher)
   *
   * Revision 1.11  2001/05/11 15:24:55  peter
   * Some updates for the module support and some other things...
   *
   * Revision 1.10  2001/05/10 09:42:28  sandy
   * bugfix in menu.php (config menu delimiter) and for the clear posting
   *
   * Revision 1.9  2001/05/10 08:31:01  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 1.8  2001/05/02 15:01:23  ivo
   * Added a maximum (4) to the stacktrace.
   *
   * Revision 1.7  2001/05/02 14:55:46  ivo
   * New cool feature: stacktrace in the topright screen (navigate through
   * pages you visited before)
   *
   * Revision 1.6  2001/05/02 14:21:51  sandy
   * new keyword in the session manager (clean)
   *
   * Revision 1.5  2001/05/02 12:44:21  ivo
   * multilevel session_back feature.
   * new relational features: maxRecords for oneToManyRelation, filter for
   * onetomanyrelation.
   *
   * Revision 1.4  2001/05/01 13:48:22  ivo
   * removed some debugcode.
   *
   * Revision 1.2  2001/05/01 12:17:02  ivo
   * bugfix
   *
   * Revision 1.1  2001/05/01 12:13:19  ivo
   * Added the sessionmanager.
   *
   *
   */        
      
  $cookie_params = session_get_cookie_params();
  session_set_cookie_params($cookie_params["lifetime"], dirname($SCRIPT_NAME));
  session_start();
  session_register("g_sessionData");  
     
  define(SESSION_DEFAULT,0);
  define(SESSION_NEW,1);
  define(SESSION_NESTED,2);
  define(SESSION_BACK,3);

  /* Workaround for a PHP4.0.4 bug */
  $atklevel = trim($atklevel);

  class atkSessionManager
  {
    var $m_namespace; 
  
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
    
    function getValue($var, $no_namespace=false)
    {
      global $g_sessionData;
      if ($no_namespace) return $g_sessionData["globals"][$var];
      else return $g_sessionData[$this->m_namespace]["globals"][$var];
    }
    
    function stackVar($var, $value="")
    {
      global $g_sessionData, $atklevel;
            
      $currentitem = &$g_sessionData[$this->m_namespace]["stack"][$atklevel];

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
      $currentitem["defined_stackvars"][] = $var;
      // We always return the current value..
      return $currentitem[$var];
      
    }        
    
    function pageVar($var, $value="")
    {
      global $g_sessionData, $atklevel;
            
      $currentitem = &$g_sessionData[$this->m_namespace]["stack"][$atklevel];

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
      $currentitem["defined_pagevars"][] = $var;
      // We always return the current value..
      return $currentitem[$var];      
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
      if (!isset($atklevel)||$atklevel=="") $atklevel=0;
      atkdebug("ATKLEVEL : ".$atklevel);
            
      $stack = &$g_sessionData[$this->m_namespace]["stack"];            
            
      if ($atklevel==-1||!is_array($stack))
      {
        atkdebug("Cleaning stack");
        $stack = array();
        $atklevel = 0;
      }
    //echo '<hr>precurrentitemcheck<pre>';
    //var_dump($stack);
    //echo '</pre><hr>';
      
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
        
        /*// and page vars are cleared because they are no longer valid in an upper level..
        foreach($g_pageVars as $var)
        {
          $currentitem[$var]="";
        }
        
        for($i=0;$i<count($currentitem["defined_pagevars"]);$i++)
        {
          $currentitem[$currentitem["defined_pagevars"][$i]]="";
        }*/
                
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
      //echo '<pre>';var_dump($currentitem);
              
        array_push($stack, $currentitem); 
        //var_dump($stack);
        //echo '</pre>';
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
        // Posted vars always overwrite anything in the current session..
        // unless we are escaping.. 
        // if atkescape is set, we store the entire form in the stack        
        $escaped = false;
        if ($postvars["atkescape"]!="")
        {
          atkdebug("Saving formdata in session");
          $escaped = true;
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
          
          // save it in the session stack..
          $stack[$atklevel] = $currentitem;       
        }
        else
        {        
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
      }              
          
      if (is_array($currentitem["atkformdata"]))
      {
        atkdebug("Restoring formdata from session");        
        // atkformdata will be present in postvars.. editForm() has to read them..
      }
    
      if (is_array($currentitem))
      {
        foreach($currentitem as $var => $value)
        {
          $postvars[$var] = $value;
        }  
      }
//      var_dump($postvars);
        
      //echo "<hr>STACK: ";
      //var_dump($g_sessionData[$this->m_namespace]["stack"]);
      //echo "<hr>Final stack:<pre>";var_dump($g_sessionData[$this->m_namespace]["stack"]);echo '</pre>';
    
    }
    
    function stackTrace()
    {
      global $g_sessionData;
      $res = "";
      $stack = $g_sessionData[$this->m_namespace]["stack"];
      
      // we only show the last 4 elements of the stack
      if (count($stack)>4) $res = "... | ";
      
      for ($i=max(0,count($stack)-4);$i<count($stack);$i++)
      {
        $title = text("title_".getNodeType($stack[$i]["atknodetype"])."_".$stack[$i]["atkaction"]);        
        if ($i<count($stack)-1) 
        {
          $res.='<a href="dispatch.php?atklevel='.$i.'">'.$title.'</a> | ';
        }
        else
        {
          $res.=$title;
        }
      }            
      return $res;
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
      atkDataDecode(&$HTTP_POST_VARS);   
      $ATK_VARS = $HTTP_POST_VARS;
    }
    else
    {
      atkDataDecode(&$HTTP_GET_VARS);   
      $ATK_VARS = $HTTP_GET_VARS;
    }
   
    $g_sessionManager->session_read(&$ATK_VARS);
  }  
  
  function sessionStore($var, $value)
  {
    global $g_sessionManager;
    $g_sessionManager->globalVar($var, $value);      
  }
  
  function sessionLoad($var)
  {
    global $g_sessionManager;
    return $g_sessionManager->getValue($var);      
  }
  
?>
