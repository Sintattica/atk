<?php
  /**
   * Tools for session management
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * $Id$
   * $Log$
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
   
  session_start();
  session_register("g_sessionData");           
  
  define(SESSION_DEFAULT,0);
  define(SESSION_NEW,1);
  define(SESSION_NESTED,2);
  define(SESSION_BACK,3);
  
  class atkSessionManager
  {
    function atkSessionManager()
    {
    }
    
    function session_read(&$postvars)
    {
      $this->globalscope($postvars);           
      $this->stackscope($postvars);
    }
    
    function globalVar($var, $value="")
    {
      global $g_sessionData;
      
      if ($value=="") $value = $GLOBALS[$var];
      
      $g_sessionData["globals"][$var] = $value;
    }
    
    function stackVar($var, $value="")
    {
      global $g_sessionData, $atklevel;
            
      $currentitem = &$g_sessionData["stack"][$atklevel];
      
      $currentitem[$var] = $value;
      
    }
    
    function globalscope(&$postvars)
    {
      global $g_sessionData;
                  
      $current = &$g_sessionData["globals"];
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
          
      //var_dump($g_sessionData["globals"]);
    }
    
    function stackscope(&$postvars)
    {
      global $g_sessionData, $atklevel, $g_sessionVars;
      if (!isset($atklevel)||$atklevel=="") $atklevel=0;
      atkdebug("ATKLEVEL: ".$atklevel);
            
      $stack = &$g_sessionData["stack"];            
            
           
      if ($atklevel==-1||!is_array($stack))
      {
        atkdebug("Cleaning stack");
        $stack = array();
        $atklevel = 0;
      }
      
      $currentitem = $stack[$atklevel];
                
      if (!isset($currentitem)||$currentitem=="")
      {
        atkdebug("New level on session stack");
        // new level.. always based on the previous level        
        //$newstackitem = array();
        $currentitem = $stack[count($stack)-1];
        
        // Posted vars always overwrite anything in the current session..
        foreach($g_sessionVars as $var)
        {
          if (isset($postvars[$var])&&$postvars[$var]!="")
          {
            $currentitem[$var] = $postvars[$var];
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
        // Posted vars always overwrite anything in the current session..
        foreach($g_sessionVars as $var)
        {
          if (isset($postvars[$var])&&$postvars[$var]!="")
          {
            $currentitem[$var] = $postvars[$var];
          }
        }
      }
                
         
      foreach($currentitem as $var => $value)
      {
        $postvars[$var] = $value;
      }                             
        
      //echo "<hr>";
      //var_dump($g_sessionData["stack"]);
    }


  }

  $g_sessionManager = new atkSessionManager();
  

?>
