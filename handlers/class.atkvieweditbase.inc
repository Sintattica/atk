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
     * Create the clickable label for the section.
     *
     * @param array $field
     * @return string Html
     */
    function getSectionControl($field, $mode)
    {
      // our name
      list($tab, $section) = explode('.', $field["name"]);
      $name = "section_{$tab}_{$section}";

      $url = partial_url($this->m_node->atknodetype(), $mode, "sectionstate",array("atksectionname"=>$name));

      // create onclick statement.
      $onClick = " onClick=\"javascript:handleSectionToggle(this,null,'{$url}');\"";

      // label
      $label = atktext($name, $this->m_node->m_module, $this->m_node->m_type, '', '', true);
      if ($label == NULL)
        $label = atktext("{$tab}_{$section}", $this->m_node->m_module, $this->m_node->m_type, '', '', true);
      if ($label == NULL)
        $label = $this->m_node->text($section);

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
      return '<a href="javascript:void(0)" id="'.$name.'" class="atksection '.$initClass.'"'.$onClick.'>'.$label.'</a>';
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

  }
?>