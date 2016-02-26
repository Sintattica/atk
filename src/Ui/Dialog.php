<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Utils\JSON;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;

/**
 * ATK dialog helper class.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage ui
 */
class Dialog
{
    protected $m_nodeType;
    protected $m_action;
    protected $m_partial;
    protected $m_params;
    protected $m_sessionStatus = SessionManager::SESSION_NEW;
    protected $m_title;
    protected $m_themeName;
    protected $m_width = null;
    protected $m_height = null;
    protected $m_serializeForm = null;
    protected $m_modifierObject = null;

    /**
     * Constructor.
     *
     * @param string $nodeType node type
     * @param string $action node action
     * @param string $partial partial name
     * @param array $params url parameters
     *
     * @return Dialog
     */
    function __construct($nodeType, $action, $partial = 'dialog', $params = array())
    {
        $this->m_nodeType = $nodeType;
        $this->m_action = $action;
        $this->m_partial = $partial;
        $this->m_params = $params;

        $ui = Ui::getInstance();
        $module = Module::getNodeModule($nodeType);
        $type = Module::getNodeType($nodeType);
        $this->m_title = $ui->title($module, $type, $action);
        $this->m_themeName = 'atkdialog';
    }

    /**
     * Returns the dialog node type.
     *
     * @return string node type
     */
    public function getNodeType()
    {
        return $this->m_nodeType;
    }

    /**
     * Returns the dialog action.
     *
     * @return string action
     */
    public function getAction()
    {
        return $this->m_action;
    }

    /**
     * Returns the dialog partial.
     *
     * @return string partial
     */
    public function getPartial()
    {
        return $this->m_partial;
    }

    /**
     * Returns the dialog sessionStatus.
     *
     * @return int sessionStatus (SessionManager::SESSION_BACK=3, SessionManager::SESSION_DEFAULT=0, SessionManager::SESSION_NESTED=2, SessionManager::SESSION_NEW=1, SessionManager::SESSION_PARTIAL=5, SessionManager::SESSION_REPLACE=4)
     */
    public function getSessionStatus()
    {
        return $this->m_sessionStatus;
    }

    /**
     * Sets the dialog sessionStatus.
     *
     * @param int $sessionStatus (SessionManager::SESSION_BACK=3, SessionManager::SESSION_DEFAULT=0, SessionManager::SESSION_NESTED=2, SessionManager::SESSION_NEW=1, SessionManager::SESSION_PARTIAL=5, SessionManager::SESSION_REPLACE=4)
     */
    public function setSessionStatus($sessionStatus)
    {
        $this->m_sessionStatus = (int)$sessionStatus;
    }

    /**
     * Sets the dialog title.
     *
     * @param string $title
     */
    function setTitle($title)
    {
        $this->m_title = $title;
    }

    /**
     * Reset to auto-size.
     */
    function setAutoSize()
    {
        $this->m_width = null;
        $this->m_height = null;
    }

    /**
     * Set dialog dimensions.
     *
     * @param int $width width
     * @param int $height height
     */
    function setSize($width, $height)
    {
        $this->m_width = $width;
        $this->m_height = $height;
    }

    /**
     * Set width.
     *
     * @param int $width
     */
    public function setWidth($width)
    {
        $this->m_width = $width;
    }

    /**
     * Set height.
     *
     * @param int $height
     */
    public function setHeight($height)
    {
        $this->m_height = $height;
    }

    /**
     * Serialize the form with the given name.
     * Defaults to the entryform.
     *
     * @param string $form form name
     */
    function setSerializeForm($form = 'entryform')
    {
        $this->m_serializeForm = $form;
    }

    /**
     * Returns the modifier object.
     *
     * @return mixed modifier object
     */
    public function getModifierObject()
    {
        return $this->m_modifierObject;
    }

    /**
     * Sets an object which modifyObject method will be called (if exists) just
     * before showing the dialog. This method is allowed to alter the dialog.
     *
     * @see Dialog::getCall
     *
     * @param mixed $object modifier object
     */
    public function setModifierObject($object)
    {
        $this->m_modifierObject = $object;
    }

    /**
     * Load JavaScript and stylesheets.
     */
    function load()
    {
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url') . 'javascript/prototype-ui/window/window.packed.js');
        $page->register_script(Config::getGlobal('assets_url') . 'javascript/prototype-ui-ext.js');
        $page->register_script(Config::getGlobal('assets_url') . 'javascript/class.atkdialog.js');
        $page->register_style(Config::getGlobal('assets_url') . 'javascript/prototype-ui/window/themes/window/window.css');
        $page->register_style(Config::getGlobal('assets_url') . 'javascript/prototype-ui/window/themes/shadow/mac_shadow.css');
    }

    /**
     * Returns the dialog URL.
     *
     * @return string dialog URL
     * @access private
     */
    function getUrl()
    {
        return Tools::partial_url($this->getNodeType(), $this->m_action, $this->m_partial, $this->m_params,
            $this->m_sessionStatus);
    }

