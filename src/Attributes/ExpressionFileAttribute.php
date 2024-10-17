<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Node;

class ExpressionFileAttribute extends ExpressionAttribute
{
    /** @var FileAttribute $m_fileAttribute useful for search and display */
    protected $m_fileAttribute;

    function __construct($name, $flags, $expression, $fileAttribute = null)
    {
        parent::__construct($name, $flags, $expression);

        $this->m_fileAttribute = $fileAttribute ?: new FileAttribute($this->fieldName());
    }

    function setOwnerInstance(Node $instance): static
    {
        parent::setOwnerInstance($instance);

        $this->m_fileAttribute->setOwnerInstance($this->m_ownerInstance);
        return $this;
    }

    public function showOnlyPreview($value)
    {
        $this->m_fileAttribute->showOnlyPreview($value);
    }

    public function useThumbnail($value)
    {
        $this->m_fileAttribute->useThumbnail($value);
    }

    public function display(array $record, string $mode): string
    {
        $record[$this->fieldName()] = ['filename' => $record[$this->fieldName()]];
        return $this->m_fileAttribute->display($record, $mode);
    }
}