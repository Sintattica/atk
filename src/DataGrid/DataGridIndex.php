<?php

namespace Sintattica\Atk\DataGrid;

use SmartyException;

/**
 * The data grid index. Can be used to render an alphanumeric index
 * for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class DataGridIndex extends DataGridComponent
{
    /**
     * Returns the available indices.
     *
     * @return array available indices
     */
    protected function getAvailableIndices()
    {
        return $this->getNode()->select()->mode($this->getGrid()->getMode())->getIndices();
    }

    /**
     * Returns an array with index links.
     */
    protected function getLinks(): array
    {
        $grid = $this->getGrid();
        $links = [];

        $chars = $this->getAvailableIndices();
        $current = $grid->getIndex();

        // indices
        foreach ($chars as $char) {
            $title = $char;
            $call = $grid->getUpdateCall(array('atkstartat' => 0, 'atkindex' => "{$char}*"));
            $links[] = array(
                'type' => 'index',
                'title' => $title,
                'call' => $call,
                'current' => "{$char}*" == $current,
            );
        }

        // view all
        if (!empty($current)) {
            $title = $grid->text('view_all');
            $call = $grid->getUpdateCall(array('atkindex' => ''));
            $links[] = array('type' => 'all', 'call' => $call, 'title' => $title);
        }

        return $links;
    }

    /**
     * Renders the index for the given data grid.
     *
     * @return string rendered HTML
     * @throws SmartyException
     */
    public function render(): string
    {
        if ($this->getGrid()->isEditing()) {
            return '';
        }

        $links = $this->getLinks();
        return $this->getUi()->render('dgindex.tpl', array('links' => $links));
    }
}
