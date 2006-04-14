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
   * Smart search handler class.
   *
   * @author Peter C. Verhage <peter@achievo.org>
   *
   * @package atk
   * @subpackage handlers
   */
  class atkSmartSearchHandler extends atkActionHandler
  {
    var $m_table = 'atk_smartsearchcriteria';
    
    /**
     * The action handler method.
     */
    function action_smartsearch()
    {
      $page = &$this->getPage();
      
      // set attribute sizes
      $this->m_node->setAttribSizes();            

      // handle partials
      if (!empty($this->m_postvars['atkpartial']))
      {
        $method = 'partial'.$this->m_postvars['atkpartial'];
        if (method_exists($this, $method))
        {
          echo $this->$method();
          die;
        }
        else 
        {
          echo 'Unknown partial';
          die;
        }
      }      
      
      // fetch and process criteria
      $criteria = $this->fetchCriteria();      

      $name = $this->m_postvars['load_criteria'];
      if (!empty($this->m_postvars['forget_criteria']))
      {
        $forget = $this->m_postvars['forget_criteria'];
        $this->forgetCriteria($forget);
        $name = NULL;
      }
      else if (!empty($this->m_postvars['save_criteria']))
      {
        $save = $this->m_postvars['save_criteria'];
        $this->saveCriteria($save, $criteria);
        $name = $save;
      }
      
      // redirect to search results and return
      $doSearch = isset($this->m_postvars['atkdosearch']);
      if ($doSearch)
      {
        $this->redirectToResults($criteria);
        return;
      }
      
      // load criteria
      if (!empty($name))
      {
        $criteria = $this->loadCriteria($name);
      }
      else
      {
        $criteria = $this->loadBaseCriteria();
      }         
      
      // render smart search page
      $smartSearchPage = $this->invoke("smartSearchPage", $name, $criteria);
      $actionPage = $this->m_node->renderActionPage("smartsearch", $smartSearchPage);
      $page->addContent($actionPage);
    }
    
    /**
     * Partial criterium.
     */
    function partialCriterium()
    {
      $criterium = $this->getCriterium((int)$this->m_postvars['next_criterium_id']);
      return $this->renderCriterium($criterium);
    }
    
    /**
     * Partial criterium field.
     */
    function partialCriteriumField()
    {
      $criterium_id = $this->m_postvars['criterium_id'];
      $field_nr = $this->m_postvars['field_nr'];
      
      $attrNames = $this->m_postvars['criteria'][$criterium_id]['attrs'];
      ksort($attrNames);
      
      $attrNames = array_slice($attrNames, 0, $field_nr + 1);
      $path = $this->getNodeAndAttrPath($attrNames);
      $path = array_slice($path, $field_nr + 1); 

      if (count($path) > 0)
      {
        return $this->getCriteriumField($criterium_id, $path, $scriptCode).
               '<script language="javascript">'.implode("\n", $scriptCode).'</script>';
      }
      else 
      {
        return '';
      }
    }

    /**
     * Partial criterium value / mode.
     * 
     * @param $type 'value' or 'mode' partial?
     */
    function _partialCriteriumValueOrMode($type)
    {
      $criterium_id = $this->m_postvars['criterium_id'];
      $field_nr = $this->m_postvars['field_nr'];
      
      $attrNames = $this->m_postvars['criteria'][$criterium_id]['attrs'];
      ksort($attrNames);        
      
      $attrNames = array_slice($attrNames, 0, $field_nr + 1);
      $path = $this->getNodeAndAttrPath($attrNames);
      
      if (count($path) == $field_nr + 1)
      {
        if ($type == "mode")
        {
          return $this->getCriteriumMode($criterium_id, $path);
        }
        else 
        {
          return $this->getCriteriumValue($criterium_id, $path);          
        }
      }
      else 
      {
        return $this->m_node->text('none');
      }
    }
    
    /**
     * Partial criterium value.
     */
    function partialCriteriumValue()
    {    
      return $this->_partialCriteriumValueOrMode('value');
    }
    
    /**
     * Partial criterium value.
     */
    function partialCriteriumMode()
    {    
      return $this->_partialCriteriumValueOrMode('mode');      
    }    
    
    /**
     * Redirect to search results based on the given criteria.
     */
    function redirectToResults($criteria)
    {
      $params = array('atksmartsearch' => $criteria);
      $url = dispatch_url($this->m_node->atkNodeType(), 'admin', $params, $PHP_SELF);
      $this->m_node->redirect(session_url($url, atkLevel() == 0 ? SESSION_REPLACE : SESSION_BACK));
    }
    
    /**
     * Returns the base labels for use in the templates.
     * Contains labels for the following fields: 
     * 'field', 'value', 'add', 'remove'
     *
     * @return String label
     */
    function getLabels()
    {
      $labels = array('criterium_field', 'criterium_value', 'criterium_mode', 'add_criterium', 'remove_criterium', 'load_criteria', 'save_criteria', 'forget_criteria', 'reset_criteria');
      $result = array();
      foreach ($labels as $label)
        $result[$label] = atktext($label, 'atk');
      return $result;
    }
    
    /**
     * Returns the template path with the given name.
     * Name can be either 'form' or 'criterium'.
     *
     * @param String $name template name
     * @return full template path
     */
    function getTemplate($name)
    {
      $ui = &$this->getUi();
      if ($name == 'form') 
        return $this->m_node->getTemplate('smartsearch');
      else if ($name == 'criterium')
        return $ui->templatePath('smartcriterium.tpl');
    }
    
    /**
     * Get searchable attributes for the given node.
     * 
     * @param atkNode $node     reference to the node
     * @param Array   $excludes attribute exclude list
     * 
     * @return Array list of reference to searchable attributes
     */
    function getSearchableAttributes(&$node, $excludes)
    {
      $attrNames = array_keys($node->m_attribList);
      $attrNames = array_diff($attrNames, $excludes);
      sort($attrNames);
      
      $attrs = array();
      foreach ($attrNames as $attrName)
      {
        $attr = &$node->getAttribute($attrName);
        if (!$attr->hasFlag(AF_HIDE_SEARCH))
          $attrs[] = &$attr;
      }
      
      return $attrs;
    }    
    
    /**
     * Returns a select element with searchable attributes for
     * a certain node.
     * 
     * @param String  $name name of the select element
     * @param atkNode $node reference to the node
     */
    function getAttributeList($entry)
    {
      if (count($entry['attrs']) == 1)
      {
        $attr = &$entry['attrs'][0];
        $label = is_a($attr,'atkmanytoonerelation') ? '' : htmlentities(strip_tags($attr->label()));        
        return $label.'<input type="hidden" name="'.$entry['name'].'" value="'.$attr->fieldName().'">';
      }
      
      $result = 
        '<select id="'.$entry['name'].'" name="'.$entry['name'].'">'.
          '<option value=""></option>';
          
      if ($entry['includeSelf'])
      {
        $result .= 
          '<option value="."'.($entry['selectSelf'] ? ' selected="selected"' : '').'>'.$this->m_node->text('self').'</option>'.
          '<option value=""></option>';        
      }

      $current = &$entry['attr'];
      for ($i = 0, $_i = count($entry['attrs']); $i < $_i; $i++)
      {
        $attr = &$entry['attrs'][$i];
        $selected = $current != NULL && $attr->fieldName() == $current->fieldName() ? ' selected="selected"' : '';
        $label = htmlentities(strip_tags($attr->label()));
        $result .= '<option value="'.$attr->fieldName().'"'.$selected.'>'.$label.'</option>';
      }
      
      $result .= 
        '</select>';
        
      return $result;
    }    
    
    /**
     * Adds a node/attribute entry to the node/attribute path.
     * 
     * The entry consists of the the following fields:
     * - nr,          number in the node/attribute path (>= 0)
     * - node,        reference to the node for this path entry
     * - attr,        reference to the currently selected attribute for this path entry
     * - attrs,       all searchable attributes for this node
     * - includeSelf, whatever the attribute list should contain a reference to ourselves or not
     * - selectSelf,  should the self option be selected? (only valid if includeSelf is true)
     * 
     * This method will modify the $path, $includeSelf and $excludes parameters to prepare
     * them for the next call to this method.
     *
     * @param Array   $path        reference to the current path
     * @param atkNode $node        reference to the current node
     * @param String  $attrName    currently selected attribute
     * @param Boolean $includeSelf should we include ourselves?
     * @param Array   $excludes    attributes to exclude
     * 
     * @return atkNode next node
     */
    function &addNodeAndAttrEntry(&$path, &$node, $attrName, &$includeSelf, &$excludes)
    {
      $attr = &$node->getAttribute($attrName);

      $nr = count($path);
      $attrs = $this->getSearchableAttributes($node, $excludes);          
      
      if (count($attrs) == 1)
      {
        $attr = &$attrs[0];
      }
      
      $selectSelf = $includeSelf && $attrName == '.';
      
      $entry = array('nr' => $nr, 'node' => &$node, 'attrs' => $attrs, 'attr' => &$attr, 'includeSelf' => $includeSelf, 'selectSelf' => $selectSelf); 
      $path[] = &$entry;

      $includeSelf = is_a($attr, 'atkmanytoonerelation');
      $excludes = is_a($attr, 'atkonetomanyrelation') ? $attr->m_refKey : array();  
      
      if (is_a($attr, 'atkrelation'))
      {
        $attr->createDestination();
        return $attr->m_destInstance;
      }
      
      return NULL;    
    }

    /**
     * Returns the node/attribute path for the given
     * attribute name path.
     *
     * @param Array $attrPath attribute name path
     * @return Array node/attribute path
     */
    function getNodeAndAttrPath($attrPath)
    {
      $path = array();
      
      $node = &$this->m_node;
      $includeSelf = false;
      $excludes = array();
      
      $entry = NULL;
      
      foreach ($attrPath as $attrName)
      {
        $node = &$this->addNodeAndAttrEntry($path, $node, $attrName, $includeSelf, $excludes);
        if ($node == NULL) break;
      }

      while ($node != NULL)
      {
        $node = &$this->addNodeAndAttrEntry($path, $node, NULL, $includeSelf, $excludes);        
      }
      
      return $path;
    }    
    
    /**
     * Returns the criterium field for the given path.
     * 
     * @param Integer $id         criterium id
     * @param Array   $path       criterium path
     * @param Array   $scriptCode lines of JavaScript
     * 
     * @return String criterium field HTML
     */
    function getCriteriumField($id, $path, &$scriptCode)
    {
      $prefix = "criteria[{$id}][attrs]";      
            
      for ($i = 0, $_i = count($path); $i < $_i; $i++)
      {
        $entry = &$path[$i];
        $entry['name'] = "{$prefix}[{$entry[nr]}]";
        $entry['field'] = $this->getAttributeList($entry);
      }

      $result = '';
      for ($i = count($path) - 1, $_i = 0; $i >= $_i; $i--)
      {
        $entry = &$path[$i];
        $fieldName = "criterium_{$id}_{$entry[nr]}_other";  
        $readOnly = count($entry['attrs']) == 1;
        $hasLabel = !$readOnly || !is_a($entry['attr'], 'atkmanytoonerelation');
        
        if (!$readOnly)
        {
          $valueName = "criterium_{$id}_value";
          $modeName = "criterium_{$id}_mode";          
          
          $fieldUrl = session_url(dispatch_url($this->m_node->atkNodeType(), 'smartsearch', array('criterium_id' => $id, 'field_nr' => $entry['nr'], 'atkpartial' => 'criteriumfield'), $PHP_SELF), SESSION_NEW);        
          $valueUrl = session_url(dispatch_url($this->m_node->atkNodeType(), 'smartsearch', array('criterium_id' => $id, 'field_nr' => $entry['nr'], 'atkpartial' => 'criteriumvalue'), $PHP_SELF), SESSION_NEW);                
          $modeUrl  = session_url(dispatch_url($this->m_node->atkNodeType(), 'smartsearch', array('criterium_id' => $id, 'field_nr' => $entry['nr'], 'atkpartial' => 'criteriummode'), $PHP_SELF), SESSION_NEW);                          
          
          $scriptCode[] = "ATK.SmartSearchHandler.registerCriteriumFieldListener('{$entry[name]}', '{$prefix}', '{$fieldName}', '{$fieldUrl}', '{$valueName}', '{$valueUrl}', '{$modeName}', '{$modeUrl}')";
        }
        
        $result = $entry['field'].($hasLabel ? '&nbsp;' : '').'<span id="'.$fieldName.'">'.$result.'</span>';                  
      }
        
      return $result;
    }

    /**
     * Returns the criterium value or mode field for the given path.
     * 
     * @param Integer $id    criterium id
     * @param Array   $path  criterium path
     * @param Array   $value current search value
     * @param Array   $mode  current search mode
     * @param String  $type  return either 'value' or 'mode' field
     * 
     * @param String criterium value or mode field HTML
     */
    function _getCriteriumValueOrMode($id, $path, $value, $mode, $type)
    {
      $entry = array_pop($path);
      if ($entry['selectSelf'])
        $entry = array_pop($path);
      $attr = &$entry['attr'];
      
      if ($attr == NULL)
      {
        $node = &$entry['node'];        
        return $node->text('none');
      }
      else 
      {
        
        $prefix = "_AE_criteria_AE_{$id}";    
        if (is_a($attr, 'atkdateattribute') || is_a($attr, 'atkdatetimeattribute')) // workaround for bug in atkDateAttribute
          $prefix = "criteria_AE_{$id}_AE_";        

        $valueArray = $value == NULL ? NULL : array($attr->fieldName() => $value);
        $modeArray = $mode == NULL ? NULL : array($attr->fieldName() => $mode);
        $attr->addToSearchFormFields($fields, $entry['node'], $valueArray, $prefix, $modeArray);
        $field = array_pop($fields);
        
        return $type == 'mode' ? $field['searchmode'] : $field['widget'];
      }
    }

    /**
     * Returns the criterium value field for the given path.
     * 
     * @param Integer $id    criterium id
     * @param Array   $path  criterium path
     * @param Array   $value current search value
     * @param Array   $mode  current search mode
     * 
     * @param String criterium value field HTML
     */
    function getCriteriumValue($id, $path, $value=array(), $mode=array())
    {
      return $this->_getCriteriumValueOrMode($id, $path, $value, $mode, 'value');
    }
      
    /**
     * Returns the criterium mode field for the given path.
     * 
     * @param Integer $id    criterium id
     * @param Array   $path  criterium path
     * @param Array   $value current search value
     * @param Array   $mode  current search mode
     * 
     * @param String criterium mode field HTML
     */
    function getCriteriumMode($id, $path, $value=array(), $mode=array())
    {
      return $this->_getCriteriumValueOrMode($id, $path, $value, $mode, 'mode');     
    }
    
    /**
     * Returns the criterium parameters needed to render a criterium. The data
     * structure contains the already known information about the currently
     * selected field and values (if any).
     *
     * @param String $id   criterium identifier
     * @param Array  $data criterium data
     */
    function getCriterium($id, $data=array())
    {
      $prefix = "criterium_{$id}";
      $path = $this->getNodeAndAttrPath($data['attrs']);
      $scriptCode[] = "ATK.SmartSearchHandler.registerCriterium($id);";

      $result = array();

      $result['id']            = $id;
      $result['element']       = array('box' => "{$prefix}_box", 'field' => "{$prefix}_field", 'value' => "{$prefix}_value", 'mode' => "{$prefix}_mode");
      $result['field']         = $this->getCriteriumField($id, $path, $scriptCode);
      $result['value']         = $this->getCriteriumValue($id, $path, $data['value'], $data['mode']);
      $result['mode']          = $this->getCriteriumMode($id, $path, $data['value'], $data['mode']);      
      $result['template']      = $this->getTemplate('criterium');
      $result['script']        = '<script language="javascript">'.implode("\n", $scriptCode).'</script>';
      $result['remove_action'] = "ATK.SmartSearchHandler.removeCriterium($id)";

      return $result;
    }

    /**
     * Renders a single criterium (field and value).
     *
     * @param Array $criterium criterium structure (from getCriterium)
     * 
     * @return String rendered criterium
     */
    function renderCriterium($criterium)
    {
      $ui = &$this->getUi();
      $params = array();
      $params['label'] = $this->getLabels();
      $params['criterium'] = $criterium;
      return $ui->render('smartcriterium.tpl', $params);
    }
    
    /**
     * List criteria.
     * 
     * @return Array criteria list
     */
    function listCriteria()
    {
      $db = &$this->m_node->getDb(); 
      $query = "SELECT c.name FROM {$this->m_table} c WHERE c.nodetype = '%s' ORDER BY UPPER(c.name)";
      $rows = $db->getRows(sprintf($query, $this->m_node->atkNodeType()));          
      
      $result = array();
      foreach ($rows as $row)
        $result[] = $row['name'];
        
      return $result;
    }
    
    /**
     * Remove search criteria.
     * 
     * @param String $name name of the search criteria
     */
    function forgetCriteria($name)
    {
      atkdebug("Forget criteria $name");      
      $db = &$this->m_node->getDb(); 
      $query = "DELETE FROM {$this->m_table} WHERE nodetype = '%s' AND UPPER(name) = UPPER('%s')";
      $db->query(sprintf($query, $this->m_node->atkNodeType(), escapeSQL($name)));      
    }
    
    /**
     * Save search criteria.
     * 
     * NOTE:
     * This method will overwrite existing criteria with the same name.
     * 
     * @param String $name     name for the search criteria
     * @param Array  $criteria search criteria data 
     */
    function saveCriteria($name, $criteria)
    {
      $this->forgetCriteria($name);
      atk_var_dump($criteria, "Save criteria $name");      
      $db = &$this->m_node->getDb(); 
      $query = "INSERT INTO {$this->m_table} (nodetype, name, criteria) VALUES('%s', '%s', '%s')";
      $db->query(sprintf($query, $this->m_node->atkNodeType(), escapeSQL($name), escapeSQL(serialize($criteria))));
    }
    
    /**
     * Load search criteria.
     * 
     * @param String $name name of the search criteria
     * @return Array search criteria
     */
    function loadCriteria($name)
    {
      $db = &$this->m_node->getDb(); 
      $query = "SELECT c.criteria FROM {$this->m_table} c WHERE c.nodetype = '%s' AND UPPER(c.name) = UPPER('%s')";
      list($row) = $db->getRows(sprintf($query, $this->m_node->atkNodeType(), escapeSQL($name)));      
      $criteria = $row == NULL ? NULL : unserialize($row['criteria']);
      atk_var_dump($criteria, "Load criteria $name");      
      return $criteria;
    }
    
    /**
     * Load base criteria.
     * 
     * @return Array search criteria
     */
    function loadBaseCriteria()
    {
      return array(array('attrs' => array()));              
    }
    
    /**
     * Returns a select list of loadable criteria which will on-selection
     * refresh the smart search page with the loaded criteria.
     * 
     * @return String criteria load HTML
     */
    function getLoadCriteria($current)
    {
      $criteria = $this->listCriteria();
      if (count($criteria) == 0) return NULL;
      
      $result = '
        <select name="load_criteria" onchange="this.form.submit();">
          <option value=""></option>';
      
      foreach ($criteria as $name)
        $result .= '<option value="'.htmlentities($name).'"'.($name == $current ? ' selected' : '').'>'.htmlentities($name).'</option>';
        
      $result .= '</select>';
      return $result;    
    }
    
    /**
     * Returns a link for resetting the currently selected criteria.
     * 
     * @return String reset url
     */
    function getResetCriteria()
    {
      return session_url(dispatch_url($this->m_node->atkNodeType(), $this->m_action), SESSION_REPLACE);
    }
    
    /**
     * Returns a link for removing the currently selected criteria. If
     * nothing (valid) is selected nothing is returned.
     * 
     * @param String $current currently loaded criteria
     * @return String forget url
     */
    function getForgetCriteria($current)
    {
      if (empty($current) || $this->loadCriteria($current) == NULL) return NULL;
      else return session_url(dispatch_url($this->m_node->atkNodeType(), $this->m_action, array('forget_criteria' => $current)), SESSION_REPLACE);
    }
    
    /**
     * Returns a checkbox for enabling/disabling the saving of criteria.
     * 
     * @return checkbox HTML
     */
    function getToggleSaveCriteria()
    {
      return '<input type="checkbox" onclick="$(save_criteria).disabled = !$(save_criteria).disabled">';      
    }
    
    /**
     * Returns a textfield for entering a name to save the search criteria as.
     * 
     * @param String $current currently loaded criteria
     * @param String textfield HTML
     */
    function getSaveCriteria($current)
    {
      return '<input type="text" size="30" name="save_criteria" value="'.htmlentities($current).'" disabled="disabled">';
    }    
    
    /**
     * Fetch posted criteria.
     * 
     * @return Array fetched criteria
     */
    function fetchCriteria()
    {
      $criteriaFields = $this->m_postvars['criteria'];
      $criteriaValues = $this->m_postvars['atksearch']['criteria'];
      $criteriaModes  = $this->m_postvars['atksearchmode']['criteria'];
      
      $criteria = array();
      if (is_array($criteriaFields))
      {
        foreach ($criteriaFields as $id => $field)
        {
          if (isset($criteriaValues[$id]))
          {
            $field = array_merge($field, array('value' => reset($criteriaValues[$id])));
          }
          
          if (isset($criteriaModes[$id]))
          {
            $field = array_merge($field, array('mode' => reset($criteriaModes[$id])));
          }
          
          $criteria[$id] = $field;
        }
      }

      return $criteria;     
    }

    /**
     * This method returns a form that the user can use to search records.
     *
     * @return String The searchform in html form.
     */
    function smartSearchForm($name="", $criteria=array())
    {
      $ui = &$this->getUi();
      
      $params = array();
      
      $params['label']                = $this->getLabels();
      $params['reset_criteria']       = $this->getResetCriteria();
      $params['load_criteria']        = $this->getLoadCriteria($name);
      $params['forget_criteria']      = $this->getForgetCriteria($name);
      $params['toggle_save_criteria'] = $this->getToggleSaveCriteria();      
      $params['save_criteria']        = $this->getSaveCriteria($name);
      
      $params["criteria"] = array();      
      foreach ($criteria as $i => $criterium)
      {
        $params["criteria"][] = $this->getCriterium($i, $criterium);
      }
            
      $url = session_url(dispatch_url($this->m_node->atkNodeType(), 'smartsearch', array('atkpartial' => 'criterium'), $PHP_SELF), SESSION_NEW);
      $params["action_add"] = "ATK.SmartSearchHandler.addCriterium('".addslashes($url)."')";

      return $ui->render($this->getTemplate("form"), $params);
    }

    /**
     * This method returns an html page that can be used as a search form.
     * 
     * @return String The html search page.
     */
    function smartSearchPage($name="", $criteria=array())
    {
      $node = &$this->m_node;
      $page = &$this->getPage();
      $ui = &$this->getUi();
      
      $node->addStyle("style.css");
      
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/prototype/prototype.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/scriptaculous/scriptaculous.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atksmartsearchhandler.js"); 

      // TODO: find some other way to pre-load required JavaScript / CSS files
      $page->register_script(session_url("include.php?file=atk/javascript/class.atkdateattribute.js.inc&atklng=".atkLanguage::getLanguage(), SESSION_NEW));
      $page->register_script(atkconfig("atkroot")."atk/javascript/calendar/calendar.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/calendar/calendar-runner.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/calendar/lang/calendar-".atkconfig("language").".js");      
      
      $theme = &atkinstance("atk.ui.atktheme");
      $page->register_style($theme->stylePath("atkdateattribute.css"));
           
      $params = array();
      
      $params["formstart"]  = 
          '<form name="entryform" action="'.$PHP_SELF.'?'.SID.'" method="post">'
        . session_form(SESSION_REPLACE)
        . '<input type="hidden" name="atkaction" value="smartsearch">'
        . '<input type="hidden" name="atknodetype" value="'.$node->atkNodeType().'">';

      $params["content"] = 
        $this->invoke("smartSearchForm", $name, $criteria);
      
      $params["buttons"][] = 
        '<input type="submit" class="btn_search" name="atkdosearch" value="'.atktext("search", "atk").'">';
        
      $params["formend"] = '</form>';

      $action = $ui->renderAction("smartsearch", $params);
      $box = $ui->renderBox(array('title' => $node->actionTitle('smartsearch'), 'content' => $action));

      return $box;
    }
  }
?>