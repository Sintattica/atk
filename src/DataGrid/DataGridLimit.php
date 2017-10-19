<?php

namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Core\Config;

/**
 * The data grid limit box. Can be used to render a
 * limit box for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class DataGridLimit extends DataGridComponent
{
    /**
     * Returns a list of possible limit values.
     *
     * @return array list of possible limit values
     */
    protected function getValues()
    {
        $defaultLimit = $this->getGrid()->getDefaultLimit();
        $limit = $this->getGrid()->getLimit();
        //$values = array(5, 10, 15, 20, 25, 30, 40, 50, 100, $defaultLimit, $limit);
        $values = Config::getGlobal('recordsperpage_options', array(5, 10, 15, 20, 25, 30, 40, 50, 100, $defaultLimit, $limit));
        $values = array_diff($values, array(-1));
        $values = array_unique($values);
        sort($values);

        return $values;
    }

    /**
     * Returns the possible options.
     *
     * @param array $values possible limits
     *
     * @return array possible options
     */
    protected function getOptions($values)
    {
        $options = [];

        $limit = $this->getGrid()->getLimit();

        foreach ($values as $value) {
            $current = $value == $limit;
            $options[] = array('title' => $value, 'value' => $value, 'current' => $current);
        }

        //Add 'show all' option.
        //if ($this -> getOption('showAll', false))
        if (Config::getGlobal('enable_showall', false)) {
            $options[] = array(
                'title' => $this->text('all'),
                'value' => -1,
                'current' => $limit == -1,
            );
        }

        return $options;
    }

    /**
     * Renders the limit box for the given data grid.
     *
     * @return string rendered HTML
     */
    public function render()
    {
        $values = $this->getValues();

        if ($this->getGrid()->getCount() <= $values[0] || $this->getGrid()->isEditing()) {
            return '';
        }

        $options = $this->getOptions($values);
        $call = $this->getGrid()->getUpdateCall(array('atkstartat' => 0), array('atklimit' => 'this.value'));
        $result = $this->getUi()->render('dglimit.tpl', array('options' => $options, 'call' => $call, 'label' => $this->text('datagrid_dglimit', '', false)));

        return $result;
    }
}
