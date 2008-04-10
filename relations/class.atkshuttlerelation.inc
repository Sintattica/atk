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
     * Return a piece of html code to edit the attribute
     * @param $record Current record
     * @return String piece of html code
     */
    function edit($record, $fieldprefix, $mode)
    {
      $this->createDestination();
      $this->createLink();

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
        if (in_array($this->m_destInstance->primaryKey($recordset[$i]),$selectedPk))
        {
          $right[] = $recordset[$i];
        }
        else
        {
          $left[] = $recordset[$i];
        }

        // fancy autowidth detection
        $width = max(strlen($this->m_destInstance->descriptor($recordset[$i]))*10, $width);  
      }
      
      if ($this->m_maxlistwidth)
      {
        $width = min($this->m_maxlistwidth, $width);
      }

      $result = '<table border="0"><tr><td>'.atktext('available', 'atk').':<br/>';

      $leftname = $fieldprefix.$this->fieldName()."_sel";
      $rightname = $fieldprefix.$this->fieldName().'[]['.$this->getRemoteKey().']';
      $result.= $this->_renderSelect($leftname, $left, $width, $rightname);

      $result.= '</td><td valign="center" align="center">';

      $result.= '<input type="button" value="&gt;" onClick="shuttle_move(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;" onClick="shuttle_move(\''.$rightname.'\', \''.$leftname.'\');"><br/><br/>';
      $result.= '<input type="button" value="&gt;&gt;" onClick="shuttle_moveall(\''.$leftname.'\', \''.$rightname.'\');"><br/>';
      $result.= '<input type="button" value="&lt;&lt;" onClick="shuttle_moveall(\''.$rightname.'\', \''.$leftname.'\');">';

      $result.= '</td><td>'.atktext('selected', 'atk').':<br/>';

      $result.= $this->_renderSelect($rightname, $right, $width, $leftname);

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
    function _renderSelect($name, $recordset, $width, $opposite)
    {
      $result = '<select class="shuttle_select" id="'.$name.'" name="'.$name.'" multiple size="10" style="width: '.$width.'px;" onDblClick="shuttle_move(\''.$name.'\', \''.$opposite.'\')">';
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