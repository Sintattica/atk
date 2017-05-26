<?php

namespace Sintattica\Atk\Relations;

/**
 * Many-to-many list relation.
 *
 * The relation shows a list of available records in a selection list
 * from which multiple records can be selected.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class ManyToManyListRelation extends ManyToManyRelation
{
    private $m_rows = 6;
    private $m_autoCalculateRows = true;

    /**
     * Auto calculate rows based on the available rows. The set
     * rows will be used as maximum. This is enabled by default.
     *
     * @param bool $enable enable?
     */
    public function setAutoCalculateRows($enable)
    {
        $this->m_autoCalculateRows = $enable;
    }

    /**
     * Is auto calculate rows enabled?
     *
     * @return bool auto-calculate rows enabled?
     */
    public function autoCalculateRows()
    {
        return $this->m_autoCalculateRows;
    }

    /**
     * Get rows.
     *
     * @return int rows
     */
    public function getRows()
    {
        return $this->m_rows;
    }

    /**
     * Set rows.
     *
     * @param int $rows
     */
    public function setRows($rows)
    {
        $this->m_rows = $rows;
    }

    /**
     * Return a piece of html code to edit the attribute.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code
     */
    public function edit($record, $fieldprefix, $mode)
    {
        $this->createDestination();
        $this->createLink();

        $selected = $this->getSelectedRecords($record);
        $selectable = $this->_getSelectableRecords($record, $mode);

        if (count($selectable) == 0) {
            return $this->text('select_none');
        }

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $size = $this->autoCalculateRows() ? min(count($selectable), $this->getRows()) : $this->getRows();

        $style = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }

        $result = '<select class="form-control" id="'.$id.'" name="'.$name.'[]['.$this->getRemoteKey().']"';
        $result .= ' multiple="multiple" size="'.$size.'"';
        if($style != ''){
            $result .= ' style="'.$style.'"';
        }
        $result .= '>';

        foreach ($selectable as $row) {
            $key = $this->m_destInstance->primaryKey($row);
            $label = $this->m_destInstance->descriptor($row);
            $selectedStr = in_array($key, $selected) ? ' selected="selected"' : '';
            $value = $row[$this->m_destInstance->primaryKeyField()];

            $result .= '<option value="'.htmlentities($value).'"'.$selectedStr.'>'.$label.'</option>';
        }

        $result .= '</select>';

        return $result;
    }
}
