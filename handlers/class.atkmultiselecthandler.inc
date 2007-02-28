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
   * @author Lineke Kerckhoffs-Willems <lineke@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkMultiSelectHandler extends atkActionHandler
  {
    /**
     * The action handler method.
     */
    function action_multiselect()
    {
      atk_var_dump($this->m_postvars);
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
      $node = &$this->m_node;
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
      $recordset['pk'] = $this->m_node->primaryKey($recordset);
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
      $node = &$this->m_node;
      $node->addStyle("style.css");
      // When there's a lot of data, records will be spread across multiple pages.
      if ($node->m_postvars['atklimit']=="") $node->m_postvars['atklimit']=atkconfig("recordsperpage");
      if ($node->m_postvars['atkstartat']=="") $node->m_postvars['atkstartat']=0;

      $filter = $node->validateFilter($node->m_postvars["atkfilter"]);

      $columnConfig = &$node->getColumnConfig();

      $recordset = $node->selectDb($filter,
                                   $columnConfig->getOrderByStatement(),
                                   array("offset" => $node->m_postvars['atkstartat'], "limit" => $node->m_postvars['atklimit']),
                                   $node->m_listExcludes,
                                   "",
                                   "multiselect");

      $ui = &$this->getUi();

      $params["header"] = atktext("title_multiselect", $node->m_module, $node->m_type);

      $navigator = &atknew("atk.utils.atknavigator");
      if ($node->m_index != "")
      {
        $params["index"] = $navigator->buildIndex($node, $recordset[0][$node->m_index]);
      }

      // create navigation bar
      $params["navbar"] = $navigator->buildNavigation($node, "multiselect");

      $actions['actions'] = array();
      $actions['mra'][] = 'multiselect';

      $recordlist = &atknew("atk.recordlist.atkrecordlist");
      $params["list"] = $recordlist->render($node, $recordset, $actions, RL_MRA);

      if(atkLevel() > 0)
      {
        $backlinkurl = session_url($_SERVER["PHP_SELF"].'?atklevel='.session_level(SESSION_BACK));
        $params["footer"] = '<br><div style="text-align: center"><input type="button" onclick="window.location=\''.$backlinkurl.'\';" value="&lt;&lt; '.atktext('back').'"></div>';
      }

      $output = $ui->renderList("multiselect", $params);

      return $ui->renderBox(array("title"=>$node->actionTitle('multiselect'),
                                  "content"=>$output));
    }
  }
?>