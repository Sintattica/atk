<?php
  /**
   * This file is part of the Ibuildings E-business Platform.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @author Dennis Luitwieler <dennis@ibuildings.nl>
   *
   * @copyright (c) 2007 Ibuildings.nl BV
   * @license see doc/LICENSE
   *
   * @version $Revision$
   * $Id$
   */

  define("ATTRIBUTEEDIT_ERROR_DEFAULT", 1);
  define("ATTRIBUTEEDIT_ERROR_UPDATE", 2);
  define("ATTRIBUTEEDIT_ERROR_NO_SELECTOR_SET", 4);
  define("ATTRIBUTEEDIT_ERROR_VALIDATE", 8);

  /**
   * Handler for the 'attributeedit' action of a node. It shows a dialog for altering the value of
   * a selectable attribute for multiple records at the same time.
   *
   * This handler only supports dialogs.
   *
   * @author Dennis Luitwieler <dennis@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkAttributeEditHandler extends atkActionHandler
  {
    var $m_processUrl = null;

    /**
     * The action method.
     */
    function action_attributeedit()
    {
      $this->setRenderMode('dialog');

      if ($this->m_partial == 'process')
      {
        $this->handleProcess();
      }
      elseif ($this->m_partial == 'refreshvaluefield')
      {
        $this->handleRefreshValuesField();
      }
      else
      {
        $atkselector = $this->getSelector();
        if (!isset($atkselector) || $atkselector=="")
        {
          $this->handleError(ATTRIBUTEEDIT_ERROR_NO_SELECTOR_SET, null, false);
        }
        else
        {
          $this->handleDialog();
        }
      }
    }

    /**
     * Get the atkselector from the postvars.
     *
     * The atkselector could be prefixed, depending on the page where we come
     * from. From the admin page it will be prefixed with the formname, from the
     * attributeedit dialog, it will not be prefixed anymore.
     *
     * @return Array
     */
    function getSelector()
    {
      // Is the selector in the atkselector postvar? ...
      if (isset($this->m_node->m_postvars['atkselector']))
        return $this->m_node->m_postvars['atkselector'];

      // ...if not, it should be in the prefixed atkselector postvar. If not, we return null.
      return $this->m_node->m_postvars[$this->m_node->m_postvars['atkformname'].'_'.'atkselector'];
    }

    /**
     * Handle dialog partial.
     */
    function handleDialog()
    {
      $page = &$this->getPage();
      $result = $this->invoke('attributeEditPage');
      $page->addContent($result);
    }

    /**
     * Handle process partial.
     */
    function handleProcess()
    {
      $node = &$this->m_node;
      $page = &$this->getPage();

      $atkselector = $this->getSelector();

      if (isset($atkselector))
      {
        // looping and changing the atkselectors by reference caused some problems
        // so we just use a for loop.
        for ($i=0,$_i=count($atkselector);$i<$_i;$i++)
        {
          /**
           * @todo: somehow the atkselector is encoded twice, we need
           * to decode it twice.
           */
          $atkselector[$i] = urldecode(urldecode($atkselector[$i]));
        }
      }

      $attributename  = $node->m_postvars["attributename"];
      $attributevalue = $node->m_postvars[$attributename];
      $attribute = $node->getAttribute($attributename);

      /** check if input is correct ... */
      if (!isset($atkselector) || count($atkselector)==0 || !is_object($attribute) || empty($attributename))
      {
        $this->handleError(ATTRIBUTEEDIT_ERROR_DEFAULT);
        return;
      }

      // All updates and validates will be successfully executed, until proven otherwise
      $validate = true;
      $success = true;
      foreach ($atkselector as $selector)
      {
        list($rec) = $node->selectDb($selector, "", "", "", array($attributename));

        $rec[$attributename] = $attribute->fetchValue($node->m_postvars);

        // Get all the attributes we are NOT changing.
        $ignoreList = $this->getIgnoreList($attributename);

        // Try to validate the new record.
        if (!$node->validate($rec, 'edit', $ignoreList))
        {
          $validate = false;
          break;
        }

        // If the validation succeeded, we will get here and try to perform the update.
        if (!$node->updateDb($rec, false, "", array($attributename)))
        {
          $success = false;
          triggerError($rec, $attributename, $node->getDb()->getErrorType(), $node->getDb()->getErrorMsg());
          break;
        }
      }

      // on succes, commit the changes and refresh the page where this dialog was initiated.
      if ($validate && $success)
      {
        $node->getDb()->commit();
        $content = "<script type=\"text/javascript\">document.location.href = document.location.href;</script>";
        $page->addContent($content);
        return;
      }

      // On validation error, show a validation error message.
      if (!$validate)
      {
        $this->handleError(ATTRIBUTEEDIT_ERROR_VALIDATE, $rec, true);
        return;
      }

      // On failure, do a rollback (if the db supports it) and show an error page.
      $node->getDb()->rollback();
      $this->handleError(ATTRIBUTEEDIT_ERROR_UPDATE, $rec, true);

    }

    /**
     * Handle errors of different types.
     *
     * @param int $error The AttributeEditHandler error type.
     */
    function handleError($error, $record=null, $reload=false)
    {
      atkimport('atk.ui.atkdialog');

      $this->registerExternalFiles();

      $content = $this->getErrorPage($error, $record);

      $page = &$this->getPage();

      if (!$reload)
      {
        $page->addContent($content);
      }
      else
      {
        $script = atkDialog::getUpdateCall($content, false);
        $page->register_loadscript($script);
      }
    }

    /**
     * Get a page indicating an error has occurred.
     *
     * @param Int $error
     * @return String HTML Error page
     */
    function getErrorPage($error, $record=null)
    {
      $errortext = '';
      if ($error == ATTRIBUTEEDIT_ERROR_UPDATE)
      {
        $errortext = $this->m_node->text('error_attributeedit_update');
      }
      elseif ($error == ATTRIBUTEEDIT_ERROR_NO_SELECTOR_SET)
      {
        $errortext = $this->m_node->text("error_attributeedit_noselectorset");
      }
      elseif ($error == ATTRIBUTEEDIT_ERROR_VALIDATE)
      {
        $errortext = $this->m_node->text("error_attributeedit_validationfailed");
      }
      else  // Other errors
      {
        $errortext = $this->m_node->text('error_attributeedit_default');
      }

      $errormsg = '';
      if (isset($record) && isset($record['atkerror']))
      {
        $errormsg = $record['atkerror']['attrib_name'].': '.$record['atkerror']['msg'].'&nbsp;';
      }

      $ui = &$this->m_node->getUi();
      atkimport('atk.ui.atkdialog');
      $params = array();
      $params["content"] = "<b>".$errortext."</b><br />";
      $params["content"].= $errormsg;
      $params["buttons"][] = '<input type="button" class="btn_cancel" value="'.$this->m_node->text('close').'" onClick="'.atkDialog::getCloseCall().'" />';
      $content = $ui->renderAction("attributeedit", $params);

      $params = array();
      $params["title"] = $this->m_node->actionTitle('attributeedit');
      $params["content"] = $content;
      $content = $ui->renderDialog($params);
      return $content;
    }


    /**
     * AttributeEdit page.
     */
    function attributeEditPage()
    {
      return $this->getAttributeEditPage();
    }

    /**
     * Returns the attributeEdit page contents.
     *
     * @return string attributeEdit page contents
     */
    function getAttributeEditPage()
    {
      $url = $this->getProcessUrl();
      $controller = &atkController::getInstance();

      $this->registerExternalFiles();

      $params = array();
      $params["formstart"] = $this->getFormStart();
      $params["formend"] = '</form>';
      $params["content"] = $this->getContent();
      $params["buttons"][] = $controller->getDialogButton('save', $this->m_node->text('update_value'), $url);
      $params["buttons"][] = $controller->getDialogButton('cancel');

      return $this->renderAttributeEditPage($params);
    }

    function registerExternalFiles()
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkattributeedithandler.js');
      $page->register_style($ui->stylePath("style.css"));
    }

    /**
     * Render the add or copy page using the given parameters.
     *
     * @param array $params parameters
     * @return string rendered page
     */
    function renderAttributeEditPage($params)
    {
      $node = &$this->m_node;
      $ui = &$node->getUi();

      $output = $ui->renderAction("add", $params);
      $this->addRenderBoxVar("title", $node->actionTitle('addorcopy'));
      $this->addRenderBoxVar("content", $output);
      $total = $ui->renderDialog($this->m_renderBoxVars);

      return $total;
    }

    /**
     * Returns the attributeEdit page contents.
     *
     * @return string attributeEdit page contents
     */
    function getContent()
    {
      $content = '
        <div id="dialogcontent">' .
          $this->_getDropDownAttributes() .
          '<br />
           <div id="selectedvaluediv">' .
              $this->_getDropDownValues() .
           '</div>
        </div>';

      return $content;
    }

    function _getDropDownAttributes($selectedAttribute="")
    {
      $fieldprefix = $this->m_node->m_postvars['atkfieldprefix'];

      $attributes = $this->getSupportedAttributes();

      $attr = null;

      // Select the first attribute if none is selected
      if ($selectedAttribute=="")
      {
        // select the first (if available)
        $record['attributename'] = isset($attributes[0]) ? $attributes[0]->fieldName() : null;
      }
      else
      {
        $record['attributename'] = $selectedAttribute;
      }

      // Create options and values
      foreach ($attributes as $attribute)
      {
        $options[] = $this->m_node->text($attribute->label($record));
        $values[]  = $attribute->fieldName();
      }

      $list = &new atkListAttribute('attributename', $options, $values);
      $list->addFlag(AF_LIST_NO_NULL_ITEM);
      $list->m_ownerInstance = &$this->m_node;
      $list->addOnChangeHandler($this->onChange());
      return $list->edit($record,$fieldprefix);
    }

    function onChange()
    {
      $url = partial_url($this->m_node->atkNodeType(), 'attributeedit', 'refreshvaluefield');
      $script = "
        ATK.AttributeEditHandler.refreshvalues('$url');
      ";

      return $script;
    }

    /**
     * Get the dropdown with the possible values for
     * the selected attribute.
     *
     * If no
     *
     * @param unknown_type $selectedAttribute
     * @return unknown
     */
    function _getDropDownValues($selectedAttribute="")
    {
      $fieldprefix = $this->m_node->m_postvars['atkfieldprefix'];

      $attr = null;

      // Select the first attribute if none is selected
      if ($selectedAttribute=="")
      {
        // get first attribute
        list($attr) = $this->getSupportedAttributes();
      }
      else
      {
        $attr = &$this->m_node->getAttribute($selectedAttribute);
      }

      if (is_object($attr))
      {
        $record[$attr->fieldName()] = $this->m_node->m_postvars[$attr->fieldName()];
        return $attr->edit($record, $fieldprefix, 'edit');
      }

      return "";
    }

    /**
     * Get the supported attributes.
     *
     * If no supported attributes were set, ATK determines them by itself.
     *
     * @return array Array with attribute objects
     */
    function getSupportedAttributes()
    {
      $supported = array();
      if (method_exists($this->m_node, 'getAttributeEditSupportedAttributes'))
      {
        $supported_attributes = $this->m_node->getAttributeEditSupportedAttributes();

        if (isset($supported_attributes) && is_array($supported_attributes))
        {
          foreach ($supported_attributes as $attribname)
          {
            $attr = $this->m_node->getAttribute($attribname);
            if (is_object($attr))
              $supported[] = $attr;
          }
        }

        return $supported;
      }

      // We let ATK determine the available attributes.
      $attributes = $this->m_node->getAttributes();

      // We do not need certain attributes.
      foreach ($attributes as $index=>$attr)
      {        
        // Attributes without labels will not be selectable.
        if ($attr->hasFlag(AF_NO_LABEL) ||  $attr->hasFlag(AF_BLANK_LABEL))
          continue;

        // Hidden attributes will not be selectable
        if ($attr->hasFlag(AF_HIDE) || $attr->hasFlag(AF_HIDE_EDIT))
          continue;

        // You cannot give multiple records a value for a field that should be unique.
        $atkselector = $this->getSelector();
        if (isset($atkselector) && count($atkselector)>1 && $attr->hasFlag(AF_UNIQUE))
          continue;

        // You cannot update readonly fields
        if ($attr->hasFlag(AF_READONLY) || $attr->hasFlag(AF_READONLY_EDIT))
          continue;

        // We do not support manytomany relations (for now...).
        if (is_a($attr, 'atkmanytomanyrelation'))
          continue;

        $supported[] = $attr;
      }

      return $supported;
    }

    /**
     * Get a list of attributenames that we can ignore (i.e. when calling
     * validate() on a node.)
     *
     * @param String $selectedattributename
     * @return Array A list of attributenames.
     */
    function getIgnoreList($selectedattributename)
    {
      $attributes = $this->m_node->getAttributes();

      $attribnames = array();
      foreach ($attributes as $attr)
      {
        $attribnames[] = $attr->fieldName();
      }

      // remove the selected attribute from all the available attributes,
      // and we have a list of attributes that we can ignore.
      return array_diff($attribnames, array($selectedattributename));
    }


    /**
     * Returns the form start.
     *
     * @return string Html form start
     */
    function getFormStart()
    {
      $controller = &atkcontroller::getInstance();
      $controller->setNode($this->m_node);

      $formstart = '<form id="dialogform" name="dialogform" action="'.$controller->getPhpFile().'?'.SID.'" method="post">';
      $atkselector = $this->getSelector();

      if (isset($atkselector))
      {
        foreach ($atkselector as &$selector)
        {
          /**
           * @todo: For some reason the atkselector has been urlencoded twice.....
           * so we need to decode it twice.
           */
          $selector = urldecode(urldecode($selector));
          $formstart.= '<input type="hidden" id="atkselector[]" name="atkselector[]" value="'.$selector.'">';
        }
      }

      return $formstart;
    }


    function handleRefreshValuesField()
    {
      $page = &$this->getPage();

      atkimport('atk.utils.atkjson');
      // get selected attribute
      $selectedattribute = $this->m_node->m_postvars["attributename"];

      $field = $this->_getDropDownValues($selectedattribute);

      $content = "<script type=\"text/javascript\">$('selectedvaluediv').innerHTML = ".atkJSON::encode($field)."</script>";

      $page->addContent($content);
    }

    /**
     * Returns the process URL.
     *
     * @return string process URL
     */
    function getProcessUrl()
    {
      if ($this->m_processUrl != null)
      {
        return $this->m_processUrl;
      }
      else
      {
        return partial_url($this->m_node->atkNodeType(), 'attributeedit', 'process');
      }
    }

    /**
     * Override the default process URL.
     *
     * @param string $url process URL
     */
    function setProcessUrl($url)
    {
      $this->m_processUrl = $url;
    }
  }
?>