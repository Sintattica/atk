<?php namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Utils\StringParser;

/**
 * The data grid summary. Can be used to render a
 * summary for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class DataGridSummary extends DataGridComponent
{

    /**
     * Renders the summary for the given data grid.
     *
     * @return string rendered HTML
     */
    public function render()
    {
        $grid = $this->getGrid();

        $limit = $grid->getLimit();
        $count = $grid->getCount();

        if ($count == 0) {
            return null;
        }

        if ($limit == -1) {
            $limit = $count;
        }

        $start = $grid->getOffset();
        $end = min($start + $limit, $count);
        $page = floor(($start / $limit) + 1);
        $pages = ceil($count / $limit);

        $string = $grid->text('datagrid_summary');

        $params = array(
            'start' => $start + 1,
            'end' => $end,
            'count' => $count,
            'limit' => $limit,
            'page' => $page,
            'pages' => $pages
        );

        $parser = new StringParser($string);
        $result = $parser->parse($params);

        return '<span class="dgridsummary">' . $result . '</span>';
    }

}

