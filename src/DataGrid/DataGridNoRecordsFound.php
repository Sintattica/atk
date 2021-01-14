<?php

namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Core\Tools;

/**
 * The data grid no records found message. Can be used to render a
 * simple message underneath the grid stating there are no records
 * found in the database.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class DataGridNoRecordsFound extends DataGridComponent
{
    /**
     * Renders the no records found message for the given data grid.
     *
     * @return null|string rendered HTML
     */
    public function render(): ?string
    {
        $grid = $this->getGrid();

        $usesIndex = $grid->getIndex() != null;
        $isSearching = is_array($grid->getPostvar('atksearch')) && Tools::count($grid->getPostvar('atksearch')) > 0;

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
