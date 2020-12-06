<?php

namespace Sintattica\Atk\Ui;

use Exception;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use SmartyException;

/**
 * Wrapper class for the Smarty template engine.
 * This class instantiates Smarty and configures it for use in ATK.
 */
class SmartyProvider
{
    /**
     * Get the Smarty instance.
     *
     * @return Smarty The one and only instance.
     * @throws Exception
     */
    public static function getInstance(): ?Smarty
    {
        static $s_smarty = null;
        if ($s_smarty == null) {
            Tools::atkdebug('Creating Smarty instance');

            $tplcompiledir = Config::getGlobal('tplcompiledir');
            if (!is_dir($tplcompiledir) && !mkdir($tplcompiledir, 0755, true)) {
                Tools::atkerror("Unable to create template compile directory: $tplcompiledir");
            }

            $tplcompiledir = realpath($tplcompiledir);

            $s_smarty = new Smarty();
            $s_smarty->setTemplateDir(Config::getGlobal('template_dir')); // name of directory for templates
            $s_smarty->autoload_filters = [];    // indicates which filters will be auto-loaded
            $s_smarty->setCompileDir($tplcompiledir); // name of directory for compiled templates
            $s_smarty->setForceCompile(Config::getGlobal('tplforcecompile')); // force templates to compile every time
            $s_smarty->addPluginsDir([__DIR__.'/smarty-plugins']);

            Tools::atkdebug('Instantiated new Smarty');
        }

        return $s_smarty;
    }

    /**
     * @param string $path
     * @param array $vars
     * @return string
     * @throws SmartyException
     * @throws Exception
     */
    public static function render(string $path, array $vars) : string
    {
        $smarty = self::getInstance();

        // First clear any existing smarty var.
        $smarty->clearAllAssign();

        $smarty->assign($vars);
        return $smarty->fetch($path);
    }
}
