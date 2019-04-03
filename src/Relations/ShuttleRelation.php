<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;

/**
 * Many-to-many relation.
 *
 * The relation shows two lists, one with elements that are currently
 * linked to the master record, and one with available records to choose from
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ShuttleRelation extends ManyToManyRelation
{
    public $m_maxlistwidth = null;

    public $m_filterBox = false;

    /**
     * Renders the onchange code on the page.
     *
     * @param string $fieldprefix The prefix to the field
     * @param string $elementNr
     */
    public function _renderChangeHandler($fieldprefix, $elementNr = '')
    {
        if (Tools::count($this->m_onchangecode)) {
            $page = $this->m_ownerInstance->getPage();
            $page->register_scriptcode('
    function '.$this->getHtmlId($fieldprefix)."_onChange()
    {
      el = $('".$this->getHtmlId($fieldprefix).'[]['.$this->getRemoteKey().']'."');
      {$this->m_onchangehandler_init}
      ".implode("\n      ", $this->m_onchangecode)."
    }\n");
        }
    }

    /**
     * ShuttleRelation expect or an array whith primary keys of the destionation node
     * or a single value that contains the primary key of the destination node.
     *
     * @param mixed $value
     *
     * @return $this
     */
    public function setInitialValue($value)
    {
        if (!is_array($value)) {
            $this->m_initialValue = array($value);
        }

        $this->m_initialValue = $value;

        return $this;
    }

    /**
     * Initial value. Returns the initial value for this attribute
     * which will be used in the add form etc.
     *
     * @return mixed initial value for this attribute
     */
    public function initialValue()
    {
        if (!is_array($this->m_initialValue)) {
            return [];
        }

        return $this->m_initialValue;
    }

    /**
     * Show an input field that can filter the option lists.
     */
    public function showFilterBox()
    {
        $this->m_filterBox = true;
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $this->createDestination();
        $this->createLink();

        $this->_renderChangeHandler($fieldprefix);

        $selectedPk = [];
        // first the selected records..
        for ($i = 0; $i < Tools::count($record[$this->m_name]); ++$i) {
            if (is_array($record[$this->fieldName()][$i][$this->getRemoteKey()])) {
                $newselected = $this->m_destInstance->primaryKeyString($record[$this->m_name][$i][$this->getRemoteKey()]);
            } else {
                $newselected = $this->m_destInstance->primaryKeyString(array(
                    $this->m_destInstance->primaryKeyField() => $record[$this->m_name][$i][$this->getRemoteKey()],
                ));
            }
            $selectedPk[] = $newselected;
        }

        $recordset = $this->_getSelectableRecords($record, $mode);

        $left = [];
        $right = [];
        $width = 100;

        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            if (in_array($this->m_destInstance->primaryKeyString($recordset[$i]), $selectedPk) || (in_array($recordset[$i][$this->m_destInstance->primaryKeyField()],
                        $this->initialValue()) && $mode == 'add')
            ) {
                $right[] = $recordset[$i];
            } else {
                $left[] = $recordset[$i];
            }

            // fancy autowidth detection
            $width = max(Tools::atk_strlen($this->m_destInstance->descriptor($recordset[$i])) * 10, $width);
        }

        if ($this->m_maxlistwidth) {
            $width = min($this->m_maxlistwidth, $width);
        }

        $result = '<table border="0" class="'.$this->get_class_name().'"><tr><td>'.Tools::atktext('available', 'atk').':<br/>';

        $fieldname = $fieldprefix.$this->fieldName();
        $leftname = $fieldname.'_sel';
        $rightname = $fieldname.'[]['.$this->getRemoteKey().']';
        $filterbox_left = false;
        $filterbox_right = false;
        $rightname_clean = '';

        if ($this->m_filterBox) {
            // fix for selecting with jQuery
            // css ids shouldn't contain characters like [ and ] : http://www.w3.org/TR/html5/dom.html#the-id-attribute
            $rightname_clean = str_replace('[', '\\\\[', $rightname);
            $rightname_clean = str_replace(']', '\\\\]', $rightname_clean);

            $filterbox_left = $fieldname.'left_filter_box';
            $filterbox_right = $fieldname.'right_filter_box';
        }

        $result .= $this->_renderSelect($leftname, $left, $width, $rightname, $fieldname, $filterbox_left);

        $result .= '</td><td class="shuttle-controls">';

        if ($this->m_filterBox) {
            // move buttons down a little if filter boxes are being displayed
            $result .= '<br><br><br>';
        }

        $result .= '<div class="shuttle-controls-single">';
        $result .= '<button class="btn btn-default" onClick="ATK.ShuttleRelation.shuttle_move(\''.$leftname.'\', \''.$rightname.'\', \''.$fieldname.'\');return false;"><i class="glyphicon glyphicon-triangle-right"></i></button>';
        $result .= '<button class="btn btn-default" type="button" value="&lt;" onClick="ATK.ShuttleRelation.shuttle_move(\''.$rightname.'\', \''.$leftname.'\', \''.$fieldname.'\');return false;"><i class="glyphicon glyphicon-triangle-left"></i></button>';
        $result .= '</div>';

        $result .= '<div class="shuttle-controls-multiple">';
        $result .= '<button class="btn btn-default" type="button" value="&gt;&gt;" onClick="ATK.ShuttleRelation.shuttle_moveall(\''.$leftname.'\', \''.$rightname.'\', \''.$fieldname.'\');return false;"><i class="glyphicon glyphicon-forward"></i></button>';
        $result .= '<button class="btn btn-default" type="button" value="&lt;&lt;" onClick="ATK.ShuttleRelation.shuttle_moveall(\''.$rightname.'\', \''.$leftname.'\', \''.$fieldname.'\');return false;"><i class="glyphicon glyphicon-backward"></i></button>';
        $result .= '</div>';

        $result .= '</td><td>'.Tools::atktext('selected', 'atk').':<br/>';

        $result .= $this->_renderSelect($rightname, $right, $width, $leftname, $fieldname, $filterbox_right);

        // on submit, we must select all items in the right selector, as unselected items
        // will not be posted.
        $page = $this->m_ownerInstance->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/shuttlerelation.js');

        if ($this->m_filterBox && $rightname_clean && $filterbox_right && $filterbox_left && $rightname &&  $leftname) {
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
        $page->register_submitscript("ATK.ShuttleRelation.shuttle_selectAll('".$rightname."');");

        $result .= '</table>';

        return $result;
    }

    /**
     * Render the multiselect list control.
     *
     * @param string $name The name of the list control
     * @param array $recordset The list of records to render in the control
     * @param int $width The width of the control in pixels
     * @param string $opposite The name of the list control connected to this list control for shuttle actions
     * @param string $fieldname The fieldname
     * @param string $filterbox Filter box id (if present)
     *
     * @return string piece of html code
     */
    public function _renderSelect($name, $recordset, $width, $opposite, $fieldname, $filterbox = '')
    {
        $result = '';
        if ($filterbox != '') {
            $result .= '<input id="'.$filterbox.'" class="form-control input-sm" placeholder="'.$this->text('filter').'..."
                style="width: '.($width - 10).'px; margin-bottom: 5px; max-width: 400px !important;"><div style="clear:both"></div>';
        }
        $result .= '<select class="form-control shuttle_select" id="'.$name.'" name="'.$name.'" multiple size="10" style="width: '.$width.'px;" onDblClick="ATK.ShuttleRelation.shuttle_move(\''.$name.'\', \''.$opposite.'\', \''.$fieldname.'\')">';
        for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
            $result .= '<option value="'.$recordset[$i][$this->m_destInstance->primaryKeyField()].'">'.$this->m_destInstance->descriptor($recordset[$i]);
        }
        $result .= '</select>';

        return $result;
    }

    /**
     * Set the maximum width of the listboxes.
     *
     * @param int $width
     */
    public function setMaxListWidth($width)
    {
        $this->m_maxlistwidth = $width;
    }
}
