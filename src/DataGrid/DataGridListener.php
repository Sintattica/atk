<?php namespace Sintattica\Atk\DataGrid;

/**
 * The data grid listener can be implemented and registered for a data grid
 * to listen for data grid events.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
interface DataGridListener
{

    /**
     * Will be called for each data grid event.
     *
     * @param DataGridEvent $event event
     */
    public function notify(DataGridEvent $event);
}
