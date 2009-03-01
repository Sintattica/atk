<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage relations
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

   /** @internal base include */
   userelation("atkmanytomanyrelation");

  /**
    * Many-to-many relation.
    *
    * The relation shows two lists, one with elements that are currently
    * linked to the master record, and one with available records to choose from
    *
    * @author Ivo Jansch <ivo@achievo.org>
    * @package atk
    * @subpackage relations
    *
    */
  class atkShuttleRelation extends atkManyToManyRelation
  {
    var $m_maxlistwidth = null;

    /**
     * Renders the onchange code on the page.
     *
     * @access private
     * @param String $fieldprefix The prefix to the field
     */
    function _renderChangeHandler($fieldprefix)
    {
      if (count($this->m_onchangecode))
      {
        $page = &$this->m_ownerInstance->getPage();
        $page->register_scriptcode("
    function ".$this->getHtmlId($fieldprefix)."_onChange()
    {
      el = $('".$this->getHtmlId($fieldprefix).'[]['.$this->getRemoteKey().']'."');
      {$this->m_onchangehandler_init}
      ".implode("\n      ", $this->m_onchangecode)."
    }\n");
      }
    }

    /**
     * AtkShuttleRelation expect or an array whith primary keys of the destionation node
     * or a single value that contains the primary key of the destination node.
     * 
     * @param mixed $value
     */
    
    function setInitialValue($value)
    {
      if(!is_array($value))
      {
        $this->m_initialValue = array($value);
      }
      
      $this->m_initialValue = $value;
    }
    
     /**
     * Initial value. Returns the initial value for this attribute
     * which will be used in the add form etc.
     *
     * @return mixed initial value for this attribute
     */
    function initialValue()
    {
      if(!is_array($this->m_initialValue))
      {
        return array();
      }
      return $this->m_initialValue;
    }
    
    
    /**
     * Return a piece of html code to edit the attribute
     * @param $record Current record
     * @return String piece of html code
     */
    function edit($record, $fieldprefix, $mode)
    {
      $this->createDestination();
      $this->createLink();

      $this->_renderChangeHandler($fieldprefix);

      $selectedPk = array();
      // first the selected records..
      for ($i=0;$i<count($record[$this->m_name]);$i++)
      {
        if(is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
          $newselected = $this->m_destInstance->primaryKey($record[$this->m_name][$i][$this->getRemoteKey()]);
        else{
          $newselected = $this->m_destInstance->primaryKey(array($this->m_destInstance->primaryKeyField()=>$record[$this->m_name][$i][$this->getRemoteKey()]));
        }
        $selectedPk[] = $newselected;
      }

      $recordset = $this->_getSelectableRecords($record, $mode);

      $left = array();
      $right = array();
      $width = 100;
      
    
      for ($i=0;$i<count($recordset);$i++)
      {
        if (in_array($this->m_destInstance->primaryKey($recordset[$i]),$selectedPk) || (in_array($recordset[$i][$this->m_destInstance->primaryKeyField()],$this->initialValue()) && $mode=='add'))
        {
          $right[] = $recordset[$i];
        }
        else
        {
          $left[] = $recordset[$i];
        }

        // fancy autowidth detection
        $width = max(atk_strlen($this->m_destInstance->descriptor($recordset[$i]))*10, $width);  
      }

      if ($this->m_maxlistwidth)
      {
        $width = min($this->m_maxlistwidth, $width);
      }

      $result = '<table border="0"><tr><td>'.atktext('available', 'atk').':<br/>';

      $fieldname = $fieldprefix.$this->fieldName();
      $leftname = $fieldname."_sel";
      $rightname = $fieldname.'[]['.$this->getRemoteKey().']';
      $result.= $this->_renderSelect($leftname, $left, $width, $rightname, $fieldname);

      $result.= '</td><td valign="center" align="center">';

      $result.= '<input type="button" value="&gt;" onClick="shuttle_move(\''.$leftname.'\', \''.$rightname.'\', \''.$fieldname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;" onClick="shuttle_move(\''.$rightname.'\', \''.$leftname.'\', \''.$fieldname.'\');"><br/><br/>';
      $result.= '<input type="button" value="&gt;&gt;" onClick="shuttle_moveall(\''.$leftname.'\', \''.$rightname.'\', \''.$fieldname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;&lt;" onClick="shuttle_moveall(\''.$rightname.'\', \''.$leftname.'\', \''.$fieldname.'\');">';

      $result.= '</td><td>'.atktext('selected', 'atk').':<br/>';

      $result.= $this->_renderSelect($rightname, $right, $width, $leftname, $fieldname);

      // on submit, we must select all items in the right selector, as unselected items
      // will not be posted.
      $page = &$this->m_ownerInstance->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/class.atkshuttlerelation.js");
      $page->register_submitscript("shuttle_selectAll('".$rightname."');");

      $result.= '</table>';

      return $result;
    }

    /**
     * Render the multiselect list control
     * @access private
     * @param String $name The name of the list control
     * @param array $recordset The list of records to render in the control
     * @param int $width The width of the control in pixels
     * @param String $opposite The name of the list control connected to this list control for shuttle actions
     * @return String piece of html code
     */
    function _renderSelect($name, $recordset, $width, $opposite, $fieldname)
    {
      $result = '<select class="shuttle_select" id="'.$name.'" name="'.$name.'" multiple size="10" style="width: '.$width.'px;" onDblClick="shuttle_move(\''.$name.'\', \''.$opposite.'\', \''.$fieldname.'\')">';
      for($i=0, $_i=count($recordset); $i<$_i; $i++)
      {
        $result.= '<option value="'.$recordset[$i][$this->m_destInstance->primaryKeyField()].'">'.$this->m_destInstance->descriptor($recordset[$i]);
      }
      $result.= '</select>';
      return $result;
    }

    /**
     * Set the maximum width of the listboxes.
     *
     * @param int $width
     */
    function setMaxListWidth($width)
    {
      $this->m_maxlistwidth = $width;
    }

  }

?>