<?php

namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Ui;

/**
 * The data grid component base class. All data grid component extend this
 * class and implement the render method.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
abstract class DataGridComponent
{
    /**
     * The DataGrid.
     *
     * @var DataGrid
     */
    private $m_grid;

    /**
     * The component options.
     *
     * @var array
     */
    private $m_options;

    /**
     * Constructor.
     *
     * @param DataGrid $grid    grid
     * @param array    $options component options
     */
    public function __construct($grid, $options = array())
    {
        $this->m_grid = $grid;
        $this->m_options = $options;
    }

    /**
     * Destroy.
     */
    public function destroy()
    {
        $this->m_grid = null;
    }

    /**
     * Returns the value for the component option with the given name.
     *
     * @param string $name     option name
     * @param string $fallback
     *
     * @return mixed option value
     */
    protected function getOption($name, $fallback = null)
    {
        return isset($this->m_options[$name]) ? $this->m_options[$name] : $fallback;
    }

    /**
     * Returns the data grid.
     *
     * @return DataGrid data grid
     */
    protected function getGrid()
    {
        return $this->m_grid;
    }

    /**
     * Returns the data grid node.
     *
     * @return Node node
     */
    protected function getNode()
    {
        return $this->getGrid()->getNode();
    }

    /**
     * Returns the page object.
     *
     * @return Page page
     */
    protected function getPage()
    {
        return $this->getNode()->getPage();
    }

    /**
     * Returns the UI object.
     *
     * @return Ui ui
     */
    protected function getUi()
    {
        return $this->getNode()->getUi();
    }

    /**
     * Translate the given string using the grid node.
     *
     * The value of $fallback will be returned if no translation can be found.
     * If you want NULL to be returned when no translation can be found then
     * leave the fallback empty and set $useDefault to false.
     *
     * @param string $string     string to translate
     * @param string $fallback   fallback in-case no translation can be found
     * @param bool   $useDefault use default ATK translation if no translation can be found?
     *
     * @return string translation
     */
    protected function text($string, $fallback = '', $useDefault = true)
    {
        return $this->getGrid()->text($string, $fallback, $useDefault);
    }

    /**
     * Renders the component.
     *
     * @return string component HTML
     */
    abstract public function render();
}
