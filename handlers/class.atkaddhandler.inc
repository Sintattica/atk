<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler for the 'add' action of a node. It draws a page where the user
   * can enter a new record.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkAddHandler extends atkActionHandler 
  {
    /**
     * The action handler.
     */
    function action_add()
    {
      atkdebug("Action add");
      $page = &$this->getPage();
      $page->addContent($this->m_node->renderActionPage("add", $this->invoke("addPage")));
    }    
    
    /**
     * Creates an add page.
     * 
     * @param array $record The record which contains default values for the
     *                      add-form.
     * @return String A String containing a box with an add form.
     */
    function addPage($record=NULL)
    {            
      $node = &$this->m_node;
      $page = &$this->getPage();
            
      // Utility.
      $edithandler = &$node->getHandler("edit");
      
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");            
      $node->addStyle("style.css");
      
      $params = $node->getDefaultActionParams();

      $formstart ='<form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';

      /* if we are on an admin page we would like to stay on the admin page after adding a new record */
      if ($node->m_action == "admin") $formstart.=session_form(SESSION_NESTED);
      else $formstart.=session_form();
      
      $params["formstart"] = $formstart;
      
      $ui = &$node->getUi();
      if (is_object($ui))
      {        
        $forceList = array();
        if (isset($node->m_postvars['atkfilter']))
        {
          $forceList = decodeKeyValueSet($node->m_postvars['atkfilter']);
        }
        $form = $edithandler->editForm("add",$record,$forceList);                      
        
        $params["content"] = $node->tabulate("add", $form);                
                    
        $params["buttons"][] = '<input type="submit" class="btn_save" value="'.text('save').'">';
        if ($node->m_action=="add")
        {
          // if action is admin, we don't show the cancelbutton..
          $params["buttons"][] ='<input type="submit" class="btn_cancel" name="atkcancel" value="'.text('cancel').'">';
        }
        $params["formend"] = '</form><SCRIPT type="text/javascript" language="JavaScript">placeFocus()</SCRIPT>';
        
        $output = $ui->renderAction("add", $params);
              
        $total = $ui->renderBox(array("title"=>$node->actionTitle('add'),
                                      "content"=>$output));
        
        return $total;               
      }      
      else
      {
        atkerror("ui object failure");
      }
    }
  }

?>
