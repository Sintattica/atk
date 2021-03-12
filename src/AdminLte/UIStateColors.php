<?php


namespace Sintattica\Atk\AdminLte;


abstract class UIStateColors
{

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

    /**
     * Returns the hex color for the provided state
     * @param string|null $state
     * @return string
     */
    static function getHex(?string $state): string
    {
        switch ($state) {
            case self::STATE_SECONDARY:
                return self::COLOR_SECONDARY;
            case self::STATE_SUCCESS:
                return self::COLOR_SUCCESS;
            case self::STATE_INFO:
                return self::COLOR_INFO;
            case self::STATE_WARNING:
                return self::COLOR_WARNING;
            case self::STATE_DANGER:
                return self::COLOR_DANGER;
            case self::STATE_DARK:
                return self::COLOR_DARK;
            case self::STATE_LIGHT:
                return self::COLOR_LIGHT;
            case self::STATE_WHITE:
                return self::COLOR_WHITE;
            case self::STATE_INDIGO:
                return self::COLOR_INDIGO;
            case self::STATE_PURPLE:
                return self::COLOR_PURPLE;
            case self::STATE_PINK:
                return self::COLOR_PINK;
            case self::STATE_ORANGE:
                return self::COLOR_ORANGE;
            case self::STATE_TEAL:
                return self::COLOR_TEAL;
            case self::STATE_PRIMARY:
                return self::COLOR_PRIMARY;
            case self::STATE_DEFAULT:
            default:
                return self::COLOR_DEFAULT;
        }
    }

    /**
     * Returns the class of the background color for the provided state
     * @param string $state
     * @return string
     */
    static function getBgClassFromState(string $state): string
    {
        return "bg-" . $state;
    }

    static function getBgStateFromClass($class): ?string
    {
        if (strpos($class, 'bg-') !== 0) {
            return null;
        }

        return str_replace('bg-', '', $class);
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

}
