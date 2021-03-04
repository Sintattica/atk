<?php


namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;

final class Footer
{
    private $template = 'footer.tpl';
    private $tplVars = [];

    private static $footerInstance = null;


    public static function getInstance(): self
    {
        if (self::$footerInstance == null) {
            self::$footerInstance = new self();
            Tools::atkdebug('Created a new Footer instance');
        }

        return self::$footerInstance;
    }

    /**
     * @return string
     */
    public function getTemplate(): string
    {
        return $this->template;
    }

    /**
     * @param string $template
     */
    public function setTemplate(string $template)
    {
        $this->template = $template;
    }

    /**
     * @return array
     */
    public function getTplVars(): array
    {
        return $this->tplVars;
    }

    /**
     * @param array $tplVars
     */
    public function setTplVars(array $tplVars)
    {
        $this->tplVars = $tplVars;

    }


    public function render(): string
    {

        if (!isset($tplVars['footer_left'])) {
            $tplVars['footer_left'] = Tools::atktext('footer_left', null, null, null, null, true)
                ?: Tools::atktext('app_title');
        }

        if (!isset($tplVars['footer_right'])) {
            $tplVars['footer_right'] = Tools::atktext('footer_right', null, null, null, null, true) ?: Config::getGlobal('version');
        }

        return SmartyProvider::render($this->template, $tplVars);
    }

}
