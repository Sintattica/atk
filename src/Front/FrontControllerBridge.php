<?php namespace Sintattica\Atk\Front;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;

/**
 * Front-end controller bridge. This bridge can be (re-)implemented to
 * support different front-ends then a stand-alone ATK application.
 *
 * @author Tjeerd Bijlsma <tjeerd@ibuildings.nl>
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage front
 */
class FrontControllerBridge
{

    /**
     * Build url using the given URI and variables.
     *
     * @param array $vars Request vars.
     * @return string url
     */
    public function buildUrl($vars)
    {
        $url = Tools::atkSelf() . '?' . http_build_query($vars);
        return $url;
    }

    /**
     * Redirect to the given url.
     *
     * @param string $url The URL.
     */
    public function doRedirect($url)
    {
        header('Location: ' . $url);
    }

    /**
     * Register stylesheet of the given media type.
     *
     * @param string $file stylesheet filename
     * @param string $media media type (defaults to 'all')
     */
    public function registerStyleSheet($file, $media = 'all')
    {
        Tools::atkinstance('atk.ui.atkpage')->register_style($file, $media);
    }

    /**
     * Register stylesheet code.
     *
     * @param string $code stylesheet code
     */
    public function registerStyleCode($code)
    {
        Tools::atkinstance('atk.ui.atkpage')->register_stylecode($code);
    }

    /**
     * Register script file.
     *
     * @param string $file script filename
     */
    public function registerScriptFile($file)
    {
        Tools::atkinstance('atk.ui.atkpage')->register_script($file);
    }

    /**
     * Register JavaScript code.
     *
     * @param string $code
     */
    public function registerScriptCode($code)
    {
        Tools::atkinstance('atk.ui.atkpage')->register_scriptcode($code);
    }

    /**
     * Get the application root
     *
     * @return string The path to the application root
     */
    public function getApplicationRoot()
    {
        return Config::getGlobal('application_dir');
    }

}
