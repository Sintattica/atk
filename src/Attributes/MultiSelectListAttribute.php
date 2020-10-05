<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;

class MultiSelectListAttribute extends MultiSelectAttribute
{
    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record Array with fields
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code with radioboxes
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $type = 'edit';

        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['dropdown-auto-width'] = true;
        $selectOptions['minimum-results-for-search'] = 10;
        $selectOptions['multiple'] = true;
        $selectOptions['placeholder'] = $this->getNullLabel();
        $selectOptions = array_merge($selectOptions, $this->m_select2Options['edit']);

        $data = '';
        foreach ($selectOptions as $k => $v) {
            $data .= ' data-'.$k.'="'.htmlspecialchars($v).'"';
        }

        if($this->getCssStyle($type, 'width') === null && $this->getCssStyle($type, 'min-width') === null) {
            $this->setCssStyle($type, 'min-width', '220px');
        }

        $style = $styles = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if($style != ''){
            $styles = ' style="'.$style.'"';
        }

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = ' onChange="'.$this->getHtmlId($fieldprefix).'_onChange(this)"';
            $this->_renderChangeHandler($fieldprefix);
        }

        $result = '<select multiple id="'.$id.'" name="'.$name.'[]" '.$this->getCSSClassAttribute('form-control').'" '.$onchange.$data.$styles.'>';

        $values = $this->getValues();
        if (!is_array($record[$this->fieldName()])) {
            $recordvalue = $this->db2value($record);
        } else {
            $recordvalue = $record[$this->fieldName()];
        }

        for ($i = 0; $i < Tools::count($values); ++$i) {
            // If the current value is selected or occurs in the record
            $sel = (Tools::atk_in_array($values[$i], $recordvalue)) ? 'selected' : '';

            $result .= '<option value="'.$values[$i].'" '.$sel.'>'.$this->_translateValue($values[$i], $record);
        }

        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";

        return $result;
    }
}
