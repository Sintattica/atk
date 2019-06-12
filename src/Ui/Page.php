<?php

namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Atk;

/**
 * Page renderer.
 *
 * This class is used to render output as an html page. It takes care of
 * creating a header, loading javascripts and loading stylesheets.
 * Since any script will output exactly one page to the browser, this is
 * a singleton. Use getInstance() to retrieve the one-and-only instance.
 *
 * @todo This should actually not be a singleton. HTML file generation¬
 *       scripts may need an instance per page generated.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class Page
{
    /**
     * Rendering flags.
     */
    const HTML_BODY = 1;                        // Add body tags to page
    const HTML_HEADER = 2;                      // Add header to page
    const HTML_DOCTYPE = 4;                     // Add doctype to page
    const HTML_ALL = 3;     // Shortcut
    const HTML_STRICT = 7;  // Shortcut
    const HTML_PARTIAL = 8;                    // Partial (only content, this flag can't be ANDed!)

    /*
     * The list of javascript files to load.
     * @access private
     * @var array
     */
    public $m_metacode = [];

    /*
     * The list of javascript files to load.
     * @access private
     * @var array
     */
    public $m_scriptfiles = [];

    /*
     * List of javascript code statements to include in the header.
     * @access private
     * @var array
     */
    public $m_scriptcode = array('before' => [], 'after' => array());

    /*
     * List of javascript code statements to execute when a form on
     * the page is submitted.
     * @access private
     * @var array
     */
    public $m_submitscripts = [];

    /*
     * List of javascript code statements to execute when the page
     * is loaded.
     * @access private
     * @var array
     */
    public $m_loadscripts = [];

    /*
     * List of stylesheet files to load.
     * @access private
     * @var array
     */
    public $m_stylesheets = [];

    /*
     * List of style statements to include in the header.
     * @access private
     * @var array
     */
    public $m_stylecode = [];

    /*
     * The content to put on the page.
     * @access private
     * @var String
     */
    public $m_content = '';

    /*
     * The hidden variables for the page
     * @access private
     * @var array
     */
    public $m_hiddenvars = [];

    /**
     * Page title.
     *
     * @var string
     */
    protected $m_title = '';

    /**
     * Retrieve the one-and-only Page instance.
     *
     * @return Page
     */
    public static function getInstance()
    {
        static $s_page = null;
        if ($s_page == null) {
            $s_page = new self();
            Tools::atkdebug('Created a new Page instance');
        }

        return $s_page;
    }

    /**
     * Constructor.
     */
    public function __construct()
    {
        // register default scripts
        $assetsUrl = Config::getGlobal('assets_url');

        $this->register_scriptcode("var LANGUAGE='".Config::getGlobal('language')."';", true);
        $this->register_script($assetsUrl.'javascript/libs.min.js');
        $this->register_script($assetsUrl.'javascript/tools.js');
        $this->register_script($assetsUrl.'javascript/atk.js');

        $style_url = Config::getGlobal('style_url');
        if($style_url){
            $this->register_style(Config::getGlobal('style_url'));
        }
    }

    /**
     * Register a javascript file to be included.
     *
     * If called twice for the same filename, the file is loaded only once.
     *
     * @param string $file The (relative path and) filename of the javascript
     *                       file.
     * @param string $before The (partial) name of a script that this script
     *                       should be loaded in front of. This can be used
     *                       to inject a script before another script, or to
     *                       avoid conflicts. Usually, this parameter is not
     *                       needed.
     */
    public function register_script($file, $before = '')
    {
        if (!in_array($file, $this->m_scriptfiles)) {
            if ($before == '') {
                $this->m_scriptfiles[] = $file;
            } else {
                // lookup the dependency and inject script right before it.
                $result = [];
                $injected = false;
                for ($i = 0, $_i = Tools::count($this->m_scriptfiles); $i < $_i; ++$i) {
                    if (stristr($this->m_scriptfiles[$i], $before) !== false) {
                        // inject the new one here.
                        $result[] = $file;
                        $injected = true;
                    }
                    $result[] = $this->m_scriptfiles[$i];
                }
                if (!$injected) {
                    $result[] = $file;
                } // inject at the end if dependency not found.

                $this->m_scriptfiles = $result;
            }
        }
    }

    /**
     * Unregister all registered javascripts.
     */
    public function unregister_all_scripts()
    {
        $this->m_scriptfiles = [];
    }

    /**
     * Unregister a javascript file.
     *
     * @param string $name The (partial) name of the script to remove
     */
    public function unregister_script($name)
    {
        $removed = false;
        for ($i = 0, $_i = Tools::count($this->m_scriptfiles); $i < $_i; ++$i) {
            if (stristr($this->m_scriptfiles[$i], $name) !== false) {
                unset($this->m_scriptfiles[$i]);
                $removed = true;
            }
        }
        if ($removed) {
            $this->m_scriptfiles = array_values($this->m_scriptfiles);
        }
    }

    /**
     * Return all javascript files.
     *
     * @return array contain file paths
     */
    public function getScripts()
    {
        return $this->m_scriptfiles;
    }

    /**
     * Register a javascript code statement which will be rendered in the
     * header.
     *
     * The method has a duplicate check. Registering the exact same statement
     * twice, will result in the statement only being rendered and executed
     * once.
     *
     * @param string $code The javascript code to place in the header.
     * @param bool $before Include the script before the javascript files
     */
    public function register_scriptcode($code, $before = false)
    {
        $element = ($before ? 'before' : 'after');
        if (!in_array($code, $this->m_scriptcode[$element])) {
            $this->m_scriptcode[$element][] = $code;
        }
    }

    /**
     * Register a javascript code statement that is executed when a form on
     * the page is submitted.
     *
     * @todo This is inconsequent, if multiple forms are present, each should
     *       have its own submitscripts. Should be moved to an atkForm class.
     *
     * @param string $code The javascript code fragment to execute on submit.
     */
    public function register_submitscript($code)
    {
        if (!in_array($code, $this->m_submitscripts)) {
            $this->m_submitscripts[] = $code;
        }
    }

    /**
     * Returns a copy of the load scripts.
     */
    public function getLoadScripts()
    {
        return $this->m_loadscripts;
    }

    /**
     * Register a javascript code statement that is executed on pageload.
     *
     * @param string $code The javascript code fragment to execute on load.
     * @param int $offset
     */
    public function register_loadscript($code, $offset = null)
    {
        if (!in_array($code, $this->m_loadscripts) && $offset === null) {
            $this->m_loadscripts[] = $code;
        } else {
            if (!in_array($code, $this->m_loadscripts)) {
                array_splice($this->m_loadscripts, $offset, 0, $code);
            }
        }
    }

    /**
     * Return all javascript codes in an array.
     *
     * @return array
     */
    public function getScriptCodes()
    {
        $scriptCodes = array_merge($this->m_scriptcode['before'], $this->m_scriptcode['after']);
        $scriptCodes[] = $this->_getGlobalSubmitScriptCode();
        $scriptCodes[] = $this->_getGlobalLoadScriptCode();

        return $scriptCodes;
    }

    /**
     * Register a Cascading Style Sheet.
     *
     * This method has a duplicate check. Calling it with the same stylesheet
     * more than once, will still result in only one single include of the
     * stylesheet.
     *
     * @param string $file The (relative path and) filename of the stylesheet.
     * @param string $media The stylesheet media (defaults to 'all').
     */
    public function register_style($file, $media = 'all')
    {
        if (empty($media)) {
            $media = 'all';
        }

        if (!array_key_exists($file, $this->m_stylesheets)) {
            $this->m_stylesheets[$file] = $media;
        }
    }

    /**
     * Unregister a Cascading Style Sheet.
     *
     * @param string $file The (relative path and) filename of the stylesheet.
     */
    public function unregister_style($file)
    {
        if (array_key_exists($file, $this->m_stylesheets)) {
            unset($this->m_stylesheets[$file]);
        }
    }

    /**
     * Return all stylesheet files.
     *
     * @return array contain file paths
     */
    public function getStyles()
    {
        return $this->m_stylesheets;
    }

    /**
     * Register Cascading Style Sheet fragment that will be included in the
     * page header.
     *
     * @param string $code The Cascading Style Sheet code fragment to place in
     *                     the header.
     */
    public function register_stylecode($code)
    {
        if (!in_array($code, $this->m_stylecode)) {
            $this->m_stylecode[] = $code;
        }
    }

    /**
     * Return all style codes.
     *
     * @return array
     */
    public function getStyleCodes()
    {
        return $this->m_stylecode;
    }

    /**
     * Register hidden variables. These will be accessible to javascript and DHTML functions/scripts
     * but will not be shown to the user unless he/she has a very, very old browser
     * that is not capable of rendering CSS.
     *
     * @param array $hiddenvars the hiddenvariables we want to register
     */
    public function register_hiddenvars($hiddenvars)
    {
        foreach ($hiddenvars as $hiddenvarname => $hiddenvarvalue) {
            $this->m_hiddenvars[$hiddenvarname] = $hiddenvarvalue;
        }
    }

    /**
     * Generate the HTML header (<head></head>) statement for the page,
     * including all scripts and styles.
     *
     * @return string The HTML pageheader, including <head> and </head> tags.
     */
    public function head()
    {
        $res = '';
        $this->addMeta($res);
        $this->addScripts($res);
        $this->addStyles($res);

        $favico = Config::getGlobal('defaultfavico');
        if ($favico != '') {
            $res .= '  <link rel="icon" href="'.$favico.'" type="image/x-icon" />'."\n";
            $res .= '  <link rel="shortcut icon" href="'.$favico.'" type="image/x-icon" />'."\n";
        }

        return $res;
    }

    /**
     * Adds javascripts from the member variables to HTML output.
     *
     * @param string $res Reference to the HTML output
     * @param bool $partial Is this a partial request or a complete request
     */
    public function addScripts(&$res, $partial = false)
    {
        $count_scriptcode = Tools::count($this->m_scriptcode['before']);
        if ($count_scriptcode > 0) {
            $res .= '  <script type="text/javascript">'."\n";
        }
        $res .= $this->renderScriptCode('before');

        if ($count_scriptcode > 0) {
            $res .= "  </script>\n";
        }

        if (!$partial) {
            for ($i = 0; $i < Tools::count($this->m_scriptfiles); ++$i) {
                $res .= '  <script type="text/javascript" src="'.$this->m_scriptfiles[$i].'"></script>'."\n";
            }
        } else {
            $files = '';
            for ($i = 0; $i < Tools::count($this->m_scriptfiles); ++$i) {
                $files .= "ATK.Tools.loadScript('".$this->m_scriptfiles[$i]."');\n";
            }

            if (!empty($files)) {
                // prepend script files
                $res = '<script type="text/javascript">'.$files.'</script>'.$res;
            }
        }

        $res .= '  <script type="text/javascript">';

        // javascript code.
        $res .= $this->renderScriptCode('after');

        $res .= $this->_getGlobalSubmitScriptCode($partial);
        $res .= $this->_getGlobalLoadScriptCode($partial);

        $res .= "  </script>\n\n";
    }

    /**
     * Renders the registered javascripts, if $position is set to "before" the scripts will be
     * placed before the scripts that are already present. Otherwise they will be appended
     * at the end.
     *
     * @param string $position ("before" or "after")
     *
     * @return string
     */
    public function renderScriptCode($position)
    {
        $res = '';
        for ($i = 0, $_i = Tools::count($this->m_scriptcode[$position]); $i < $_i; ++$i) {
            $res .= $this->m_scriptcode[$position][$i]."\n";
        }

        return $res;
    }

    /**
     * Get the globalSubmit javascript code.
     *
     * @param bool $partial Is this a partial request or a complete request
     *
     * @return string with javascript code
     */
    public function _getGlobalSubmitScriptCode($partial = false)
    {
        // global submit script can only be registered in the original request
        if ($partial) {
            return '';
        }

        $res  = "if (!window.ATK) {var ATK = {};}\n";
        $res .= "ATK.globalSubmit = function(form, standardSubmit)\n";
        $res .= "    {\n";
        $res .= "      var retval = true; var bag = {};\n";
        $res .= "      if (typeof(ATK.FormSubmit.preGlobalSubmit) == 'function') { ATK.FormSubmit.preGlobalSubmit(form, bag, standardSubmit);}\n";

        for ($i = 0, $_i = Tools::count($this->m_submitscripts); $i < $_i; ++$i) {
            $res .= '      retval = '.$this->m_submitscripts[$i]."\n";
            $res .= "      if (retval != true) {\n";
            $res .= "        if (typeof(ATK.FormSubmit.postGlobalSubmit) == 'function') {\n";
            $res .= "           return ATK.FormSubmit.postGlobalSubmit(form, bag, retval, standardSubmit);\n";
            $res .= "        }\n";
            $res .= "        return false;\n";
            $res .= "      }\n";
        }

        $res .= "      if (typeof(ATK.FormSubmit.postGlobalSubmit) == 'function') { return ATK.FormSubmit.postGlobalSubmit(form, bag, retval, standardSubmit);}\n";
        $res .= "      return retval;\n";
        $res .= "    }\n";

        return $res;
    }

    /**
     * Get the globalLoad javascript code.
     *
     * @param bool $partial Is this a partial request or a complete request
     *
     * @return string with javascript code
     */
    public function _getGlobalLoadScriptCode($partial = false)
    {
        $res = '';
        if (Tools::count($this->m_loadscripts)) {
            $res = '';
            if (!$partial) {
                $res .= "function globalLoad()\n";
                $res .= "{\n";
            }

            for ($i = 0, $_i = Tools::count($this->m_loadscripts); $i < $_i; ++$i) {
                $res .= "{$this->m_loadscripts[$i]}\n";
            }

            if (!$partial) {
                $res .= "}\n";
                $res .= "jQuery(function ($) {globalLoad();});\n";
            }
        }

        return $res;
    }

    /**
     * Add stylesheets and stylecodes to the HMTL output.
     *
     * @param string $res Reference to the HTML output
     * @param bool $partial Is this a partial request or a complete request
     */
    public function addStyles(&$res, $partial = false)
    {
        if (!$partial) {
            foreach ($this->m_stylesheets as $file => $media) {
                $res .= '  <link href="'.$file.'" rel="stylesheet" type="text/css" media="'.$media.'" />'."\n";
            }

            for ($i = 0; $i < Tools::count($this->m_stylecode); ++$i) {
                $res .= '<style type="text/css"> '.$this->m_stylecode[$i].' </style>'."\n";
            }
        } else {
            $files = '';
            foreach ($this->m_stylesheets as $file => $media) {
                $files .= "ATK.Tools.loadStyle('{$file}', '{$media}');\n";
            }

            if (!empty($files)) {
                // prepend stylesheets
                $res = '<script type="text/javascript">'.$files.'</script>'.$res;
            }
        }
    }

    /**
     * Add content to the page.
     *
     * @param string $content The content to add to the page.
     */
    public function addContent($content)
    {
        $this->m_content .= $content;
    }

    /**
     * Returns the current page content.
     *
     * @return string current page content
     */
    public function getContent()
    {
        return $this->m_content;
    }

    /**
     * Sets the page content (overwriting current content).
     *
     * @param string $content new page content
     */
    public function setContent($content)
    {
        $this->m_content = $content;
    }

    /**
     * Sets the page title.
     *
     * @param string $title page title
     */
    public function setTitle($title)
    {
        $this->m_title = $title;
    }

    /**
     * Render the complete page, including head and body.
     *
     * @param string $title Title of the HTML page.
     * @param bool|int $flags (bool) Set to true to generate <body> tags. It is useful
     *                                 to set this to false only when rendering content
     *                                 that either already had its own <body></body>
     *                                 statement, or content that needs no body
     *                                 statements, like a frameset. (DEPRICATED !!)
     *                                 (int) Flags for the render function
     * @param string $extrabodyprops Extra attributes to add to the <body> tag.
     * @param string $extra_header HTML code of extra headers to add to the head section
     *
     * @return string The HTML page, including <html> and </html> tags.
     */
    public function render($title = null, $flags = self::HTML_STRICT, $extrabodyprops = '', $extra_header = '')
    {
        if ($title == null) {
            $title = $this->m_title != '' ? $this->m_title : Tools::atktext('app_title');
        }

        $ui = Ui::getInstance();

        if (is_bool($flags) && $flags == true) {
            $flags = self::HTML_STRICT;
        } elseif (is_bool($flags) && $flags == false) {
            $flags = self::HTML_HEADER | self::HTML_DOCTYPE;
        } elseif (Tools::hasFlag($flags, self::HTML_PARTIAL)) {
            return $this->renderPartial();
        }

        $this->m_content = $ui->render('page.tpl', array('content' => $this->m_content));

        $layout = [];
        $layout['title'] = $title;
        if (Tools::hasFlag($flags, self::HTML_HEADER)) {
            $layout['head'] = $this->head().$extra_header;
        }
        if (Tools::hasFlag($flags, self::HTML_BODY)) {
            $layout['extrabodyprops'] = $extrabodyprops;
            $layout['body'] = $this->m_content."\n";
        }

        $layout['hiddenvars'] = $this->hiddenVars();
        $layout['atkversion'] = Atk::VERSION;

        return $ui->render('layout.tpl', $layout);
    }

    /**
     * Render partial.
     */
    public function renderPartial()
    {
        $result = $this->m_content;
        $this->addMeta($result, true);
        $this->addScripts($result, true);
        $this->addStyles($result, true);

        return $result;
    }

    /**
     * Here we render a hidden div in the page with hidden variables
     * that we want to make accessible to client side scripts.
     *
     * @return string a hidden div with the selected ATK variabels
     */
    public function hiddenVars()
    {
        $res = '';
        if ($this->m_hiddenvars) {
            foreach ($this->m_hiddenvars as $hiddenvarname => $hiddenvarvalue) {
                $res .= "\n <span id='$hiddenvarname'>".htmlspecialchars($hiddenvarvalue).'</span>';
            }
        }

        return $res;
    }

    /**
     * Check if the page is empty (no content).
     *
     * This is useful to check at the rendering stage of scripts whether there is something to render.
     *
     * @param array $record The record that holds this attribute's value.
     * @return bool true if there is no content in the page, false if there is
     */
    public function isEmpty($record)
    {
        return $this->m_content == '';
    }

    /**
     * Add a meta tag to the page: <meta ... />.
     *
     * @param string $code
     */
    public function register_metacode($code)
    {
        $this->m_metacode[] = $code;
    }

    /**
     * Adds meta lines from the member variables to HTML output.
     *
     * @param string $res Reference to the HTML output
     * @param bool $partial Is this a partial request or a complete request
     */
    public function addMeta(&$res, $partial = false)
    {
        if (!$partial) {
            foreach ($this->m_metacode as $line) {
                $res .= '  '.$line."\n";
            }
        }
    }
}
