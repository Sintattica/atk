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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Handler for the 'admin' action of a node. It displays a recordlist with
   * existing records, and links to view/edit/delete them (or custom actions
   * if present), and an embedded addform or a link to an addpage (depending
   * on the presence of the NF_ADD_LINK or NF_ADD_DIALOG flag).
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
  atkimport("atk.handlers.atkactionhandler");

  class atkAdminHandler extends atkActionHandler
  {
    var $m_actionSessionStatus = SESSION_NESTED;

    /**
     * The action method
     */
    function action_admin()
    {
      if (!empty($this->m_partial))
      {
        $this->partial($this->m_partial);
        return;
      }

      $page = &$this->getPage();
      $res = $this->renderAdminPage();
      $page->addContent($this->m_node->renderActionPage("admin", $res));
    }

    /**
     * Sets the action session status for actions in the recordlist.
     * (Defaults to SESSION_NESTED).
     */
    function setActionSessionStatus($sessionStatus)
    {
      $this->m_actionSessionStatus = $sessionStatus;
    }

    /**
     * Render the adminpage, including addpage if necessary
     *
     * @param array $record
     * @return array with result of adminPage and addPage
     */
    function renderAdminPage()
    {
      $res = array();
      if ($this->m_node->hasFlag(NF_NO_ADD)==false&&$this->m_node->allowed("add"))
      {
        if (!$this->m_node->hasFlag(NF_ADD_LINK) && !$this->m_node->hasFlag(NF_ADD_DIALOG)) // otherwise, in adminPage, an add link will be added.
        {
          // we could get here because of a reject.
          $record = $this->getRejectInfo();

          $res[]=$this->invoke("addPage", $record);
        }
      }
      $res[] = $this->invoke("adminPage");
      return $res;
    }

    /**
     * Draws the form for adding new records.
     *
     * The implementation delegates drawing of the form to the atkAddHandler.
     *
     * @return String A box containing the add page.
     */
    function addPage($record=NULL)
    {
      // Reuse the atkAddHandler for the addPage.
      $handler = $this->m_node->getHandler("add");
      $handler->setNode($this->m_node);
      $handler->setReturnBehaviour(ATK_ACTION_STAY); // have the save action stay on the admin page
      return $handler->invoke("addPage", $record);
    }

    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     *
     * @param array $actions The list of actions displayed next to each
     *                       record. Nodes can implement a
     *                       recordActions($record, &$actions, &$mraactions)
     *                       method to add/remove record-specific actions.
     * @return String A box containing the admin page (without the add form,
     *                which is added later.
     */
    function adminPage($actions="")
    {
      $ui = &$this->getUi();

      $vars = array("title"=>$this->m_node->actionTitle($this->getNode()->m_action),
                    "content"=>$this->renderAdminList());

      if ($this->getRenderMode() == 'dialog')
      {
        $output = $ui->renderDialog($vars);
      }
      else
      {
        $output = $ui->renderBox($vars);
      }

      return $output;
    }

    /**
     * Renders the recordlist for the admin mode
     *
     * @param Array $actions An array with the actions for the admin mode
     * @return String The HTML for the admin recordlist
     */
    function renderAdminList($actions="")
    {
      $this->getNode()->addStyle("style.css");

      atkimport('atk.datagrid.atkdatagrid');
      $grid = atkDataGrid::create($this->getNode(), 'admin');
      
      if (is_array($actions))
      {
        $grid->setDefaultActions($actions);
      }
      
      $this->modifyDataGrid($grid, atkDataGrid::CREATE);

      if ($this->redirectToSearchAction($grid))
      {
        return '';
      }

      $params = array();
      $params["header"] = $this->invoke("adminHeader").$this->getHeaderLinks();
      $params["list"] = $grid->render();
      $params["footer"] = $this->invoke("adminFooter");
      $output = $this->getUi()->renderList("admin", $params);
      return $output;
    }

    /**
     * Update the admin datagrid.
     *
     * @return string new grid html
     */
    public function partial_datagrid()
    {
      atkimport('atk.datagrid.atkdatagrid');
      $grid = atkDataGrid::resume($this->getNode());
      
      $this->modifyDataGrid($grid, atkDataGrid::RESUME);      

      if ($this->redirectToSearchAction($grid))
      {
        return '';
      }

      return $grid->render();
    }
     
    /**
     * Modify grid.
     * 
     * @param atkDataGrid $grid grid
     * @param int         $mode CREATE or RESUME
     */
    protected function modifyDataGrid(atkDataGrid $grid, $mode)
    {    
      $method = 'modifyDataGrid';
      if (method_exists($this->getNode(), $method))
      {
        $this->getNode()->$method($grid, $mode);
      }
    }

    /**
     * If a search action has been defined and a search only returns one result
     * the user will be automatically redirected to the search action.
     *
     * @param atkDataGrid $grid data grid
     * @return boolean redirect active?
     */
    protected function redirectToSearchAction($grid)
    {
      $node = $this->getNode();
      $search = $grid->getPostvar('atksearch');

      // check if we are searching and a search action has been defined
      if (!is_array($search) || count($search) == 0 || !is_array($node->m_search_action) || count($node->m_search_action) == 0)
      {
        return false;
      }

      // check if there is only a single record in the result
      $grid->loadRecords();
      if ($grid->getCount() != 1)
      {
        return false;
      }

      $records = $grid->getRecords();

      foreach ($node->m_search_action as $action)
      {
        if (!$node->allowed($action, $records[0]))
        {
          continue;
        }

        // reset search so we can back to the normal admin screen if we want
        $grid->setPostvar('atksearch', array());

        $url = session_url(dispatch_url($node->atkNodeType(), $action, array('atkselector' => $node->primaryKey($records[0]))), SESSION_NESTED);

        if ($grid->isUpdate())
        {
          atkimport('atk.utils.atkjson');
          $script = 'document.location.href = '.atkJSON::encode($url).';';
          $node->getPage()->register_loadscript($script);
        }
        else
        {
          $node->redirect($url);
        }

        return true;
      }

      return false;
    }

    /**
     * Function that is called when creating an adminPage.
     *
     * The default implementation returns an empty string, but developers can
     * override this function in their custom handlers or directly in the
     * node class.
     *
     * @return String A string that is displayed above the recordlist.
     */
    function adminHeader()
    {
      return "";
    }

   /**
     * Function that is called when creating an adminPage.
     *
     * The default implementation returns an empty string, but developers can
     * override this function in their custom handlers or directly in the
     * node class.
     *
     * @return String A string that is displayed below the recordlist.
     */
    function adminFooter()
    {
      return "";
    }

    function getImportLink()
    {
      $link = "";
      if ($this->m_node->allowed("add")&&!$this->m_node->hasFlag(NF_READONLY) && $this->m_node->hasFlag(NF_IMPORT))
      {
        $link.= href(dispatch_url($this->m_node->atkNodeType(), "import"), atktext("import", "atk", $this->m_node->m_type), SESSION_NESTED);
      }
      return $link;
    }

    function getExportLink()
    {
      $link = "";
      if ($this->m_node->allowed("view") && $this->m_node->allowed("export") && $this->m_node->hasFlag(NF_EXPORT))
      {
        $filter = '';    
        if(count($this->m_node->m_fuzzyFilters) > 0)
        {
          $filter = implode(' AND ', $this->m_node->m_fuzzyFilters); 
        }

        $link.= href(dispatch_url($this->m_node->atkNodeType(), "export", array('atkfilter'=>$filter)), atktext("export", "atk", $this->m_node->m_type), SESSION_NESTED);
      }
      return $link;
    }

    function getAddLink()
    {
			if (!$this->m_node->hasFlag(NF_NO_ADD) && $this->m_node->allowed("add"))
			{
				$label = atktext("link_".$this->m_node->m_type."_add", $this->m_node->m_module, "", "", "", true);
        if (empty($label))
        {
          // generic text
          $label = atktext(getNodeType($this->m_node->m_type),$this->m_node->m_module,$this->m_node->m_type)." ".atktext("add","atk","");
        }

        atkimport('atk.handlers.atkaddorcopyhandler');
        $add = $this->m_node->hasFlag(NF_ADD_DIALOG);
        $addorcopy = $this->m_node->hasFlag(NF_ADDORCOPY_DIALOG) &&
                     atkAddOrCopyHandler::hasCopyableRecords($this->m_node);


        if ($add || $addorcopy)
				{
				  $action = $this->m_node->hasFlag(NF_ADDORCOPY_DIALOG) ? 'addorcopy' : 'add';

					atkimport('atk.ui.atkdialog');
					$dialog = new atkDialog($this->m_node->atkNodeType(), $action, 'dialog');
					$dialog->setModifierObject($this->m_node);
					$dialog->setSessionStatus(SESSION_PARTIAL);
					$onClick = $dialog->getCall();

					return '
			      <a href="javascript:void(0)" onclick="'.$onClick.'; return false;" class="valignMiddle">'.$label.'</a>
			    ';
				}
				elseif ($this->m_node->hasFlag(NF_ADD_LINK))
				{
					$addurl = atkSelf().'?atknodetype='.$this->m_node->atknodetype().'&atkaction=add';
	        return atkHref($addurl,$label,SESSION_NESTED);
				}

			}

			return "";
    }

    function getHeaderLinks()
    {
      $links = array();
      $addlink = $this->getAddLink();
      if ($addlink!="") $links[] = $addlink;
      $importlink = $this->getImportLink();
      if ($importlink!="") $links[] = $importlink;
      $exportlink = $this->getExportLink();
      if ($exportlink!="") $links[] = $exportlink;
      return implode(" | ", $links);
    }

    /**
     * Dialog handler.
     */
    function partial_dialog()
    {
      $this->setRenderMode('dialog');
      $result = $this->renderAdminPage();
      return $this->m_node->renderActionPage("admin", $result);
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

      return $attr->partial($partial, 'admin');
    }
  }
?>
