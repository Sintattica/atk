<?php
  /**
   * Tools for session management
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * $Id$
   * $Log$
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
      // We always return the current value..
      return $currentitem[$var];
      
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
    
    function stackTrace()
    {
      global $g_sessionData;
      $res = "";
      
      $stack = $g_sessionData["stack"];
      
      for ($i=0;$i<count($stack);$i++)
      {
        $title = text("title_".$stack[$i]["atknodetype"]."_".$stack[$i]["atkaction"]);        
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

  $g_sessionManager = new atkSessionManager();
  

?>