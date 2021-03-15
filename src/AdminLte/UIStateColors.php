<?php


namespace Sintattica\Atk\AdminLte;

use Sintattica\Atk\Core\Tools;

class UIStateColors
{

    /**
     * Todo: Add more colors.
     */

    public const STATE_DEFAULT = 'default';
    public const COLOR_DEFAULT = '#6c757d';

    public const STATE_PRIMARY = 'primary'; //blue
    public const COLOR_PRIMARY = '#007bff';

    public const STATE_SECONDARY = 'secondary'; //gray
    public const COLOR_SECONDARY = '#6c757d';

    public const STATE_SUCCESS = 'success'; //green
    public const COLOR_SUCCESS = '#28a745';

    public const STATE_INFO = 'info'; //cyan
    public const COLOR_INFO = '#17a2b8';

    public const STATE_WARNING = 'warning'; //yellow
    public const COLOR_WARNING = '#ffc107';

    public const STATE_DANGER = 'danger'; //red
    public const COLOR_DANGER = '#dc3545';

    public const STATE_DARK = 'dark'; //dark-gray
    public const COLOR_DARK = '#343a40';

    public const STATE_LIGHT = 'light'; //light-gray
    public const COLOR_LIGHT = '#f8f9fa';

    public const STATE_WHITE = 'white';
    public const COLOR_WHITE = '#ffffff';

    public const STATE_INDIGO = 'indigo';
    public const COLOR_INDIGO = '#6610f2';

    public const STATE_PURPLE = 'purple';
    public const COLOR_PURPLE = '#6f42c1';

    public const STATE_PINK = 'pink';
    public const COLOR_PINK = '#e83e8c';

    public const STATE_ORANGE = 'orange';
    public const COLOR_ORANGE = '#fd7e14';

    public const STATE_TEAL = 'teal';
    public const COLOR_TEAL = '#20c997';

    public const STATE_DISABLED = 'atk-disabled';
    public const COLOR_DISABLED = '#CED4DA';

    public const STATE_LIGHT_GREEN = 'light-green';
    public const COLOR_LIGHT_GREEN = '#94F0A9';

    public const STATE_RED = 'red';
    public const COLOR_RED = '#F13030';


    public const HEX_COLOR = 'hex_color';
    public const BG_CLASS = 'bg_class';
    public const HEX_COLOR_RLIST = 'hex_color_rl';

    //Used to calculate the intensity of the hover effect on the record.
    public const HOVER_RLIST_INTENSITY = 15;

    private $colorPalette = [];

    /**
     * Get new menu object.
     *
     * @return UIStateColors|null class object
     */
    public static function getInstance(): ?self
    {
        static $instance = null;
        if ($instance == null) {
            Tools::atkdebug('Creating UI color states');
            $instance = new static();
            $instance->initColorPalette();
        }

        return $instance;
    }


