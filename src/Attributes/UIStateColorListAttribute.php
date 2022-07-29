<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\AdminLte\UIStateColors;

class UIStateColorListAttribute extends ListAttribute
{
    private const SPOT_SUFFIX = '_spot';

    public function __construct($name, $flags = 0)
    {
        $optionArray = UIStateColors::getAllUIStates();

        parent::__construct($name, $flags, $optionArray);
    }

    public function display($record, $mode)
    {
        $display = parent::display($record, $mode);

        if ($display) {
            $stateColor = new StateColorAttribute($this->fieldName() . self::SPOT_SUFFIX);
            if ($record[$this->fieldName()]) {
                $stateColor->setColor($record[$this->fieldName()]);
            }
            $stateColor->setBordered(true);
            $display = '<div>' . $stateColor->display($record, $mode) . '<span class="ml-1">' . $display . '</span></div>';
        }

        return $display;
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $edit = parent::edit($record, $fieldprefix, $mode);

        if ($edit) {
            $stateColor = new StateColorAttribute($this->fieldName() . self::SPOT_SUFFIX);
            if ($record[$this->fieldName()]) {
                $stateColor->setColor($record[$this->fieldName()]);
            }
            $stateColor->setBordered(true);

            $edit = '<div class="row w-100 d-flex no-gutters align-items-center prepend-ui-item">' .
                $stateColor->display($record, $mode) .
                '<span class="ml-1 d-inline-block" style="flex-grow: 2;">' . $edit . '</span>' .
                '</div>';
        }

        return $edit;
    }
}