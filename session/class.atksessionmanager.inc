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
    
    function getValue($var, $namespace="")
    {
      global $g_sessionData;
      if ($namespace=="globals") return $g_sessionData["globals"][$var];
      else if ($namespace!="") return $g_sessionData[$namespace]["globals"][$var];
      return $g_sessionData[$this->m_namespace]["globals"][$var];
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
      if (!is_array($currentitem["defined_stackvars"])||!in_array($var, $currentitem["defined_stackvars"]))
      {
        $currentitem["defined_stackvars"][] = $var;
      }
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
      if (!is_array($currentitem["defined_pagevars"])||!in_array($var, $currentitem["defined_pagevars"]))
      {
        $currentitem["defined_pagevars"][] = $var;
      }
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
      else if ($atklevel==-2)
      {
        // Replace top level.
        array_pop($stack);
        
        // Note that the atklevel is now -2. This is actually wrong. We are at 
        // some level in the stack. We can determine the real level by 
        // counting the stack.
        $atklevel = count($stack);
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
//      var_dump($postvars);
        
      //echo "<hr>STACK: ";
      //var_dump($g_sessionData[$this->m_namespace]["stack"]);
    //  echo "<hr>Final stack:<pre>";var_dump($g_sessionData[$this->m_namespace]["stack"]);echo '</pre>';
  //    global $QUERY_STRING;
//    echo $QUERY_STRING;
    
    }
    
    function stackTrace()
    {
      global $g_sessionData, $g_layout;
      $res = "";
      $stack = $g_sessionData[$this->m_namespace]["stack"];
      
      // we only show the last 4 elements of the stack
      if (count($stack)>4) $res = "... | ";
      
      for ($i=max(0,count($stack)-4);$i<count($stack);$i++)
      {    
        $mod = getNodeModule($stack[$i]["atknodetype"]);
        $node = getNodeType($stack[$i]["atknodetype"]);
        $title = text($g_layout->title($mod, $node, $stack[$i]["atkaction"]));
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
  
  function sessionLoad($var, $namespace="")
  {
    global $g_sessionManager;
    return $g_sessionManager->getValue($var, $namespace);      
  }
  
?>
