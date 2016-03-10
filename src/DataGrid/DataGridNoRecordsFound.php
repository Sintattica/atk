<?php namespace Sintattica\Atk\DataGrid;

/**
 * The data grid no records found message. Can be used to render a
 * simple message underneath the grid stating there are no records
 * found in the database.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class DataGridNoRecordsFound extends DataGridComponent
{

    /**
     * Renders the no records found message for the given data grid.
     *
     * @return string rendered HTML
     */
    public function render()
    {
        $grid = $this->getGrid();

        $usesIndex = $grid->getIndex() != null;
        $isSearching = is_array($grid->getPostvar('atksearch')) && count($grid->getPostvar('atksearch')) > 0;

        if ($grid->getCount() == 0 && ($usesIndex || $isSearching)) {
            return $grid->text('datagrid_norecordsfound_search');
        } else {
            if ($grid->getCount() == 0) {
                return $grid->text('datagrid_norecordsfound_general');
            } else {
                return null;
            }
        }
    }
}
