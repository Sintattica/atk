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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Handler class for the select action of a node. The handler draws a
   * generic select form for searching through the records and selecting
   * one of the records.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkSelectHandler extends atkActionHandler
  {
    /**
     * The action handler method.
     */
    function action_select()
    {      
      $output = $this->invoke("selectPage");
      if ($output!="")
      {
        $page =  &$this->getPage();
        $page->addContent($this->m_node->renderActionPage("select", $output));
      }
    }
    
    /**
     * This method returns an html page containing a recordlist to select
     * records from. The recordlist can be searched, sorted etc. like an
     * admin screen.
     * 
     * @return String The html select page.
     */
    function selectPage()
    {
     atkimport("atk.utils.atkstringparser");
      $node = &$this->m_node;
      // When there's a lot of data, records will be spread across multiple pages.
      if ($node->m_postvars['atklimit']=="") $node->m_postvars['atklimit']=atkconfig("recordsperpage");
      if ($node->m_postvars['atkstartat']=="") $node->m_postvars['atkstartat']=0;

      $filter = $node->validateFilter($node->m_postvars["atkfilter"]);          

      $recordset = $node->selectDb($filter,
                                   $node->m_postvars['atkorderby'],
                                   array("offset" => $node->m_postvars['atkstartat'], "limit" => $node->m_postvars['atklimit']),
                                   $node->m_listExcludes,
                                   "",
                                   "select");

      if (count($recordset)==1 && $node->hasFlag(NF_AUTOSELECT))
      {
        // There's only one record and the autoselect flag is set, so we
        // automatically go to the target.
        $parser = new atkStringParser(rawurldecode(atkurldecode($node->m_postvars['atktarget'])));
        $target = $parser->parse($recordset[0],true);
        $node->redirect(session_url($target, SESSION_NESTED));
        return "";
      }
      else
      {        
        $ui = &$this->getUi();
        
        $params["header"] = text("title_select", $node->m_type);

        $navigator = &atknew("atk.utils.atknavigator");
        if ($node->m_index != "") 
        {
          $params["index"] = $navigator->buildIndex($node, $recordset[0][$node->m_index]);
        }

        // create navigation bar        
        $params["navbar"] = $navigator->buildNavigation($node, "select");        

        $actions["select"]=atkurldecode($node->m_postvars['atktarget']);

        $recordlist = &atknew("atk.recordlist.atkrecordlist");
        $params["list"] = $recordlist->render($node, $recordset, $actions);
        
        $output = $ui->renderList("select", $params);
        
        return $ui->renderBox(array("title"=>$node->actionTitle('select'),
                                    "content"=>$output));         
      }

    }

  }

?>