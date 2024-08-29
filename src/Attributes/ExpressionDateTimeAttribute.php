<?php

namespace Sintattica\Atk\Attributes;

/**
 * Class ExpressionDateAttribute
 */
class ExpressionDateTimeAttribute extends ExpressionAttribute
{
    /**
     * @var DateTimeAttribute $m_dateTimeAttribute useful for search and display
     */
    protected $m_dateTimeAttribute;

    function __construct($name, $flags, $expression, $dateTimeAttribute = null)
    {
        parent::__construct($name, $flags, $expression, 'string');

        $this->m_dateTimeAttribute = $dateTimeAttribute ?: new DateTimeAttribute($this->fieldName());
    }

    function setOwnerInstance($instance)
    {
        parent::setOwnerInstance($instance);

        $this->m_dateTimeAttribute->setOwnerInstance($this->m_ownerInstance);
    }

    public function display(array $record, string $mode): string
    {
        if (!$record[$this->fieldName()]) {
            return '';
        }

        $value = $record[$this->fieldName()];

        if (!is_array($value)) {
            foreach (['-', ' ', ':'] as $toRemove) {
                $value = str_replace($toRemove, '', $value);
            }

            $record[$this->fieldName()] = DateTimeAttribute::datetimeArray($value);
        }
        return $this->m_dateTimeAttribute->display($record, $mode);
    }
}
