<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage front
   *
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Imports
   * @access private
   */
  atkimport('atk.utils.atkdataholder');

  /**
   * FrontEnd Controller base class.
   *
   * @author Martin Roest <martin@ibuildings.nl>
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage front
   */
  class atkFrontController implements ArrayAccess
  {
    private static $s_current = null;
    private static $s_bridges = array();

    /**
     * @var atkFrontControllerBridge
     */
    protected $m_bridge;

    protected $m_module;
    protected $m_name;
    protected $m_action;
    protected $m_parent;

    protected $m_request;
    protected $m_session;

    protected $m_vars = array();
    protected $m_template = "";

    protected $m_plugins = NULL;

    protected $m_partial = false;
    protected $m_result = "";
    protected $m_rendered = false;

    protected $m_headers = array();
    protected $m_contentType;

    protected $m_cacheableActions = array();

    protected $m_styleSheets = array();
    protected $m_styleCodes = array();

    protected $m_scriptFiles = array();
    protected $m_scriptCodes = array();

    /**
     * Dispatch request. Find the right controller, let it handle the request
     * and return it's result.
     *
     * @param string $uri controller request uri
     * @param array $vars custom variables, will be merged with the request variables
     *                    and overwrite existing (request) variables with the same name
     *                    unless $merge is set to false
     * @param bool $merge merge custom and request variables?
     *
     * @return string controller result
     */
    public static function dispatchRequest($vars=array(), $merge=true)
    {
      $parent = self::getCurrent();

      // If this is the root controller we first look if a request variable named URI
      // is set, if so we use this URI, else we fallback to the custom (default) URI. If
      // this is a nested controller, we should always respect the custom given URI.
      $uri = $parent == null && isset($_REQUEST['uri']) ? $_REQUEST['uri'] : $vars['uri'];

      if ($merge)
      {
        if (is_object($parent))
        {
          $request = $parent->getRequest()->toArray();
        }
        else
        {
          $request = $_REQUEST;
        }

        $vars = array_merge($request, $vars);
      }

      // Always set the URI to the real URI.
      $vars['uri'] = $uri;

      $controller = self::create($uri, $parent);
      self::setCurrent($controller);
      $result = $controller->handleRequest($vars);
      self::setCurrent($parent);

      return $result;
    }

    /**
     * Create controller based on the given URI.
     *
     * @param string $uri uri string
     * @param atkFrontController $parent parent controller
     * @return atkFrontController the controller
     */
    protected static function create($uri, $parent=NULL)
    {
      if ($uri{0} == '/')
        $uri = substr($uri, 1);

      list($module, $name, $action) = explode('/', $uri);

      if ($action == NULL)
        $action = 'index';

      $importPath = "module.{$module}.controllers.{$name}controller";
      if (atkimport($importPath))
      {
        atkdebug('Create '.$importPath.' controller instance');
        $class = "{$name}controller";
        return new $class($module, $name, $action, $parent);
      }
      else
      {
        throw new Exception("Can't find controller for: '$uri' expecting '$importpath'");
      }
    }

    /**
     * Returns the current controller instance. This can either be the
     * root or a nested controller.
     *
     * @return atkFrontController current controller instance
     */
    protected static function getCurrent()
    {
      return self::$s_current;
    }

    /**
     * Sets the current controller instance.
     *
     * @param atkFrontController $controller current controller instance
     */
    protected static function setCurrent($controller)
    {
      self::$s_current = $controller;
    }

    /**
     * Bridge factory method. Returns the currently configured controller bridge.
     * If an instance already exists, re-uses it, else a new bridge will be created.
     *
     * @return atkFrontControllerBridge front controller bridge
     */
    protected static function createBridge()
    {
      $class = atkconfig("frontcontroller_bridge", "atk.front.atkfrontcontrollerbridge");

      if (!isset(self::$s_bridges[$class]))
      {
        atkdebug('Create '.$class.' bridge instance');
        self::$s_bridges[$class] = atknew($class);
      }

      return self::$s_bridges[$class];
    }

    /**
     * Constructor.
     *
     * @param string $module module name
     * @param string $name   controller name
     */
    protected function __construct($module, $name, $action, $parent = NULL)
    {
      $this->m_module = $module;
      $this->m_name = $name;
      $this->m_action = $action;
      $this->m_parent = $parent;

      if (isset($this->cacheableActions))
      {
        $this->m_cacheableActions = $this->cacheableActions;
      }

      if ($parent !== null)
      {
        $this->m_bridge = $parent->m_bridge;
      }
      else
      {
        $this->m_bridge = self::createBridge();
      }
    }

    /**
     * Returns the parent controller.
     *
     * @return atkFrontController parent
     */
    protected function getParent()
    {
      return $this->m_parent;
    }

    /**
     * Is this the root controller?
     *
     * @return boolean is root controller?
     */
    protected function isRoot()
    {
      return $this->getParent() == NULL;
    }

    /**
     * Returns the root controller.
     *
     * @return atkFrontController root
     */
    protected function getRoot()
    {
      if ($this->isRoot()) return $this;
      else return $this->getParent()->getRoot();
    }

    /**
     * Returns the request parameters.
     *
     * @return atkDataHolder request parameters
     */
    protected function getRequest()
    {
      return $this->m_request;
    }

    /**
     * Returns the session.
     *
     * @return atkDataHolder session
     */
    protected function getSession()
    {
      return $this->m_session;
    }

    /**
     * Returns the controller name.
     *
     * @return string controller name
     */
    protected function getName()
    {
      return $this->m_name;
    }

    /**
     * Returns the controller module.
     *
     * @return string controller module
     */
    protected function getModule()
    {
      return $this->m_module;
    }

    /**
     * Returns the action.
     *
     * @return string action
     */
    protected function getAction()
    {
      return $this->m_action;
    }

    /**
     * Add response header. (Will only be outputted when
     * renderPartial or renderContent is called for the
     * root controller).
     */
    protected function addHeader($header)
    {
      $this->m_headers[] = $header;
    }

    /**
     * Sets the content type (Will only be outputted when
     * renderPartial or renderContent is called for the
     * root controller).
     *
     * @param string $contentType
     */
    protected function setContentType($contentType)
    {
      $this->m_contentType = $contentType;
    }

    /**
     * Sets the cacheable actions.
     *
     * @param array|string $actions cacheable actions
     * @param ...
     */
    protected function setCacheableActions($actions)
    {
      if (!is_array($actions))
        $actions = func_get_args();
      $this->m_cacheableActions = $actions;
    }

    /**
     * Get action cache key.
     */
    protected function getActionCacheKey()
    {
      return '';
    }

    /**
     * Get action cache file.
     *
     * @return atkTmpFile
     */
    private function getActionCacheFile()
    {
      atkimport('atk.utils.atktmpfile');
      $path = 'frontcontrollers/'.$this->m_module.'/'.$this->m_name.'/'.$this->m_action;
      $filename = md5($this->getActionCacheKey()).'.cache';
      $file = new atkTmpFile($path.'/'.$filename);
      return $file;
    }

    /**
     * Is this action cached?
     */
    private function isActionCached()
    {
      // action is never cached
      if (!in_array($this->m_action, $this->m_cacheableActions)) return false;

      $file = $this->getActionCacheFile();
      return $file->exists();
    }

    /**
     * Load from action cache.
     */
    private function loadFromActionCache()
    {
      $file = $this->getActionCacheFile();

      include($file->getPath());

      $this->m_result      = $data['result'];
      $this->m_headers     = $data['headers'];
      $this->m_styleSheets = $data['styleSheets'];
      $this->m_styleCodes  = $data['styleCodes'];
      $this->m_scriptFiles = $data['scriptFiles'];
      $this->m_scriptCodes = $data['scriptCodes'];
    }

    /**
     * Store current action results in cache.
     */
    private function storeInActionCache()
    {
      // action is never cached
      if (!in_array($this->m_action, $this->m_cacheableActions)) return;

      $data = array(
        'result'      => $this->m_result,
        'headers'     => $this->m_headers,
        'styleSheets' => $this->m_styleSheets,
        'styleCodes'  => $this->m_styleCodes,
        'scriptFiles' => $this->m_scriptFiles,
        'scriptCodes' => $this->m_scriptCodes
      );

      $file = $this->getActionCacheFile();
      $file->writeAsPhp('data', $data);
      $file->close();
    }

    /**
     * Register styles and scripts using the bridge.
     */
    private function registerStylesAndScripts()
    {
      foreach ($this->m_styleSheets as $entry)
        $this->m_bridge->registerStyleSheet($entry['file'], $entry['media']);
      foreach ($this->m_styleCodes as $entry)
        $this->m_bridge->registerStyleCode($entry['code']);
      foreach ($this->m_scriptFiles as $entry)
        $this->m_bridge->registerScriptFile($entry['file']);
      foreach ($this->m_scriptCodes as $entry)
        $this->m_bridge->registerScriptCode($entry['code']);
    }

    /**
    * Handle action.
    *
    * @param array $request request variables
    *
    * @return result of action
    */
    protected function handleRequest($request)
    {
      try
      {
        $this->setTemplate($this->m_action);

        $this->m_request = new atkDataHolder($request);
        $this->m_session = $this->loadSession();

        $this->module =   $this->m_module;
        $this->name =     $this->m_name;
        $this->action =   $this->m_action;
        $this->request =  $this->m_request;
        $this->session =  $this->m_session;

        $this->init();

        $this->preFilter();

        if ($this->isActionCached())
        {
          $this->loadFromActionCache();
        }
        else
        {
          $this->installPlugins();

          try
          {
            $method = $this->getActionMethod();
          }
          catch (Exception $ex)
          {
            throw new Exception("Invalid action {$this->m_action} for controller {$this->m_module}/{$this->m_name}! (".$ex->getMessage().")");
          }

          $method->invoke($this);

          $this->render();

          $this->storeInActionCache();

          $this->uninstallPlugins();
        }

        // register styles and scripts in bridge
        $this->registerStylesAndScripts();

        $this->postFilter();

        if ($this->m_partial && $this->isRoot())
        {
          while (@ob_end_clean());

          foreach ($this->m_headers as $header)
            header($header);

          if (isset($this->m_contentType))
            header('Content-Type: '.$this->m_contentType);

          echo $this->m_result;

          die;
        }
      }
      catch (Exception $ex)
      {
        $this->uninstallPlugins();
        $this->handleException($ex);
      }

      return $this->m_result;
    }

    /**
     * Get the method to call for the current request.
     *
     * @return ReflectionMethod The method.
     */
    protected function getActionMethod()
    {
      $methodName = str_replace('_', '', $this->m_action).'Action';
      if (!method_exists($this, $methodName))
        throw new Exception("Action method not found!");

      $method = new ReflectionMethod(get_class($this), $methodName);
      if (!$method->isPublic())
        throw new Exception("Action method is not public!");

      return $method;
    }

    /**
     * Will be called just before the request is dispatched
     * to the right method.
     */
    protected function preFilter()
    {}

    /**
     * Init will be called just after preFilter and just before
     * the request is dispatched to the right method.
     */
    protected function init()
    {}

    /**
     * Will be called just after the request has been dispatched
     * to the right method and the rendering took place but before
     * the result is returned or outputted.
     */
    protected function postFilter()
    {}

    /**
     * Handle exception.
     *
     * @param $exception exception
     */
    protected function handleException($exception)
    {
      if ($this->isRoot())
      {
        atkerror($exception->__toString());
        $this->renderContent('An unknown error occured.');
      }
      else
      {
        throw $exception;
      }
    }

    /**
     * Load session.
     */
    protected function loadSession()
    {
      return new atkDataHolder($_SESSION);
    }

    /**
     * Install smarty plug-ins.
     */
    protected function installPlugins()
    {
      /* @var $smarty Smarty */
      $smarty = atkinstance("atk.ui.atksmarty");
      $this->m_plugins = $smarty->_plugins;
      $smarty->register_function('_partial', array($this, 'partialFunctionTag'), false);
      $smarty->register_function('_url', array($this, 'urlFunctionTag'), false);
      $smarty->register_function('_form_vars', array($this, 'formVarsFunctionTag'), false);
      $smarty->register_block('_link', array($this, 'linkBlockTag'), false);
    }

    /**
     * Uninstall smarty plug-ins.
     */
    protected function uninstallPlugins()
    {
      if ($this->m_plugins == NULL) return;
      $smarty = atkinstance("atk.ui.atksmarty");
      $smarty->_plugins = $this->m_plugins;
      $this->m_plugins = NULL;
    }

    /**
     * Get current template.
     *
     * @return string $template template name
     */
    protected function getTemplate()
    {
      return $this->m_template;
    }

    /**
     * Set current template.
     *
     * @param string $template template name
     */
    protected function setTemplate($template)
    {
      $this->m_template = $template;
    }

    /**
     * Get assigned template variables.
     *
     * @return array template variables
     */
    protected function getVars()
    {
      return $this->m_vars;
    }

    /**
     * Is template variable set?
     */
    public function __isset($name)
    {
      return isset($this->m_vars[$name]);
    }

    /**
     * Template variable assignment.
     *
     * @param string $name variable name
     * @param unknown $value variable value
     */
    public function __set($name, $value)
    {
      $this->m_vars[$name] = $value;
    }

    /**
     * Get template variable value.
     *
     * @param string $name variable name
     * @return unknown
     */
    public function &__get($name)
    {
      return $this->m_vars[$name];
    }

    /**
     * Checks if a certain offset exists.
     *
     * @param mixed $offset offset
     * @return bool offset exists?
     */
    public function offsetExists($offset)
    {
      return isset($this->m_vars[$offset]);
    }

    /**
     * Returns the value at the given offset.
     *
     * @param mixed $offset
     * @return mixed value
     */
    public function offsetGet($offset)
    {
      return $this->m_vars[$offset];
    }

    /**
     * Sets the given offset with the given value
     *
     * @param mixed $offset offset
     * @param mixed $value  value
     */
    public function offsetSet($offset, $value)
    {
      $this->m_vars[$offset] = $value;
    }

    /**
     * Unsets the given offset.
     *
     * @param mixed $offset offset
     */
    public function offsetUnset($offset)
    {
      unset($this->m_vars[$offset]);
    }

    /**
     * Build the URI for the given controller and action.
     *
     * @param string $controller controller name (uses current if empty)
     * @param string $action action name (uses current if empty and controller isn't changed, uses index if empty and other controller)
     * @return string uri
     */
    protected function uri($controller, $action)
    {
      if ($this->isRoot())
      {
        $controller = str_replace('.', '/', $controller);

        if ($controller != NULL && strpos($controller, '/') === FALSE)
        {
          $controller = "{$this->m_module}/{$controller}";
        }

        if ($controller == NULL || $controller == "{$this->m_module}/{$this->m_name}")
        {
          $controller = "{$this->m_module}/{$this->m_name}";

          if ($action == NULL)
            $action = $this->m_action;
        }
        else if ($action == NULL)
        {
          $action = 'index';
        }

        return '/'.$controller.'/'.$action;
      }
      else
      {
        return $this->getParent()->uri($controller, $action);
      }
    }

    /**
     * Build url using the given URI and variables.
     *
     * @param string $uri The controller URI.
     * @param array $vars Request vars.
     * @return string url
     */
    protected function buildUrl($uri, $vars)
    {
      return $this->m_bridge->buildUrl($uri, $vars);
    }

    /**
     * Build url or return the current url.
     *
     * @param string $controller controller name (uses current if empty)
     * @param string $action action name (uses current if empty and controller isn't changed, uses index if empty and other controller)
     * @param array $vars request vars
     * @return string url
     */
    protected function url($controller=NULL, $action=NULL, $vars=array())
    {
      $uri = $this->uri($controller, $action);
      return $this->buildUrl($uri, $vars);
    }

    /**
     * Smarty wrapper function for the url function.
     *
     * @param array $params parameters
     * @param Smarty $smarty smarty reference
     * @return string url
     */
    public function urlFunctionTag($params, $smarty)
    {
      $controller = isset($params['controller']) ? $params['controller'] : NULL;
      $action = isset($params['action']) ? $params['action'] : NULL;
      $vars = isset($params['vars']) ? $params['vars'] : array();

      unset($params['controller']);
      unset($params['action']);
      unset($params['vars']);

      $vars = array_merge($vars, $params);

      return $this->url($controller, $action, $vars);
    }

    /**
     * Smarty function for converting the controller, action etc.
     * to hidden form vars.
     *
     * @param array $params parameters
     * @param Smarty $smarty smarty reference
     * @return string hidden form vars html
     */
    public function formVarsFunctionTag($params, $smarty)
    {
      $controller = isset($params['controller']) ? $params['controller'] : NULL;
      $action = isset($params['action']) ? $params['action'] : NULL;
      $vars = isset($params['vars']) ? $params['vars'] : array();

      unset($params['controller']);
      unset($params['action']);
      unset($params['vars']);

      $uri = $this->uri($controller, $action);
      $vars = array_merge($vars, $params, array('uri' => $uri));

      $result = '';
      foreach ($vars as $name => $value)
      {
        $value = htmlentities($value);
        $result .= "<input type=\"hidden\" name=\"{$name}\" value=\"{$value}\">\n";
      }

      return $result;
    }

    /**
     * Smarty wrapper function for the url function.
     *
     * @param array $params parameters
     * @param string $content content between link tags
     * @param Smarty $smarty smarty reference
     * @param boolean $repeat true first time
     */
    public function linkBlockTag($params, $content, $smarty, &$repeat)
    {
       if (!$repeat)
      {
        return '<a href="'.$this->urlFunctionTag($params, $smarty).'">'.$content.'</a>';
      }

      return NULL;
    }

    /**
     * Smarty partial function, render partial template.
     *
     * @param $params parameters
     * @param Smarty $smarty smarty instance
     * @return string rendered partial
     */
    public function partialFunctionTag($params, $smarty)
    {
      $template = $params[0];
      unset($params[0]);
      return $this->_render($template, $params, true);
    }

    /**
     * Redirect to the given url.
     *
     * @param string $url The URL.
     */
    protected function doRedirect($url)
    {
      $this->m_bridge->doRedirect($url);
    }

    /**
     * Redirect to the given url.
     *
     * @param string $controller controller name (uses current if empty)
     * @param string $action action name (uses current if empty and controller isn't changed, uses index if empty and other controller)
     * @param array $vars request vars
     * @return string url
     */
    protected function redirect($controller=NULL, $action=NULL, $vars=array(), $force=FALSE)
    {
      if ($this->isRoot() || $force)
      {
        $this->doRedirect($this->url($controller, $action, $vars));
      }
      else
      {
        $vars['uri'] = $this->uri($controller, $action);
        $content = self::dispatchRequest($vars, false);
        $this->renderContent($content);
      }
    }

    /**
    * Set locale based on the given language.
    *
    * @param string $lng language (ISO code)
    */
    protected function setLocale($lng)
    {
      switch (strtoupper($lng))
      {
        case "EN":
          setlocale(LC_ALL, "en_EN");
          break;
        case "DE":
          setlocale(LC_ALL, "de_DE");
          break;
        case "FR":
          setlocale(LC_ALL, "fr_FR");
          break;
        default:
          setlocale(LC_ALL, "nl_NL");
      }
    }

    /**
     * Register stylesheet of the given media type.
     *
     * @param string $file stylesheet filename
     * @param string $media media type (defaults to 'all')
     */
    protected function registerStyleSheet($file, $media='all')
    {
      $this->m_styleSheets[] = array('file' => $file, 'media' => $media);
    }

    /**
     * Register stylesheet code.
     *
     * @param string $code stylesheet code
     */
    protected function registerStyleCode($code)
    {
      $this->m_styleCodes[] = array('code' => $code);
    }

    /**
     * Register script file.
     *
     * @param string $file script filename
     */
    protected function registerScriptFile($file)
    {
      $this->m_scriptFiles[] = array('file' => $file);
    }

    /**
     * Register JavaScript code.
     *
     * @param string $code
     */
    protected function registerScriptCode($code)
    {
      $this->m_scriptCodes[] = array('code' => $code);
    }

    /**
     * Render partial.
     *
     * Will assign the partial and result variables and will prevent
     * double rendering!
     *
     * @param string $template template name
     * @param array $vars template variables
     */
    protected function renderPartial($template=NULL, $vars=NULL)
    {
      $this->render($template, $vars, true);
    }

    /**
     * Render content.
     *
     * Will assign the partial and result variables and will prevent
     * double rendering!
     *
     * @param string $content content to render
     */
    protected function renderContent($content)
    {
      $this->m_partial = true;
      $this->m_result = $content;
      $this->m_rendered = true;
    }

    /**
     * Render template.
     *
     * Will assign the partial and result variables and will prevent
     * double rendering!
     *
     * @param string $template template name
     * @param array $vars template variables
     * @param boolean $partial render partial?
     */
    protected function render($template=NULL, $vars=NULL, $partial=false)
    {
      if ($this->m_rendered) return;

      if ($template == NULL)
        $template = $this->getTemplate();

      if ($vars == NULL)
        $vars = $this->getVars();

      if (!$partial && file_exists(moduleDir($this->m_module)."scripts/{$this->m_name}.js"))
      {
        $this->registerScriptFile(moduleDir($this->m_module)."scripts/{$this->m_name}.js");
      }

      if (!$partial && file_exists(moduleDir($this->m_module)."styles/{$this->m_name}.js"))
      {
        $this->registerStyleSheet(moduleDir($this->m_module)."styles/{$this->m_name}.js");
      }

      $this->m_partial = $partial;
      $this->m_result = $this->_render($template, $vars, $partial);
      $this->m_rendered = true;
    }

    /**
     * Returns the template path for the given template.
     *
     * @param string $template  template name
     * @param string $directory the base template directory
     *
     * @return string template path
     */
    protected function getTemplatePath($template, $directory='templates')
    {
      if (file_exists(atkconfig('atkroot')."$directory/$template"))
      {
        return atkconfig('atkroot')."$directory/$template";
      }
      else if (file_exists(moduleDir($this->m_module)."$directory/$template"))
      {
        return moduleDir($this->m_module)."$directory/$template";
      }
      else if (file_exists(moduleDir($this->m_module)."skel/$directory/$template"))
      {
        return moduleDir($this->m_module)."skel/$directory/$template";
      }
      else
      {
        return null;
      }
    }

    /**
     * Returns the layout template path for this controller.
     */
    protected function getLayoutTemplatePath()
    {
      $path = $this->getTemplatePath($this->m_module.'/'.$this->m_name.'.tpl', 'layouts');
      if ($path != null) return $path;

      $path = $this->getTemplatePath($this->m_module.'/root.tpl', 'layouts');
      if ($path != null) return $path;

      $path = $this->getTemplatePath('root.tpl', 'layouts');
      if ($path != null) return $path;

      return $null;
    }

    /**
     * Render template.
     *
     * @param string $template template name
     * @param array $vars template variables
     * @param boolean $partial render partial?
     *
     * @return string rendered template
     */
    protected function _render($template, $vars, $partial)
    {
      if ($template{0} != '/')
      {
        $template = $this->m_module.'/'.$this->m_name.'/'.($partial ? '_' : '').$template;
      }
      else
      {
        $parts = explode('/', $template);
        $template = implode('/', array_slice($parts, 0, count($parts) - 1)).'/'.($partial ? '_' : '').$parts[count($parts) - 1];
      }

      $template = "{$template}.tpl";

      /* @var $smarty Smarty */
      $smarty = atkinstance('atk.ui.atksmarty');

      $oldVars = $smarty->get_template_vars();
      $oldSerials = $smarty->_cache_serials;
      $oldCaching = $smarty->caching;
      $oldForceCompile = $smarty->force_compile;

      $smarty->assign($vars);
      $smarty->caching = false;
      $smarty->force_compile = true;

      $result = $smarty->fetch($this->getTemplatePath($template));

      $layoutTemplatePath = $this->getLayoutTemplatePath();
      if ($this->isRoot() && !$partial && $layoutTemplatePath != null)
      {
        $vars['content_for_layout'] = $result;
        $smarty->assign($vars);
        $result = $smarty->fetch($layoutTemplatePath);
      }

      $smarty->force_compile = $oldForceCompile;
      $smarty->caching = $oldCaching;
      $smarty->_cache_serials = $oldSerials;
      $smarty->assign($oldVars);

      return $result;
    }

    /**
     * To string.
     */
    public function __toString()
    {
      return (!$this->isRoot() ? $this->getParent()->__toString().'/' : '').$this->getName();
    }
  }
