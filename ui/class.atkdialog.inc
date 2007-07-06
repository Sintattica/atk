<?php
/**
 * ATK dialog helper class.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class atkDialog
{
  var $m_nodeType;
  var $m_action;
  var $m_partial;
  var $m_params;

  var $m_title;

  var $m_themeName;
  var $m_themeLoad;

  var $m_width = null;
  var $m_height = null;

  /**
   * Constructor.
   *
   * @param string $nodeType node name
   * @param string $action   node action
   * @param string $partial  partial name
   * @param array $params    url parameters
   *
   * @return atkDialog
   */
  function atkDialog($nodeType, $action, $partial='dialog', $params=array())
  {
    $this->m_nodeType = $nodeType;
    $this->m_action = $action;
    $this->m_partial = $partial;
    $this->m_params = $params;

    atkimport('atk.ui.atkui');
    $ui = &atkUI::getInstance();
    $node = &atkGetNode($this->m_nodeType);
    $this->m_title = $ui->title($node->m_module, $node->m_type, $action);

    $theme = &atkTheme::getInstance();
    $this->m_themeName = $theme->getAttribute('dialog_theme_name', 'alphacube');
    $this->m_themeLoad = $theme->getAttribute('dialog_theme_load', true);
  }

  /**
   * Sets the dialog title.
   *
   * @param string title
   */
  function setTitle($title)
  {
    $this->m_title = $title;
  }

  /**
   * Sets the theme to use.
   *
   * @param string  $name theme name
   * @param boolean $load load theme?
   */
  function setTheme($name, $load=true)
  {
    $this->m_themeName = $name;
    $this->m_themeLoad = $load;
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
   * Load JavaScript and stylesheets.
   */
  function load()
  {
    atkimport('atk.ui.atkpage');

    $page = &atkPage::getInstance();
    $page->register_script(atkconfig('atkroot').'atk/javascript/window/window.js');
    $page->register_script(atkconfig('atkroot').'atk/javascript/class.atkdialog.js');
    $page->register_style(atkconfig('atkroot').'atk/javascript/window/themes/default.css');

    if ($this->m_themeLoad)
    {
      $page->register_style(atkconfig('atkroot').'atk/javascript/window/themes/'.$this->m_themeName.'.css');
    }
  }

  /**
   * Returns the dialog URL.
   *
   * @return string dialog URL
   * @access private
   */
  function getUrl()
  {
    return partial_url($this->m_nodeType, $this->m_action, $this->m_partial, $this->m_params);
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

    if ($this->m_width !== null)
      $options['width'] = $this->m_width;
    if ($this->m_height !== null)
      $options['height'] = $this->m_height;

    return $options;
  }

  /**
   * Returns the JavaScript call to open the dialog.
   *
   * @param boolean $load   load JavaScript and stylesheets needed to show this dialog?
   * @param boolean $encode encode using htmlentities (needed to use in links etc.)
   *
   * @return string JavaScript call for opening the dialog
   */
  function getCall($load=true, $encode=true)
  {
    if ($load)
    {
      $this->load();
    }

    atkimport('atk.utils.atkjson');

    $call = "(new ATK.Dialog(%s, %s, %s, %s)).show();";
    $params = array(
      atkJSON::encode($this->m_title),
      atkJSON::encode($this->getUrl()),
      atkJSON::encode($this->m_themeName),
      atkJSON::encode($this->getOptions())
    );

    $result = vsprintf($call, $params);
    return $encode ? htmlentities($result) : $result;
  }

  /**
   * Returns JavaScript code to save the contents of the current
   * active ATK dialog.
   *
   * @param string $url        save URL
   * @param string $formName   form name (will be serialized)
   * @param array $extraParams key/value array with URL parameters that need to be send
   *                           the parameters will override form element with the same name!
   * @param boolean $encode    encode using htmlentities (needed to use in links etc.)
   *
   * @return string JavaScript call for saving the current dialog
   *
   * @static
   */
  function getSaveCall($url, $formName='dialogform', $extraParams=array(), $encode=true)
  {
    atkimport('atk.utils.atkjson');

    $call = 'ATK.Dialog.getCurrent().save(%s, %s, %s);';
    $params = array(
      atkJSON::encode($url),
      atkJSON::encode($formName),
      count($extraParams) == 0 ? '{}' : atkJSON::encode($extraParams)
    );

    $result = vsprintf($call, $params);
    return $encode ? htmlentities($result) : $result;
  }

  /**
   * Returns JavaScript code to close the current ATK dialog.
   *
   * @return string JavaScript call for closing the current dialog
   *
   * @static
   */
  function getCloseCall()
  {
    return 'ATK.Dialog.getCurrent().close();';
  }

  /**
   * Returns JavaScript code to save the contents of the current
   * active ATK dialog and close it immediately.
   *
   * @param string $url        save URL
   * @param string $formName   form name (will be serialized)
   * @param array $extraParams key/value array with URL parameters that need to be send
   *                           the parameters will override form element with the same name!
   * @param boolean $encode    encode using htmlentities (needed to use in links etc.)
   *
   * @return string JavaScript call for saving the current dialog and closing it immediately
   *
   * @static
   */
  function getSaveAndCloseCall($url, $formName='dialogform', $extraParams=array(), $encode=true)
  {
    atkimport('atk.utils.atkjson');

    $call = 'ATK.Dialog.getCurrent().saveAndClose(%s, %s, %s);';
    $params = array(
      atkJSON::encode($url),
      atkJSON::encode($formName),
      atkJSON::encode($extraParams)
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
  function getUpdateCall($content, $encode=true)
  {
    atkimport('atk.utils.atkjson');

    $call = 'ATK.Dialog.getCurrent().update(%s);';
    $params = array(atkJSON::encode($content));

    $result = vsprintf($call, $params);
    return $encode ? htmlentities($result) : $result;
  }
}