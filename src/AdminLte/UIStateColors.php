<?php


namespace Sintattica\Atk\AdminLte;

use Sintattica\Atk\Core\Tools;

class UIStateColors
{

    public const STATE_DEFAULT = 'default';
    public const COLOR_DEFAULT = '#6c757d';

    public const STATE_PRIMARY = 'primary'; // blue
    public const COLOR_PRIMARY = '#007bff';

    public const STATE_SECONDARY = 'secondary'; // gray
    public const COLOR_SECONDARY = '#6c757d';

    public const STATE_SUCCESS = 'success'; // green
    public const COLOR_SUCCESS = '#28a745';

    public const STATE_INFO = 'info'; // cyan
    public const COLOR_INFO = '#17a2b8';

    public const STATE_WARNING = 'warning'; // yellow
    public const COLOR_WARNING = '#ffc107';

    public const STATE_DANGER = 'danger'; // red
    public const COLOR_DANGER = '#dc3545';

    public const STATE_DARK = 'dark'; // dark-gray
    public const COLOR_DARK = '#343a40';

    public const STATE_LIGHT = 'light'; // light-gray
    public const COLOR_LIGHT = '#f8f9fa';

    public const STATE_WHITE = 'white';
    public const COLOR_WHITE = '#ffffff';

    public const STATE_INDIGO = 'indigo';
    public const COLOR_INDIGO = '#6610f2';

    public const STATE_PURPLE = 'purple';
    public const COLOR_PURPLE = '#6f42c1';

    public const STATE_PINK = 'pink';
    public const COLOR_PINK = '#e83e8c';

    public const STATE_TEAL = 'teal';
    public const COLOR_TEAL = '#20c997';

    public const STATE_DISABLED = 'atk-disabled';
    public const COLOR_DISABLED = '#ced4da';

    public const STATE_GREEN_LIGHT = 'green-light';
    public const COLOR_GREEN_LIGHT = '#cef3da';

    public const STATE_GREEN_STRONG = 'green-strong';
    public const COLOR_GREEN_STRONG = '#98ddb8';

    public const STATE_BLUE_LIGHT = 'blue-light';
    public const COLOR_BLUE_LIGHT = '#5dbbfe'; // TODO: check

    public const STATE_BLUE_STRONG = 'blue-strong';
    public const COLOR_BLUE_STRONG = '#0177cb'; // TODO: check

    public const STATE_CYAN_LIGHT = 'cyan-light';
    public const COLOR_CYAN_LIGHT = '#caf0f8';

    public const STATE_CYAN_STRONG = 'cyan-strong';
    public const COLOR_CYAN_STRONG = '#5dd2ea';

    public const STATE_RED_LIGHT = 'red-light';
    public const COLOR_RED_LIGHT = '#f0a8ab'; //'#f6cacc';

    public const STATE_RED_STRONG = 'red-strong';
    public const COLOR_RED_STRONG = '#dc2e36';

    public const STATE_YELLOW_LIGHT = 'yellow-light';
    public const COLOR_YELLOW_LIGHT = '#fff5c2';

    public const STATE_YELLOW_STRONG = 'yellow-strong';
    public const COLOR_YELLOW_STRONG = '#ffdd47';

    public const STATE_ORANGE_ULTRA_LIGHT = 'orange-ultra-light';
    public const COLOR_ORANGE_ULTRA_LIGHT = '#ffdc85';

    public const STATE_ORANGE_LIGHT = 'orange-light';
    public const COLOR_ORANGE_LIGHT = '#ffd085';

    public const STATE_ORANGE_STRONG = 'orange-strong';
    public const COLOR_ORANGE_STRONG = '#ff7900';


    public const HEX_COLOR = 'hex_color';
    public const HEX_BORDER_COLOR = 'hex_border_color';
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
            self::STATE_GREEN_LIGHT => [
                self::HEX_COLOR => self::COLOR_GREEN_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_GREEN_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_GREEN_LIGHT, -20),
            ],
            self::STATE_GREEN_STRONG => [
                self::HEX_COLOR => self::COLOR_GREEN_STRONG,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_GREEN_STRONG),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_GREEN_STRONG, -20),
            ],
            self::STATE_BLUE_LIGHT => [
                self::HEX_COLOR => self::COLOR_BLUE_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_BLUE_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_BLUE_LIGHT, -20),
            ],
            self::STATE_BLUE_STRONG => [
                self::HEX_COLOR => self::COLOR_BLUE_STRONG,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_BLUE_STRONG),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_BLUE_STRONG, -20),
            ],
            self::STATE_CYAN_LIGHT => [
                self::HEX_COLOR => self::COLOR_CYAN_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_CYAN_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_CYAN_LIGHT, -20),
            ],
            self::STATE_CYAN_STRONG => [
                self::HEX_COLOR => self::COLOR_CYAN_STRONG,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_CYAN_STRONG),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_CYAN_STRONG, -20),
            ],
            self::STATE_RED_LIGHT => [
                self::HEX_COLOR => self::COLOR_RED_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_RED_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_RED_LIGHT, -20),
            ],
            self::STATE_RED_STRONG => [
                self::HEX_COLOR => self::COLOR_RED_STRONG,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_RED_STRONG),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_RED_STRONG, -20),
            ],
            self::STATE_YELLOW_LIGHT => [
                self::HEX_COLOR => self::COLOR_YELLOW_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_YELLOW_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_YELLOW_LIGHT, -20),
            ],
            self::STATE_ORANGE_ULTRA_LIGHT => [
                self::HEX_COLOR => self::COLOR_ORANGE_ULTRA_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_ORANGE_ULTRA_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_ORANGE_ULTRA_LIGHT, -20),
            ],
            self::STATE_YELLOW_STRONG => [
                self::HEX_COLOR => self::COLOR_YELLOW_STRONG,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_YELLOW_STRONG),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_YELLOW_STRONG, -20),
            ],
            self::STATE_ORANGE_LIGHT => [
                self::HEX_COLOR => self::COLOR_ORANGE_LIGHT,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_ORANGE_LIGHT),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_ORANGE_LIGHT, -20),
            ],
            self::STATE_ORANGE_STRONG => [
                self::HEX_COLOR => self::COLOR_ORANGE_STRONG,
                self::BG_CLASS => self::getBgClassFromState(self::STATE_ORANGE_STRONG),
                self::HEX_COLOR_RLIST => Tools::dimColorBy(self::COLOR_ORANGE_STRONG, -20),
            ],
        ];

        foreach ($this->colorPalette as $uiStateColor => &$values) {
            $values[self::HEX_BORDER_COLOR] = self::getBorderColor($uiStateColor);
        }
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
    static function getTextClassFromState(string $state): string
    {
        return "text-" . $state;
    }

    /**
     * @param string $class
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

    public static function getAllUIStates(): array
    {
        return array_keys(self::getInstance()->colorPalette);
    }

    public static function getBorderColor(?string $uiStateColor): string
    {
        if (!$uiStateColor) {
            $uiStateColor = self::STATE_WHITE;
        }
        return Tools::dimColorBy(UIStateColors::getHex($uiStateColor), 15);
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

    public static function getColorPalette(): array
    {
        return self::getInstance()->colorPalette;
    }

}
