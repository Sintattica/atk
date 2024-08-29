<?php


namespace Sintattica\Atk\Attributes;

class SubmitButtonAttribute extends ButtonAttribute
{
    private $onClickCallback = null;
    private $doUpdate = true;

    public function display(array $record, string $mode): string
    {
        $classes = implode(' ', $this->m_cssclasses);
        $tranlatedText = $this->text($this->m_text);

        return '<button type="submit" class="' . $classes . '" name="' . $this->m_name . '" value="' . $tranlatedText . '">' . $tranlatedText . '</button>';

    }

    public function getType(): string
    {
        return self::TYPE_SUBMIT;
    }

    public function getOnClickCallback(): callable
    {
        return $this->onClickCallback;
    }

    public function onClick(callable $clickCallback): self
    {
        $this->onClickCallback = $clickCallback;
        return $this;
    }

    public function isDoUpdate(): bool
    {
        return $this->doUpdate;
    }

    public function setDoUpdate(bool $doUpdate): self
    {
        $this->doUpdate = $doUpdate;
        return $this;
    }
}