    private function initColorPalette()
    {
        $this->colorPalette = [
            self::STATE_DEFAULT => [
                self::HEX_COLOR => self::COLOR_DEFAULT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_DEFAULT),
                self::HEX_COLOR_RLIST => self::COLOR_WHITE,
            ],

            self::STATE_PRIMARY => [
                self::HEX_COLOR => self::COLOR_PRIMARY,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_PRIMARY),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_PRIMARY, -30),
            ],
            self::STATE_SECONDARY => [
                self::HEX_COLOR => self::COLOR_SECONDARY,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_SECONDARY),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_SECONDARY, -20),
            ],
            self::STATE_SUCCESS => [
                self::HEX_COLOR => self::COLOR_SUCCESS,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_SUCCESS),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_SUCCESS, -15),
            ],
            self::STATE_INFO => [
                self::HEX_COLOR => self::COLOR_INFO,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_INFO),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_INFO, -40),
            ],
            self::STATE_WARNING => [
                self::HEX_COLOR => self::COLOR_WARNING,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_WARNING),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_WARNING, -30),
            ],
            self::STATE_DANGER => [
                self::HEX_COLOR => self::COLOR_DANGER,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_DANGER),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_DANGER, -30),
            ],
            self::STATE_DARK => [
                self::HEX_COLOR => self::COLOR_DARK,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_DARK),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_DARK, -10),
            ],
            self::STATE_LIGHT => [
                self::HEX_COLOR => self::COLOR_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_LIGHT, 5),
            ],
            self::STATE_WHITE => [
                self::HEX_COLOR => self::COLOR_WHITE,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_WHITE),
                self::HEX_COLOR_RLIST => self::COLOR_WHITE,
            ],
            self::STATE_PURPLE => [
                self::HEX_COLOR => self::COLOR_PURPLE,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_PURPLE),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_PURPLE, 0),
            ],
            self::STATE_INDIGO => [
                self::HEX_COLOR => self::COLOR_INDIGO,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_INDIGO),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_INDIGO, 0),
            ],
            self::STATE_PINK => [
                self::HEX_COLOR => self::COLOR_PINK,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_PINK),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_PINK, 0),
            ],
            self::STATE_ORANGE => [
                self::HEX_COLOR => self::COLOR_ORANGE,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_ORANGE),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_ORANGE, 0),
            ],
            self::STATE_TEAL => [
                self::HEX_COLOR => self::COLOR_TEAL,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_TEAL),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_TEAL, -20),
            ],
            self::STATE_DISABLED => [
                self::HEX_COLOR => self::COLOR_DISABLED,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_DISABLED),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_DISABLED, -10),
            ],

            self::STATE_LIGHT_GREEN => [
                self::HEX_COLOR => self::COLOR_LIGHT_GREEN,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_LIGHT_GREEN),
                self::HEX_COLOR_RLIST => self::COLOR_LIGHT_GREEN,
            ],

            self::STATE_RED => [
                self::HEX_COLOR => self::STATE_RED,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_RED),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_RED, -20),
            ],
        ];
    }


    /**
     * Returns the hex color for the provided state
     * @param string|null $state
     * @return string
     */
    static function getHex(?string $state): ?string
    {

        if (!self::getInstance()->colorPalette[$state]) {
            $state = self::STATE_WHITE;
        }

        return self::getInstance()->colorPalette[$state][self::HEX_COLOR];
    }

    static function getHexRList(?string $state): ?string
    {

        if (!isset(self::getInstance()->colorPalette[$state])) {
            $state = self::STATE_WHITE;
        }

        return self::getInstance()->colorPalette[$state][self::HEX_COLOR_RLIST];
    }

    /**
     * Returns the class of the background color for the provided state
     * @param string $state
     * @return string
     */
    public static function getBgClassFromState(string $state): string
    {
        return "bg-" . $state;
    }

    /**
     * Returns the class of the text color for the provided state
     * @param string $state
     * @return string
     */
    static function getTextClass(string $state): string
    {
        return "text-" . $state;
    }

    /**
     * @param $class
     * @return string|null
     */
    static function getBgStateFromClass(string $class): ?string
    {

        foreach (self::getInstance()->colorPalette as $state => $values) {
            if ($values[self::BG_CLASS] === $class) {
                return $state;
            }
        }

        return null;
    }

    public function getAllUIStates(): array
    {
        return array_keys($this->colorPalette);
    }

    /**
     * Add color on the palette, if existing it gets overwritten.
     * @param string $uiState
     * @param string $hexColor
     * @param string|null $hexColorRList
     */
    public function addColor(string $uiState, string $hexColor, ?string $hexColorRList = null): void
    {
        $this->colorPalette[$uiState] = [
            self::HEX_COLOR => $hexColor,
            self::BG_CLASS => self::getBgClassFromState($uiState),
            self::HEX_COLOR_RLIST => $hexColorRList ?: $hexColor
        ];
    }

}
