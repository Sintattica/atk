<?php


namespace Sintattica\Atk\AdminLte;


abstract class UIStateColors
{

    /*  --blue: #007bff;
        --indigo: #6610f2;
        --purple: #6f42c1;
        --pink: #e83e8c;
        --red: #dc3545;
        --orange: #fd7e14;
        --yellow: #ffc107;
        --green: #28a745;
        --teal: #20c997;
        --cyan: #17a2b8;
        --white: #fff;
        --gray: #6c757d;
        --gray-dark: #343a40;
        --primary: #007bff;
        --secondary: #6c757d;
        --success: #28a745;
        --info: #17a2b8;
        --warning: #ffc107;
        --danger: #dc3545;
        --light: #f8f9fa;
        --dark: #343a40;
     */

    public const COLOR_DEFAULT = 'default';
    public const COLOR_PRIMARY = 'primary';
    public const COLOR_SECONDARY = 'secondary';
    public const COLOR_SUCCESS = 'success';
    public const COLOR_INFO = 'info';
    public const COLOR_WARNING = 'warning';
    public const COLOR_DANGER = 'danger';
    public const COLOR_DARK = 'dark';
    public const COLOR_LIGHT = 'light';

    static function getHex(string $state): string
    {
        switch ($state) {
            case self::COLOR_SECONDARY:
                return '#6c757d';
            case self::COLOR_SUCCESS:
                return '#28a745';
            case self::COLOR_INFO:
                return '#17a2b8';
            case self::COLOR_WARNING:
                return '#ffc107';
            case self::COLOR_DANGER:
                return '#dc3545';
            case self::COLOR_DARK:
                return '#343a40';
            case self::COLOR_LIGHT:
                return '#f8f9fa';
            case self::COLOR_DEFAULT:
            case self::COLOR_PRIMARY:
            default:
                return '#007bff';
        }
    }
}
