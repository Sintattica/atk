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
   * Handler class for the search action of a node. The handler draws a
   * generic search form for the given node.
   *
   * The actual search is not performed by this handler. The search values are
   * stored in the default atksearch variables, which the admin page uses to
   * perform the actual search. The search form by default redirects to
   * the adminpage to display searchresults.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Sandy Pleyte <sandy@achievo.org>
   * @package atk
   * @subpackage handlers
   * @todo The admin action handler is called when a search is performed. This
   *       should be customizable in the future.
   *
   */
   atkimport("atk.handlers.atkabstractsearchhandler");
  class atkSearchHandler extends atkAbstractSearchHandler
  {

    /**
     * The action handler method.
     */
    function action_search()
    {
      if (!empty($this->m_partial))
      {
        $this->partial($this->m_partial);
        return;
      }

      // save criteria
      $criteria = $this->fetchCriteria();
      $name = $this->handleSavedCriteria($criteria);

      // redirect to search results and return
      $doSearch = isset($this->m_postvars['atkdosearch']);
      if ($doSearch)
      {
        $this->redirectToResults();
        return;
      }
      elseif (!empty($this->m_postvars['atkcancel']))
      {
        $this->m_node->redirect(session_url(dispatch_url($this->m_node->atkNodeType(), 'admin'), SESSION_BACK));
      }

      $page = &$this->getPage();
      $searcharray = array();

      // load criteria
      if(isset($this->m_postvars['load_criteria']))
      {
        if (!empty($name))
        {
          $criteria = $this->loadCriteria($name);
          $searcharray = $criteria['atksearch'];
        }
      }
      elseif (isset($this->m_postvars["atksearch"]))
      {
        $searcharray = $this->m_postvars["atksearch"];
      }
      $page->addcontent($this->m_node->renderActionPage("search", $this->invoke("searchPage", $searcharray)));
    }

    /**
     * Redirect to search results based on the given criteria.
     */
    function redirectToResults()
    {
      $url = dispatch_url($this->m_node->atkNodeType(), 'admin', $this->fetchCriteria(), atkSelf());
      $this->m_node->redirect(session_url($url, atkLevel() == 0 ? SESSION_REPLACE : SESSION_BACK));
    }

    /**
     * Attribute handler.
     *
     * @param string $partial full partial
     */
    function partial_attribute($partial)
    {
      list($type, $attribute, $partial) = explode('.', $partial);

      $attr = &$this->m_node->getAttribute($attribute);
      if ($attr == NULL)
      {
        atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeType()."'");
        return '';
      }

      return $attr->partial($partial, 'add');
    }

    /**
     * This method returns an html page that can be used as a search form.
     * @param array $record A record containing default values that will be
     *                      entered in the searchform.
     * @return String The html search page.
     */
    function searchPage($record=NULL)
    {
      $node = &$this->m_node;

      $node->addStyle("style.css");
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_loadscript("placeFocus();");
      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $params = array();
        $params["formstart"] = '<form name="entryform" action="'.$controller->getPhpFile().'?'.SID.'" method="post">';

        $params["formstart"].= session_form(SESSION_REPLACE);
        $params["formstart"].='<input type="hidden" name="atkaction" value="search">';

        $params["formstart"].='<input type="hidden" name="atknodetype" value="'.$node->atknodetype().'">';
        $params["formstart"].='<input type="hidden" name="atkstartat" value="0">'; // start at first page after new search

        $params["content"] = $this->invoke("searchForm", $record);

        $params["buttons"][] = '<input type="submit" class="btn_search atkdefaultbutton" value="'.atktext("search", "atk").'" name="atkdosearch" >';
        $params["buttons"][] = '<input class="btn_cancel" type="submit" value="'.atktext("cancel", "atk").'" name="atkcancel"/>';


        $params["formend"] = '</form>';

        $output = $ui->renderAction("search", $params);

        $total = $ui->renderBox(array("title"=>$node->actionTitle('search'),
                                      "content"=>$output));

        return $total;
      }
      else
      {
        atkerror("ui object failure");
      }
    }

    /**
     * This method returns a form that the user can use to search records.
     *
     * @param array $record A record containing default values to put into
     *                      the search fields.
     * @return String The searchform in html form.
     */
    function searchForm($record = NULL)
    {
      $node = &$this->m_node;
      $ui = &$this->getUi();

      if (is_object($ui))
      {
        $node->setAttribSizes();

        $criteria = $this->fetchCriteria();
        $name = $this->handleSavedCriteria($criteria);

        $params = array();
        $params['searchmode_title'] = atktext("search_mode", "atk");
        $params['searchmode_and']   = '<input type="radio" name="atksearchmethod" class="atkradio" value="AND" checked>'.atktext("search_and", "atk");
        $params['searchmode_or']    = '<input type="radio" name="atksearchmethod" class="atkradio" value="OR">'.atktext("search_or", "atk");
        $params['saved_criteria']   = $this->getSavedCriteria($name);

        $params["fields"] = array();

        foreach (array_keys($node->m_attribList) as $attribname)
        {
          $p_attrib = &$node->m_attribList[$attribname];

          if (!$p_attrib->hasFlag(AF_HIDE_SEARCH))
            $p_attrib->addToSearchformFields($params["fields"], $node, $record, "", $this->m_postvars['atksearchmode']);
        }
        return $ui->render($node->getTemplate("search", $record), $params);
      }
      else atkerror("ui object error");
    }

    /**
     * Fetch posted criteria.
     *
     * @return Array fetched criteria
     */
    function fetchCriteria()
    {
      return array('atksearchmethod' => $this->m_postvars['atksearchmethod'],
                   'atksearch'       => $this->m_postvars['atksearch'],
                   'atksearchmode'   => $this->m_postvars['atksearchmode']);
    }

  }
?>