<?php namespace Sintattica\Atk\DataGrid;


/**
 * Grid event.
 *
 * @see DGListener
 * @see DataGrid
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class DataGridEvent
{
    /**
     * Event will be triggered at the start of the call to DataGrid::render,
     * before the grid or any of it's components have been rendered.
     */
    const PRE_RENDER = "preRender";

    /**
     * Event will be triggered at the end of the call to DataGrid::render,
     * after all component and the grid itself have been rendered.
     */
    const POST_RENDER = "postRender";

    /**
     * Event will be triggered at the start of the call to
     * DataGrid::loadRecords, before the records are loaded.
     */
    const PRE_LOAD = "preLoad";

    /**
     * Event will be triggered at the end of the call to
     * DataGrid::loadRecords, after the records are loaded.
     */
    const POST_LOAD = "postLoad";

    /**
     * Grid.
     *
     * @var DataGrid
     */
    private $m_grid;

    /**
     * Event identifier.
     *
     * @var int
     */
    private $m_event;

    /**
     * Constructs a new event
     *
     * @param DataGrid $grid grid
     * @param string $event event identifier
     */
    public function __construct(DataGrid $grid, $event)
    {
        $this->m_grid = $grid;
        $this->m_event = $event;
    }

    /**
     * Returns the grid for this event.
     *
     * @return DataGrid grid
     */
    public function getGrid()
    {
        return $this->m_grid;
    }

    /**
     * Returns the event identifier.
     *
     * @return string event
     */
    public function getEvent()
    {
        return $this->m_event;
    }

}
