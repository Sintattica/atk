<?php

 /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage keyboard
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 1640 $
   * $Id$
   */     

  /**
   * Imports used by the class..
   */
  atkimport("atk.ui.atkpage");
  
  /**
   * Define some key shortcut constants. (mind you, these are not actual ascii key values)
   */
  define("KB_UP", 1);
  define("KB_DOWN", 2);
  define("KB_LEFT", 4);
  define("KB_RIGHT", 8);
  define("KB_UPDOWN", KB_UP|KB_DOWN);
  define("KB_LEFTRIGHT", KB_LEFT|KB_RIGHT);
  define("KB_CURSOR", KB_UPDOWN|KB_LEFTRIGHT);
  define("KB_CTRLCURSOR", 16);      
    
  /**
   * This class handles keyboard navigation. It is used to register keyboard
   * event handlers. This class is a singleton. Use getInstance() to retrieve
   * the instance.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage keyboard
   *
   */
  class atkKeyboard
  {
    /**
     * WORKAROUND: in php (4.3.1 at least) at least one member var must exist, to make it possible to create singletons.
     * @access private
     */
    var $m_dummy=""; 
    
    /**
     * Get the one and only (singleton) instance of the atkKeyboard class.
     * @return atkKeyboard The singleton instance.
     */
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
    
    /**
     * Make a form element keyboard aware. Once added with this function, the
     * element will automatically respond to cursor key navigation.
     * 
     * @param String $id The HTML id of the form element for which keyboard 
     *                   navigation is added.
     * @param int $navkeys A bitwise mask indicating which keys should be 
     *                     supported for this element. Some elements, like
     *                     for example textarea's, use some cursor movements
     *                     for their own navigation. In this case, pass a 
     *                     mask that uses different keys.
     */
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
    
    /**
     * Make a recordlist keyboard aware. Once added with this function, the 
     * recordlist will automatically respond to keyboard navigation events.
     *
     * @param String $id The unique id of the recordlist.
     * @param String $highlight The color used to highlight rows that are 
     *                          selected with cursorkeys.
     * @param String $reccount The number of records in the list. The
     *                         handler needs this to be able to determine
     *                         when it's at the end of the list, so it
     *                         can wrap around when the cursor is moved
     *                         beyond the end.
     */
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
    
    /**
     * Register a generic keyboard handler. This method is used internally
     * by other atkKeyboard members, but can also be used to add a custom
     * keyboard handler to the page.
     *
     * @param String $handlertype The name of the javascript class used for
     *                            keyboard traps.
     * @param array $params Any param you may want to pass to the handler.
     *                      The params you need to pass depend completely on
     *                      the handler used. See the handlers' documentation
     *                      on params needed.
     */ 
    function addHandler($handlertype, $params)
    {    
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/keyboard/javascript/keyboardhandler.js");
      $page->register_loadscript("kb_init();\n");
      $page->register_loadscript("kb_addListener(new $handlertype(".implode(",",$params)."));");
    }
  }
?>