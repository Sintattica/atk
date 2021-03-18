<?php

namespace Sintattica\Atk\Attributes;

/**
 * Class StateColorAttribute
 * @package Sintattica\Atk\Attributes
 *
 * Classe utile per mostrare lo stato in maniera grafica (cerchio colorato + testo).
 * È possibile specificare una funzione di callback per determinare il colore in base al record.
 *
 * TODO: integrare con UIStateColors + bootstrap + admin lte
 */
class StateColorAttribute extends DummyAttribute
{
    /**
     * Colori disponibili: red, orange, yellow, blue, green, grey, black.
     *
     * @var string $color
     */
    protected $color;

    /**
     * @var callable Funzione di callback per determinare il colore in base al record.
     */
    protected $colorCondition;

    /**
     * Contenuto testuale mostrato a destra del cerchio colorato.
     *
     * @var string $content
     */
    protected $content;

    /**
     * Modalità di display del contenuto.
     *
     * @var string $displayStyle
     */
    protected $displayStyle;

    public function __construct($name, $flags = 0, $color = '', $content = '')
    {
        parent::__construct($name, $flags | self::AF_READONLY | self::AF_DUMMY_SHOW_LABEL);

        $this->color = $color;
        $this->content = $content;
        $this->displayStyle = 'inline-block';
    }

    public function display($record, $mode)
    {
        if ($this->colorCondition) {
            $this->color = call_user_func($this->colorCondition, $record);
        }

        $display = '<div class="colore-stato ' . ($this->color ?: 'empty') . '"></div>';

        if ($this->content) {
            $display .= '<div style="display: ' . $this->displayStyle . '; margin-left: 4px;">' . $this->content . '</div>';
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
     */
    public function setColor(string $color)
    {
        $this->color = $color;
    }

    /**
     * @return string
     */
    public function getContent(): string
    {
        return $this->content;
    }

    /**
     * @param string $content
     */
    public function setContent(string $content)
    {
        $this->content = $content;
    }

    /**
     * @return string
     */
    public function getDisplayStyle(): string
    {
        return $this->displayStyle;
    }

    /**
     * @param string $displayStyle
     */
    public function setDisplayStyle(string $displayStyle)
    {
        $this->displayStyle = $displayStyle;
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
     */
    public function setColorCondition(callable $colorCondition)
    {
        $this->colorCondition = $colorCondition;
    }
}
