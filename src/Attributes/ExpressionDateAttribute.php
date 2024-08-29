<?php

namespace Sintattica\Atk\Attributes;

/**
 * Class ExpressionDateAttribute
 */
class ExpressionDateAttribute extends ExpressionAttribute
{
    /** @var DateAttribute $m_dateAttribute useful for search and display
     */
    protected $m_dateAttribute;

    function __construct($name, $flags, $expression, $dateAttribute = null)
    {
        parent::__construct($name, $flags, $expression, 'string');

        $this->m_dateAttribute = $dateAttribute ?: new DateAttribute($this->fieldName());
    }

    function setOwnerInstance($instance)
    {
        parent::setOwnerInstance($instance);

        $this->m_dateAttribute->setOwnerInstance($this->m_ownerInstance);
    }

    public function display(array $record, string $mode): string
    {
        if (!$record[$this->fieldName()]) {
            return '';
        }
        if (!is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = DateAttribute::dateArray($record[$this->fieldName()]);
        }
        return $this->m_dateAttribute->display($record, $mode);
    }
}
