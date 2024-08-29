<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\AdminLte\UIStateColors;
use Sintattica\Atk\Core\Tools;

/**
 * TODO: rename in UIStateColor?
 *
 * Class StateColorAttribute
 * @package Sintattica\Atk\Attributes
 *
 * Shows an attribute that creates a colored dot and eventually a text on its right.
 * It's possible to set the colors using a callback function.
 */
class StateColorAttribute extends DummyAttribute
{
    public const SHAPE_ROUND = 'shape-round';
    public const SHAPE_SQUARE = 'shape-square';
    public const SHAPE_FLUID = 'shape-fluid';
    public const SIZE_MD = 'md';
    public const SIZE_SM = 'sm';
    public const SIZE_LG = 'lg';

    /**
     * State from UIStateColors
     *
     * @var UIStateColors $color
     */
    protected $color = UIStateColors::STATE_WHITE;

    /**
     * @var callable Base function to conditionally retrieve the color
     */
    protected $colorCondition;

    /**
     * Text content (shown on the right side of the color)
     *
     * @var string $content
     */
    protected $textContent;

    /**
     * List of css classes of text content
     *
     * @var array $textCssClasses
     */
    private $textCssClasses = [];

    private $shape = self::SHAPE_ROUND;
    private $bordered = false;
    private $coloredText = false;
    private $size = self::SIZE_MD;

    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags | self::AF_READONLY | self::AF_DUMMY_SHOW_LABEL);
    }

    public function display(array $record, string $mode): string
    {
        $shapeClasses = [];

        if ($this->colorCondition) {
            $this->color = call_user_func($this->colorCondition, $record);
        }

        $shapeClasses[] = $this->shape;
        $shapeClasses[] = UIStateColors::getBgClassFromState($this->color);
        $shapeClasses[] = $this->size;

        $borderStyle = '';
        if ($this->bordered) {
            $shapeClasses[] = 'bordered';
            $borderStyle = 'style="border-color:' . UIStateColors::getBorderColor($this->color) . ';"';
        }

        $display = '<span class="' . implode(' ', $shapeClasses) . ' d-inline-block state-color-attribute"' . $borderStyle . '></span>';

        if ($this->textContent) {
            $textClasses = $this->textCssClasses;
            if ($this->coloredText) {
                $textClasses[] = UIStateColors::getTextClassFromState($this->color);
            }

            $display .= '<span class="ml-2 ' . implode(' ', $textClasses) . '">' . $this->textContent . '</span>';
        }

        return $display;
    }

    public function getColor(): string
    {
        return $this->color;
    }

    public function setColor(string $color): self
    {
        $this->color = $color;
        return $this;
    }

    public function getTextContent(): string
    {
        return $this->textContent;
    }

    public function setTextContent(string $content): self
    {
        $this->textContent = $content;
        return $this;
    }

    public function getColorCondition(): callable
    {
        return $this->colorCondition;
    }

    public function setColorCondition(callable $colorCondition): self
    {
        $this->colorCondition = $colorCondition;
        return $this;
    }

    public function getShape(): string
    {
        return $this->shape;
    }

    public function setShape(string $shape): self
    {
        $this->shape = $shape;
        return $this;
    }

    public function getBordered(): bool
    {
        return $this->bordered;
    }

    public function setBordered(bool $bordered): self
    {
        $this->bordered = $bordered;
        return $this;
    }

    public function getColoredText(): bool
    {
        return $this->coloredText;
    }

    public function setColoredText($coloredText): self
    {
        $this->coloredText = $coloredText;
        return $this;
    }

    public function getSize(): string
    {
        return $this->size;
    }

    public function setSize(string $size): self
    {
        $this->size = $size;
        return $this;
    }

    public function getTextCssClasses(): array
    {
        return $this->textCssClasses;
    }

    public function setTextCssClasses(array $textCssClasses): self
    {
        $this->textCssClasses = $textCssClasses;
        return $this;
    }

    public function addTextCssClass(string $textCssClass): self
    {
        $this->textCssClasses[] = $textCssClass;
        return $this;
    }
}
