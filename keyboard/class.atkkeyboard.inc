<?php

  atkimport("atk.ui.atkpage");
  
  // Define some key shortcut constants. (mind you, these are not actual ascii key values)
  define("KB_UP", 1);
  define("KB_DOWN", 2);
  define("KB_LEFT", 4);
  define("KB_RIGHT", 8);
  define("KB_UPDOWN", KB_UP|KB_DOWN);
  define("KB_LEFTRIGHT", KB_LEFT|KB_RIGHT);
  define("KB_CURSOR", KB_UPDOWN|KB_LEFTRIGHT);
  define("KB_CTRLCURSOR", 16);      
    
  class atkKeyboard
  {
    // WORKAROUND: in php (4.3.1 at least) at least one member var must exist, to make it possible to create singletons.
    var $m_dummy=""; 
    
    function &getInstance()
    {
      static $s_kb;
      if ($s_kb == NULL)
      {
        atkdebug("Creating atkKeyboard instance");
        $s_kb = new atkKeyboard();
      }
      
      return $s_kb;
    }        
    
    function addFormElementHandler($id, $navkeys)
    {      
      $params = array("'".$id."'", 
                      hasFlag($navkeys, KB_UP)?"1":"0",
                      hasFlag($navkeys, KB_DOWN)?"1":"0",
                      hasFlag($navkeys, KB_LEFT)?"1":"0",
                      hasFlag($navkeys, KB_RIGHT)?"1":"0",
                      hasFlag($navkeys, KB_CTRLCURSOR)?"1":"0");
    
      $this->addHandler("atkFEKeyListener", $params);      
    }
    
    function addRecordListHandler($id, $highlight, $reccount)
    {
      // TODO/FIXME: we can't handle a highlight color per row yet, because javascript
      // does not know the highlight color of each row.
      $params = array("'".$id."'", "'".$highlight."'", $reccount);
      $this->addHandler("atkRLKeyListener", $params);
      
      // atkrlkeylistener must be loaded after the main addHandler, which loads keyboardhandler.
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/keyboard/javascript/class.atkrlkeylistener.js");
    }
    
    function addHandler($handlertype, $params)
    {    
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/keyboard/javascript/keyboardhandler.js");
      $page->register_loadscript("kb_init();\n");
      $page->register_loadscript("kb_addListener(new $handlertype(".implode(",",$params)."));");
    }
  }
?>