<?php namespace Sintattica\Atk\Keyboard;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Core\Config;

/**
 * This class handles keyboard navigation. It is used to register keyboard
 * event handlers. This class is a singleton. Use getInstance() to retrieve
 * the instance.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage keyboard
 *
 */
class Keyboard
{
    /**
     * Define some key shortcut constants. (mind you, these are not actual ascii key values)
     */
    const KB_UP = 1;
    const KB_DOWN = 2;
    const KB_LEFT = 4;
    const KB_RIGHT = 8;
    const KB_UPDOWN = 3;
    const KB_LEFTRIGHT = 12;
    const KB_CURSOR = 15;
    const KB_CTRLCURSOR = 16;

    /**
     * WORKAROUND: in php (4.3.1 at least) at least one member var must exist, to make it possible to create singletons.
     * @access private
     */
    var $m_dummy = "";

    /**
     * Get the one and only (singleton) instance of the atkKeyboard class.
     * @return Keyboard The singleton instance.
     */
    public static function &getInstance()
    {
        static $s_kb;
        if ($s_kb == null) {
            Tools::atkdebug("Creating atkKeyboard instance");
            $s_kb = new self();
        }

        return $s_kb;
    }

    /**
     * Make a form element keyboard aware. Once added with this function, the
     * element will automatically respond to cursor key navigation.
     *
     * @param string $id The HTML id of the form element for which keyboard
     *                   navigation is added.
     * @param int $navkeys A bitwise mask indicating which keys should be
     *                     supported for this element. Some elements, like
     *                     for example textarea's, use some cursor movements
     *                     for their own navigation. In this case, pass a
     *                     mask that uses different keys.
     */
    function addFormElementHandler($id, $navkeys)
    {
        $params = array(
            "'" . $id . "'",
            Tools::hasFlag($navkeys, self::KB_UP) ? "1" : "0",
            Tools::hasFlag($navkeys, self::KB_DOWN) ? "1" : "0",
            Tools::hasFlag($navkeys, self::KB_LEFT) ? "1" : "0",
            Tools::hasFlag($navkeys, self::KB_RIGHT) ? "1" : "0",
            Tools::hasFlag($navkeys, self::KB_CTRLCURSOR) ? "1" : "0"
        );

        $this->addHandler("atkFEKeyListener", $params);
    }

    /**
     * Make a recordlist keyboard aware. Once added with this function, the
     * recordlist will automatically respond to keyboard navigation events.
     *
     * @param string $id The unique id of the recordlist.
     * @param string $highlight The color used to highlight rows that are
     *                          selected with cursorkeys.
     * @param string $reccount The number of records in the list. The
     *                         handler needs this to be able to determine
     *                         when it's at the end of the list, so it
     *                         can wrap around when the cursor is moved
     *                         beyond the end.
     */
    function addRecordListHandler($id, $highlight, $reccount)
    {
        // does not know the highlight color of each row.
        $params = array("'" . $id . "'", "'" . $highlight . "'", $reccount);
        $this->addHandler("atkRLKeyListener", $params);

        // atkrlkeylistener must be loaded after the main addHandler, which loads keyboardhandler.
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal("assets_url") . "keyboard/javascript/class.atkrlkeylistener.js");
    }

    /**
     * Register a generic keyboard handler. This method is used internally
     * by other atkKeyboard members, but can also be used to add a custom
     * keyboard handler to the page.
     *
     * @param string $handlertype The name of the javascript class used for
     *                            keyboard traps.
     * @param array $params Any param you may want to pass to the handler.
     *                      The params you need to pass depend completely on
     *                      the handler used. See the handlers' documentation
     *                      on params needed.
     */
    function addHandler($handlertype, $params)
    {
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal("assets_url") . "keyboard/javascript/keyboardhandler.js");
        $page->register_loadscript("kb_init();\n");
        $page->register_loadscript("kb_addListener(new $handlertype(" . implode(",", $params) . "));");
    }

}

