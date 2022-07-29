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
     * Text content (shown in the right side of the color)
     *
     * @var string $content
     */
    protected $textContent;

    private $shape = self::SHAPE_ROUND;
    private $bordered = false;
    private $coloredText = false;
    private $size = self::SIZE_MD;

    public function __construct($name, $flags = 0)
    {
        parent::__construct($name, $flags | self::AF_READONLY | self::AF_DUMMY_SHOW_LABEL);
    }

    public function display($record, $mode)
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
            $borderStyle = 'style="border-color:' . Tools::dimColorBy(UIStateColors::getHex($this->color), 15) . ';"';
        }

        $display = '<span class="' . implode(' ', $shapeClasses) . ' d-inline-block state-color-attribute"' . $borderStyle . '></span>';

        if ($this->textContent) {
            $textClasses = [];
            if ($this->coloredText) {
                $textClasses[] = UIStateColors::getTextClassFromState($this->color);
            }

            $display .= '<span class="ml-2 ' . implode(' ', $textClasses) . '">' . $this->textContent . '</span>';
        }

        return $display;
    }

    /**
     * @return string
     */
    public function getColor(): string
    {
        return $this->color;
    }

    /**
     * @param string $color
     * @return StateColorAttribute
     */
    public function setColor(string $color): self
    {
        $this->color = $color;
        return $this;
    }

    /**
     * @return string
     */
    public function getTextContent(): string
    {
        return $this->textContent;
    }

    /**
     * @param string $content
     * @return StateColorAttribute
     */
    public function setTextContent(string $content): self
    {
        $this->textContent = $content;
        return $this;
    }

    /**
     * @return callable
     */
    public function getColorCondition(): callable
    {
        return $this->colorCondition;
    }

    /**
     * @param callable $colorCondition
     * @return StateColorAttribute
     */
    public function setColorCondition(callable $colorCondition): self
    {
        $this->colorCondition = $colorCondition;
        return $this;
    }

    /**
     * @return string
     */
    public function getShape(): string
    {
        return $this->shape;
    }

    /**
     * @param string $shape
     * @return StateColorAttribute
     */
    public function setShape(string $shape): self
    {
        $this->shape = $shape;
        return $this;
    }

    /**
     * @return bool
     */
    public function getBordered(): bool
    {
        return $this->bordered;
    }

    /**
     * @param bool $bordered
     * @return StateColorAttribute
     */
    public function setBordered(bool $bordered): self
    {
        $this->bordered = $bordered;
        return $this;
    }

    /**
     * @return bool
     */
    public function getColoredText(): bool
    {
        return $this->coloredText;
    }

    /**
     * @param null $coloredText
     * @return StateColorAttribute
     */
    public function setColoredText($coloredText): self
    {
        $this->coloredText = $coloredText;
        return $this;
    }

    /**
     * @return string
     */
    public function getSize(): string
    {
        return $this->size;
    }

    /**
     * @param string $size
     * @return StateColorAttribute
     */
    public function setSize(string $size): self
    {
        $this->size = $size;
        return $this;
    }
}
