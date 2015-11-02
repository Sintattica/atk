<?php
/**
 * This file is part of the ATK distribution on GitHub.
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
 * @version $Revision: 6320 $
 * $Id: class.atkshuttlerelation.inc 6354 2009-04-15 02:41:21Z mvdam $
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

    var $m_filterBox = false;

    /**
     * Renders the onchange code on the page.
     *
     * @access private
     * @param String $fieldprefix The prefix to the field
     */
    function _renderChangeHandler($fieldprefix)
    {
        if (count($this->m_onchangecode)) {
            $page = &$this->m_ownerInstance->getPage();
            $page->register_scriptcode("
    function " . $this->getHtmlId($fieldprefix) . "_onChange()
    {
      el = $('" . $this->getHtmlId($fieldprefix) . '[][' . $this->getRemoteKey() . ']' . "');
      {$this->m_onchangehandler_init}
      " . implode("\n      ", $this->m_onchangecode) . "
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
        if (!is_array($value)) {
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
        if (!is_array($this->m_initialValue)) {
            return array();
        }
        return $this->m_initialValue;
    }


    /**
     * Show an input field that can filter the option lists
     */
    function showFilterBox()
    {
        $this->m_filterBox = true;
    }

    /**
     * Return a piece of html code to edit the attribute
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String piece of html code
     */
    function edit($record, $fieldprefix, $mode)
    {
        $this->createDestination();
        $this->createLink();

        $this->_renderChangeHandler($fieldprefix);

        $selectedPk = array();
        // first the selected records..
        for ($i = 0; $i < count($record[$this->m_name]); $i++) {
            if (is_array($record[$this->fieldName()][$i][$this->getRemoteKey()]))
                $newselected = $this->m_destInstance->primaryKey($record[$this->m_name][$i][$this->getRemoteKey()]);
            else {
                $newselected = $this->m_destInstance->primaryKey(array(
                    $this->m_destInstance->primaryKeyField() => $record[$this->m_name][$i][$this->getRemoteKey()]));
            }
            $selectedPk[] = $newselected;
        }

        $recordset = $this->_getSelectableRecords($record, $mode);

        $left = array();
        $right = array();
        $width = 100;


        for ($i = 0; $i < count($recordset); $i++) {
            if (in_array($this->m_destInstance->primaryKey($recordset[$i]), $selectedPk) || (in_array($recordset[$i][$this->m_destInstance->primaryKeyField()], $this->initialValue()) && $mode == 'add')) {
                $right[] = $recordset[$i];
            } else {
                $left[] = $recordset[$i];
            }

            // fancy autowidth detection
            $width = max(atk_strlen($this->m_destInstance->descriptor($recordset[$i])) * 10, $width);
        }

        if ($this->m_maxlistwidth) {
            $width = min($this->m_maxlistwidth, $width);
        }

        $result = '<table border="0"><tr><td>' . atktext('available', 'atk') . ':<br/>';

        $fieldname = $fieldprefix . $this->fieldName();
        $leftname = $fieldname . "_sel";
        $rightname = $fieldname . '[][' . $this->getRemoteKey() . ']';

        if ($this->m_filterBox) {
            // fix for selecting with jQuery
            // css ids shouldn't contain characters like [ and ] : http://www.w3.org/TR/html5/dom.html#the-id-attribute
            $rightname_clean = str_replace('[', '\\\\[', $rightname);
            $rightname_clean = str_replace(']', '\\\\]', $rightname_clean);

            $filterbox_left = $fieldname . 'left_filter_box';
            $filterbox_right = $fieldname . 'right_filter_box';
        }

        $result .= $this->_renderSelect($leftname, $left, $width, $rightname, $fieldname, $filterbox_left);

        $result .= '</td><td valign="center" align="center">';

        if ($this->m_filterBox) {
            // move buttons down a little if filter boxes are being displayed
            $result .= '<br><br><br>';
        }

        $result .= '<input class="btn btn-default" type="button" value="&gt;" onClick="shuttle_move(\'' . $leftname . '\', \'' . $rightname . '\', \'' . $fieldname . '\');"><br/>';
        $result .= '<input class="btn btn-default" type="button" value="&lt;" onClick="shuttle_move(\'' . $rightname . '\', \'' . $leftname . '\', \'' . $fieldname . '\');"><br/><br/>';
        $result .= '<input class="btn btn-default" type="button" value="&gt;&gt;" onClick="shuttle_moveall(\'' . $leftname . '\', \'' . $rightname . '\', \'' . $fieldname . '\');"><br/>';
        $result .= '<input class="btn btn-default" type="button" value="&lt;&lt;" onClick="shuttle_moveall(\'' . $rightname . '\', \'' . $leftname . '\', \'' . $fieldname . '\');">';

        $result .= '</td><td>' . atktext('selected', 'atk') . ':<br/>';

        $result .= $this->_renderSelect($rightname, $right, $width, $leftname, $fieldname, $filterbox_right);

        // on submit, we must select all items in the right selector, as unselected items
        // will not be posted.
        $page = &$this->m_ownerInstance->getPage();
        $page->register_script(atkconfig("atkroot") . "atk/javascript/class.atkshuttlerelation.js");

        if ($this->m_filterBox) {
            // do the filtering
            $page->register_scriptcode("
                jQuery(function($){
                    $('#$filterbox_left').on('input', function() {
                        var val = this.value.toLowerCase();
                        $('#$leftname > option').hide()
                        .filter(function() {
                            return this.text.toLowerCase().indexOf( val ) > -1;
                        })
                        .show();
                    });

                     $('#$filterbox_right').on('input', function() {
                        var val = this.value.toLowerCase();
                        $('#$rightname_clean > option').hide()
                        .filter(function() {
                            return this.text.toLowerCase().indexOf( val ) > -1;
                        })
                        .show();
                    });
                });
            ");
        }
        $page->register_submitscript("shuttle_selectAll('" . $rightname . "');");

        $result .= '</table>';

        return $result;
    }

    /**
     * Render the multiselect list control
     * @access private
     * @param String $name The name of the list control
     * @param array $recordset The list of records to render in the control
     * @param int $width The width of the control in pixels
     * @param String $opposite The name of the list control connected to this list control for shuttle actions
     * @param String $fieldname The fieldname
     * @param String $filterbox Filter box id (if present)
     * @return String piece of html code
     */
    function _renderSelect($name, $recordset, $width, $opposite, $fieldname, $filterbox = false)
    {
        $result = '';
        if ($filterbox) {
            $result .= '<input id="' . $filterbox . '" class="form-control input-sm" placeholder="'. $this->text('filter') .'..."
                style="width: ' . ($width-10) . 'px; margin-bottom: 5px; max-width: 400px !important;"><div style="clear:both"></div>';
        }
        $result .= '<select class="form-control shuttle_select" id="' . $name . '" name="' . $name . '" multiple size="10" style="width: ' . $width . 'px;" onDblClick="shuttle_move(\'' . $name . '\', \'' . $opposite . '\', \'' . $fieldname . '\')">';
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++) {
            $result .= '<option value="' . $recordset[$i][$this->m_destInstance->primaryKeyField()] . '">' . $this->m_destInstance->descriptor($recordset[$i]);
        }
        $result .= '</select>';
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