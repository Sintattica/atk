<?php
  /**
   * Tools for session management
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * $Id$
   * $Log$
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
      global $g_sessionData, $atklevel, $g_sessionVars, $g_pageVars, $HTTP_GET_VARS;
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
        
        // and page vars are cleared because they are no longer valid in an upper level..
        foreach($g_pageVars as $var)
        {
          $currentitem[$var]="";
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
    
      foreach($currentitem as $var => $value)
      {
        $postvars[$var] = $value;
      }  
//      var_dump($postvars);
        
      //echo "<hr>STACK: ";
      //var_dump($g_sessionData["stack"]);
      //echo "<hr>";
    }
    
    function stackTrace()
    {
      global $g_sessionData;
      $res = "";
      
      $stack = $g_sessionData["stack"];
      
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
  function atksession()
  {
    global $HTTP_POST_VARS, $HTTP_GET_VARS, $ATK_VARS, $g_sessionManager;
    
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

  $g_sessionManager = new atkSessionManager();
  

?>