    /**
     * Returns the dialog options.
     *
     * @return array dialog options
     * @access private
     */
    function getOptions()
    {
        $options = array();

        if ($this->m_width !== null) {
            $options['width'] = $this->m_width;
        }
        if ($this->m_height !== null) {
            $options['height'] = $this->m_height;
        }
        if ($this->m_serializeForm != null) {
            $options['serializeForm'] = $this->m_serializeForm;
        }

        return $options;
    }

    /**
     * Window options like the show effect etc. These options can be controlled
     * by setting the special theme attribute 'dialog_window_options'. This
     * attribute should contain a JavaScript object with the window options.
     *
     * @return mixed
     */
    protected function getWindowOptions()
    {
        return '{}';
    }

    /**
     * Returns the JavaScript call to open the dialog.
     *
     * @param boolean $load load JavaScript and stylesheets needed to show this dialog?
     * @param boolean $encode encode using htmlentities (needed to use in links etc.)
     * @param boolean $callModifier call node's dialog modifier (modifyDialog method)?
     * @param boolean $lateParamBinding
     *
     * @return string JavaScript call for opening the dialog
     */
    function getCall($load = true, $encode = true, $callModifier = true, $lateParamBinding = false)
    {
        if ($load) {
            $this->load();
        }

        if ($callModifier) {
            $method = 'modifyDialog';
            if ($this->getModifierObject() != null &&
                method_exists($this->getModifierObject(), $method)
            ) {
                $this->getModifierObject()->$method($this);
            }
        }

        $call = "(new ATK.Dialog(%s, %s, " . ($lateParamBinding ? 'params' : '{}') . ", %s, %s, %s)).show();";
        $params = array(
            JSON::encode($this->m_title),
            JSON::encode($this->getUrl()),
            JSON::encode($this->m_themeName),
            count($this->getOptions()) == 0 ? '{}' : JSON::encode($this->getOptions()),
            $this->getWindowOptions()
        );

        $result = vsprintf($call, $params);
        $result = $encode ? htmlentities($result) : $result;
        $result = $lateParamBinding ? "function(params) { $result }" : $result;

        return $result;
    }

    /**
     * Returns JavaScript code to save the contents of the current
     * active ATK dialog.
     *
     * @param string $url save URL
     * @param string $formName form name (will be serialized)
     * @param array $extraParams key/value array with URL parameters that need to be send
     *                           the parameters will override form element with the same name!
     * @param boolean $encode encode using htmlentities (needed to use in links etc.)
     *
     * @return string JavaScript call for saving the current dialog
     */
    public static function getSaveCall($url, $formName = 'dialogform', $extraParams = array(), $encode = true)
    {

        $call = 'ATK.Dialog.getCurrent().save(%s, %s, %s);';
        $params = array(
            JSON::encode($url),
            JSON::encode($formName),
            count($extraParams) == 0 ? '{}' : JSON::encode($extraParams)
        );

        $result = vsprintf($call, $params);
        return $encode ? htmlentities($result) : $result;
    }

    /**
     * Returns JavaScript code to close the current ATK dialog.
     *
     * @return string JavaScript call for closing the current dialog
     */
    public static function getCloseCall()
    {
        return 'ATK.Dialog.getCurrent().close();';
    }

    /**
     * Returns JavaScript code to save the contents of the current
     * active ATK dialog and close it immediately.
     *
     * @param string $url save URL
     * @param string $formName form name (will be serialized)
     * @param array $extraParams key/value array with URL parameters that need to be send
     *                           the parameters will override form element with the same name!
     * @param boolean $encode encode using htmlentities (needed to use in links etc.)
     *
     * @return string JavaScript call for saving the current dialog and closing it immediately
     */
    public static function getSaveAndCloseCall($url, $formName = 'dialogform', $extraParams = array(), $encode = true)
    {

        $call = 'ATK.Dialog.getCurrent().saveAndClose(%s, %s, %s);';
        $params = array(
            JSON::encode($url),
            JSON::encode($formName),
            count($extraParams) == 0 ? '{}' : JSON::encode($extraParams)
        );

        $result = vsprintf($call, $params);
        return $encode ? htmlentities($result) : $result;
    }

    /**
     * Returns JavaScript code to update the contents of the current modal dialog.
     *
     * @param string $content new dialog contents
     * @param boolean $encode encode using htmlentities (needed to use in links etc.)
     *
     * @return string JavaScript call for updating the dialog contents
     */
    public static function getUpdateCall($content, $encode = true)
    {
        $call = 'ATK.Dialog.getCurrent().update(%s);';
        $params = array(JSON::encode($content));

        $result = vsprintf($call, $params);
        return $encode ? htmlentities($result) : $result;
    }

    /**
     * Returns JavaScript code to update the contents of the current modal dialog
     * using an Ajax request to the given URL.
     *
     * @param string $url url for the Ajax request
     *
     * @return string JavaScript call for updating the dialog contents
     */
    public static function getAjaxUpdateCall($url)
    {
        $call = "ATK.Dialog.getCurrent().ajaxUpdate('%s');";
        $result = sprintf($call, addslashes($url));
        return $result;
    }

    /**
     * Returns JavaScript code to reload the contents of the current modal dialog.
     *
     * @return string JavaScript call for reloading the dialog contents
     */
    public static function getReloadCall()
    {
        return "ATK.Dialog.getCurrent().reload();";
    }

}
