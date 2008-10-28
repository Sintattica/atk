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
   * multiple records.
   *
   * @author Lineke Kerckhoffs-Willems <lineke@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  atkimport("atk.handlers.atkadminhandler");
  
  class atkMultiSelectHandler extends atkAdminHandler
  {
    /**
     * The action handler method.
     */
    function action_multiselect()
    {
      if (!empty($this->m_partial))
      {
        $this->partial($this->m_partial);
        return;
      }
      
      if (isset($this->m_postvars['atkselector']))
      {
        $output = $this->invoke("handleMultiselect");
      }
      else $output = $this->invoke("multiSelectPage");

      if ($output != "")
      {
        $page =  &$this->getPage();
        $page->addContent($this->m_node->renderActionPage("multiselect", $output));
      }
    }

    /**
     * Parse atkselectors in postvars into atktarget using atktargetvartpl and atktargetvar
     * Then redirect to atktarget
     *
     */
    function handleMultiselect()
    {
      $node = &$this->getNode();
      $columnConfig = &$node->getColumnConfig();
      $recordset = $node->selectDb(implode(' OR ',$this->m_postvars['atkselector']),
                                   $columnConfig->getOrderByStatement(),
                                   "",
                                   $node->m_listExcludes,
                                   "",
                                   "multiselect");

      // loop recordset to parse atktargetvar
      $atktarget    = atkurldecode($node->m_postvars['atktarget']);
      $atktargetvar = $node->m_postvars['atktargetvar'];
      $atktargettpl = $node->m_postvars['atktargetvartpl'];

      for ($i = 0; $i < count($recordset); $i++)
      {
      	if ($i == 0 && strpos($atktarget, '&') === false) $atktarget.= '?';
      	else $atktarget .= '&';
      	$atktarget.= $atktargetvar.'[]='.$this->parseString($atktargettpl, $recordset[$i]);
      }
      $node->redirect($atktarget);
    }

    function parseString($string, $recordset)
    {
      atkimport("atk.utils.atkstringparser");
      $parser = new atkStringParser($string);

      // for backwardscompatibility reasons, we also support the '[pk]' var.
      $recordset['pk'] = $this->getNode()->primaryKey($recordset);
      $output = $parser->parse($recordset,true);
      return $output;
    }

    /**
     * This method returns an html page containing a recordlist to select
     * records from. The recordlist can be searched, sorted etc. like an
     * admin screen.
     *
     * @return String The html select page.
     */
    function multiSelectPage()
    {
      // add the postvars to the form
      global $g_stickyurl;
      $g_stickyurl[] = 'atktarget';
      $g_stickyurl[] = 'atktargetvar';
      $g_stickyurl[] = 'atktargetvartpl';
      $GLOBALS['atktarget']       = $this->getNode()->m_postvars['atktarget'];
      $GLOBALS['atktargetvar']    = $this->getNode()->m_postvars['atktargetvar'];
      $GLOBALS['atktargetvartpl'] = $this->getNode()->m_postvars['atktargetvartpl'];
      
      $this->getNode()->addStyle("style.css");

      $params["header"] = atktext("title_multiselect", $this->getNode()->m_module, $this->getNode()->m_type);

      $actions['actions'] = array();
      $actions['mra'][] = 'multiselect';

      atkimport('atk.datagrid.atkdatagrid');
      $grid = atkDataGrid::create($this->getNode(), 'multiselect');
      $grid->setDefaultActions($actions);
      $grid->removeFlag(atkDataGrid::EXTENDED_SEARCH);
      $grid->addFlag(atkDataGrid::MULTI_RECORD_ACTIONS);
      $params["list"] = $grid->render();

      if(atkLevel() > 0)
      {
        $backlinkurl = atkSessionManager::sessionUrl(atkSelf().'?atklevel='.session_level(SESSION_BACK));
        $params["footer"] = '<br><div style="text-align: center"><input type="button" onclick="window.location=\''.$backlinkurl.'\';" value="&lt;&lt; '.atktext('back').'"></div>';
      }

      $output = $this->getUi()->renderList("multiselect", $params);

      return $this->getUi()->renderBox(array("title"=>$this->getNode()->actionTitle('multiselect'),
                                             "content"=>$output));
    }
  }
?>