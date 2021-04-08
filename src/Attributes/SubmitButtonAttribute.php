<?php


namespace Sintattica\Atk\Attributes;

class SubmitButtonAttribute extends ButtonAttribute
{
    private $onClickCallback = null;


    public function display($record, $mode)
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
}
