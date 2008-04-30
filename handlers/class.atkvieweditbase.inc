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

  atkimport("atk.handlers.atkactionhandler");

  /**
   * Handler class for the edit action of a node. The handler draws a
   * generic edit form for the given node.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage handlers
   *
   */
  class atkViewEditBase extends atkActionHandler
  {
    /**
     * Get section label.
     * 
     * @param atkNode $node
     * @param string $rawName
     * 
     * @return string label
     * 
     * @static 
     */
    function getSectionLabel($node, $rawName)
    {
      list($tab, $section) = explode('.', $rawName);
      $strings = array("section_{$tab}_{$section}", "{$tab}_{$section}", "section_{$section}", $section);
      return $node->text($strings);
    }
    
    /**
     * Get tab label.
     *
     * @param atkNode $node
     * @param string $tab
     * 
     * @return string label
     * 
     * @static 
     */
    function getTabLabel($node, $tab)
    {
      $strings = array("tab_{$tab}", $tab);
      return $node->text($strings);
    }    
    
    /**
     * Create the clickable label for the section.
     *
     * @param array $field
     * @return string Html
     */
    function getSectionControl($field, $mode)
    {
      // label
      $label = atkViewEditBase::getSectionLabel($this->m_node, $field['name']);
      
      // our name
      list($tab, $section) = explode('.', $field["name"]);
      $name = "section_{$tab}_{$section}";

      $url = partial_url($this->m_node->atknodetype(), $mode, "sectionstate",array("atksectionname"=>$name));

      // create onclick statement.
      $onClick = " onClick=\"javascript:handleSectionToggle(this,null,'{$url}'); return false;\"";
      $initClass = "openedSection";

      //if the section is not active, we close it on load.
      $state = atknew("atk.session.atkstate");
	    $sectionstate = $state->get(array("nodetype"=>$this->m_node->atknodetype(),"section"=>$name));
			
      if( $sectionstate == 'closed' || ($sectionstate == null && !in_array($field["name"],$this->m_node->getActiveSections($tab, $mode))))  
      {
        $initClass =  "closedSection";
        $page = &$this->getPage();
        $page->register_scriptcode("addClosedSection('$name');");
      }

      // create the clickable link
      return '<span class="atksectionwr"><a href="javascript:void(0)" id="'.$name.'" class="atksection '.$initClass.'"'.$onClick.'>'.$label.'</a></span>';
    }

    /**
     * Based on the attributes that are part of this section we
     * check if this section should initially be shown or not.
     *
     * @param string $section section name
     * @param array $fields edit fields
     * @return boolean
     */
    function isSectionInitialHidden($section, $fields)
    {
      foreach ($fields as $field)
      {
        if (is_array($field["sections"]) && in_array($section, $field['sections']) && (!isset($field['initial_hidden']) || !$field['initial_hidden']))
        {
          return false;
        }
      }

      return true;
    }

    /**
     * Adds numbering to the label of a field
     * @access private
     * @param array $field    the currently handled attribute
     * @param array $tplfield the template data for the current attribute
     * @param int $i          the counter being used to loop the node for each attribute
     */
    function _addNumbering(&$field, &$tplfield, &$i)
    {
      static $number, $subnumber;

      if (!$number && !$subnumber) $number = $field["attribute"]->m_ownerInstance->getNumbering();
      if (!$subnumber)
      {
        if (strlen($number) == 1 || (floor($number) <= 9 && floor($number) >= -9 && floor($number) == $number))
        {
          $subnumber = $number;
          $number = null;
        }
        else
        {
          $subnumber = substr($number, strrpos($number, ".")+1);
          $number = substr($number, 0, strrpos($number, "."));
        }
      }

      if ($field["label"])
      {
        if ($number) $tplfield["label"]="$number.$subnumber. ";
        else $tplfield["label"]="$subnumber. ";
        $subnumber++;
      }
    }

    /**
     * Section state handler.
     */
    function partial_sectionstate()
    {
      atkimport("atk.session.atkstate");
    	atkState::set(array("nodetype"=>$this->m_node->atknodetype(),"section"=>$this->m_postvars['atksectionname']),$this->m_postvars['atksectionstate']);
    	die;
    }

    /**
     * Get array with tab name as key and tab template as value
     *
     * @param object $node
     * @param array $tabs
     * @param string $mode
     * @param array $record
     * @return array with tab=>template pear
     */
    function _getTabTpl($node, $tabs, $mode, $record)
    {
      $tabTpl = array();
      foreach ($tabs as $t)
      {
        $tabTpl['section_'.$t] = $node->getTemplate($mode, $record, $t);
      }
      return $tabTpl;
    }
    
    /**
     * Render tabs using templates
     * 
     * @todo this method seems broken by design, read comments for more info!
     *
     * @param array $fields
     * @param array $tabTpl
     * @return array with already rendering tabs
     */
    function _renderTabs($fields, $tabTpl)
    {
      $ui = &$this->getUi();
      $tabs = array();
      $perTpl = array();//per template array
      
      for($i=0,$_i=count($fields);$i<$_i;$i++)
      {
        $allTabs = explode(' ', $fields[$i]["tab"]); // should not use "tab" here, because it actually contains the CSS class names and not only the tab names
        $allMatchingTabs = array_values(array_intersect($allTabs, array_keys($tabTpl))); // because of the CSS thingee above we search for the first matching tab
        if (count($allMatchingTabs) == 0) $allMatchingTabs = array_keys($tabTpl); // again a workaround for this horribly broken method
        $tab = $allMatchingTabs[0]; // attributes can be part of one, more than one or all tabs, at the moment it seems only one or all are supported
        $perTpl[$tabTpl[$tab]]['fields'][] = $fields[$i];//make field available in numeric array
        $perTpl[$tabTpl[$tab]][$fields[$i]["attribute"]] = $fields[$i];//make field available in associative array
      }
      
      // Add 'alltab' fields to all templates
      foreach ($fields as $field)
      {
        if (in_array('alltabs', explode(' ', $field["tab"])))
        {
          $templates = array_keys($perTpl);
          foreach ($templates as $tpl)
          {
            if (!$perTpl[$tpl][$field['attribute']])
            {
              $perTpl[$tpl]['fields'][] = $field;
              $perTpl[$tpl][$field['attribute']] = $field;
            }
          }
        }
      }
      
      $tpls = array_unique(array_values($tabTpl));
      foreach ($tpls as $tpl)
      {
        $tabs[] = $ui->render($tpl,$perTpl[$tpl]);  
      }
      
      return $tabs;
    }
    
    /**
     * Attribute handler.
     *
     * @param string $partial full partial
     */
    public function partial_attribute($partial)
    {
      list(, $attribute, $partial) = explode('.', $partial);

      $attr = $this->m_node->getAttribute($attribute);
      if ($attr == NULL)
      {
        atkerror("Unknown / invalid attribute '$attribute' for node '".$this->m_node->atkNodeType()."'");
        return '';
      }

      return $attr->partial($partial, $this->m_action);
    }    
  }
?>