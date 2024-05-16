<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Ui\SmartyProvider;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Db\Db;

/**
 * This class implements the ATK debug console for analysing queries
 * performed in a page.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class Debugger
{
    public $m_isconsole = true;
    public $m_redirectUrl = null;
    private static $s_queryCount = 0;
    private static $s_systemQueryCount = 0;

    /**
     * Get an instance of this class.
     *
     * @return Debugger Instance of atkDebugger
     */
    public static function getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            if (!SessionManager::getInstance()) {
                Tools::atkwarning('Instantiating debugger without sessionmanager, debugger will not do anything until session is started. Also, the debugging info already in the session will not be cleaned, this could lead to monster sessions over time!');
            }
            $s_instance = new self();
        }

        return $s_instance;
    }

    /**
     * Constructor.
     */
    public function __construct()
    {
        $this->m_isconsole = (strpos($_SERVER['SCRIPT_NAME'], 'debugger.php') !== false);
        if (!$this->m_isconsole) {
            $link = $this->consoleLink('Open console', '', [], true);
            $data = $this->getDebuggerData(true);
            $data = []; // start clean
            global $g_debug_msg;
            $g_debug_msg[] = Tools::atkGetTimingInfo().'Debugger initialized. ['.$link.']';
        }
    }

    /**
     * Add a debug statement.
     *
     * @param string $txt The debug statement
     *
     * @return bool Indication if statement is added
     */
    public static function addStatement($txt)
    {
        if (SessionManager::getInstance()) {
            $instance = self::getInstance();
            if (is_object($instance)) {
                return $instance->_addStatement($txt);
            }
        }

        return false;
    }

    /**
     * Add a query string to the debugger.
     *
     * @param string $query
     * @param bool $isSystemQuery is system query? (e.g. for retrieving metadata, warnings, setting locks etc.)
     *
     * @return bool Indication if query is added
     */
    public static function addQuery(string $query, bool $isSystemQuery = false): ?bool
    {
        self::$s_queryCount += !$isSystemQuery ? 1 : 0;
        self::$s_systemQueryCount += $isSystemQuery ? 1 : 0;

        if (Config::getGlobal('debug') > 2) {
            if (SessionManager::getInstance()) {
                $instance = self::getInstance();
                if (is_object($instance)) {
                    return $instance->_addQuery($query);
                }
            }
        } else {
            Tools::atkdebug(htmlentities($query));

            return true;
        }

        return false;
    }

    /**
     * Add a debug statement.
     *
     * @param string $txt The debug statement
     *
     * @return bool Indication if statement is added
     */
    protected function _addStatement($txt)
    {
        if (!$this->m_isconsole) {
            $data = $this->getDebuggerData();
            global $g_debug_msg;
            $data['statements'][] = array('statement' => $txt, 'trace' => Tools::atkGetTrace());
            $link = $this->consoleLink('trace', 'statement', array('stmt_id' => Tools::count($data['statements']) - 1), true);
            $txt = preg_replace("|MB\]|", "MB] [$link]", $txt, 1);
            $g_debug_msg[] = $txt;

            return true;
        }

        return false;
    }

    /**
     * Add a query string to the debugger.
     *
     * @param string $query
     *
     * @return bool Indication if query is added
     */
    protected function _addQuery($query)
    {
        if (!$this->m_isconsole) { // don't add queries executed by the console itself
            $data = $this->getDebuggerData();

            $data['queries'][] = array('query' => $query, 'trace' => Tools::atkGetTrace());

            Tools::atkdebug('['.$this->consoleLink('query&nbsp;details', 'query', array('query_id' => Tools::count($data['queries']) - 1),
                    true).'] '.htmlentities($query));

            return true;
        }

        return false;
    }

    /**
     * Create the console link.
     *
     * @param string $text The name of the link
     * @param string $action The action
     * @param array $params Array with parameters to add to the url
     * @param bool $popup IS this a popup link?
     * @param int $stackId The stack id
     *
     * @return string HTML code with the console link
     */
    public function consoleLink($text, $action = '', $params = [], $popup = false, $stackId = null)
    {
        if ($stackId == null) {
            $sm = SessionManager::getInstance();
            $stackId = $sm->atkStackID();
        }

        static $s_first = true;
        $res = '';
        $url = './debugger.php?atkstackid='.$stackId.'&action='.$action.'&atkprevlevel='.$sm->atkLevel().$this->urlParams($params);

        if ($popup) {
            if ($s_first) {
                $s_first = false;
            }
            $res .= '<a href="javascript:ATK.Tools.newWindow(\''.$url.'\', \'atkconsole\', 800, 600, \'yes\', \'yes\')">'.$text.'</a>';
        } else {
            $res .= '<a href="'.$url.'">'.$text.'</a>';
        }

        return $res;
    }

    /**
     * Convert a params array to a querystring to add to the url.
     *
     * @param array $params
     *
     * @return string
     */
    public function urlParams($params)
    {
        if (Tools::count($params)) {
            $res = '';
            foreach ($params as $key => $value) {
                $res .= '&'.$key.'='.rawurlencode($value);
            }

            return $res;
        }

        return '';
    }

    /**
     * Render the console.
     *
     * @return string The HTML code
     */
    public function renderConsole()
    {
        $page = Page::getInstance();
        $data = $this->getDebuggerData(false, $_REQUEST['atkstackid']);
        $res = $this->consoleControls().'<br/><br/>';
        switch ($_REQUEST['action']) {
            case 'query':
                $res .= $this->queryDetails($data['queries'], $_REQUEST['query_id']);
                break;
            case 'statement':
                $res .= $this->statementDetails($data['statements'], $_REQUEST['stmt_id']);
                break;
            default: {
                $res .= $this->renderQueryList($data['queries']);
                $res .= $this->renderStatementList($data['statements']);
            }
        }
        $page->addContent($res);

        return $page->render('Console');
    }

    /**
     * Get the HTML code for the console controls.
     *
     * @return string The HTML code
     */
    public function consoleControls()
    {
        return '<div id="console"><table width="100%" border="0"><tr><td align="left">ATK Debug Console</td><td align="right">'.$this->consoleLink('Console index',
            '', [], false, $_REQUEST['atkstackid']).' | <a href="javascript:window.close()">Close console</a></td></tr></table></div>';
    }

    /**
     * Get details for the query.
     *
     * @param array $queries Array with queries
     * @param int $id The index in the queries array we want the details from
     *
     * @return string The query details
     */
    public function queryDetails($queries, $id)
    {
        $output = '<h1>Query</h1>';
        $query = $queries[$id]['query'];
        $output .= $this->highlightQuery($query);
        $db = Db::getInstance();
        if (strtolower(substr(trim($query), 0, 6)) == 'select') {
            $output .= '<h1>Resultset</h1>';
            $result = $db->getRows($query);
            if (Tools::count($result)) {
                $output .= $this->arrToTable($result, $_REQUEST['full'], $id);
            } else {
                $output .= 'Query returned no rows';
            }
            $output .= '<h1>Explain plan</h1>';
            $result = $db->getRows('EXPLAIN '.$query);
            $output .= $this->arrToTable($result);
        }
        if ($queries[$id]['trace'] != '') {
            $output .= '<h1>Backtrace</h1>';
            $output .= $queries[$id]['trace'];
        }

        return $output;
    }

    /**
     * Get the statement details.
     *
     * @param array $stmts Array with statements
     * @param int $id The index in the statements array we want the details from
     *
     * @return string The statement details
     */
    public function statementDetails($stmts, $id)
    {
        $output = '<h1>Debug Statement</h1>';
        $stmt = $stmts[$id]['statement'];
        $output .= '<b>'.$stmt.'</b>';

        if ($stmts[$id]['trace'] != '') {
            $output .= '<h1>Backtrace</h1>';
            $output .= $stmts[$id]['trace'];
        }

        return $output;
    }

    /**
     * Convert an array to a table.
     *
     * @param array $result The array to convert
     * @param bool $full All results?
     * @param int $id
     *
     * @return string HTML table
     */
    public function arrToTable($result, $full = true, $id = '')
    {
        if (Tools::count($result)) {
            $cols = array_keys($result[0]);
            $data = '<table border="1"><tr>';
            foreach ($cols as $col) {
                $data .= '<th>'.$col.'</th>';
            }
            $data .= '</tr>';
            for ($i = 0, $_i = Tools::count($result); $i < $_i && ($i < 10 || $full); ++$i) {
                $data .= '<tr><td>'.implode('</td><td>', $result[$i]).'</td></tr>';
            }
            $data .= '</table>';
            if ($i != $_i) {
                $data .= ($_i - $i).' more results. '.$this->consoleLink('Full result', 'query', array('query_id' => $id, 'full' => 1));
            }

            return $data;
        }

        return '';
    }

    /**
     * Highlight a query.
     *
     * @param string $query The query to highlight
     *
     * @return string The highlighted query
     */
    public function highlightQuery($query)
    {
        $query = strtolower($query);
        $query = str_replace('select', '<b>SELECT</b>', $query);
        $query = str_replace('distinct', '<b>DISTINCT</b>', $query);
        $query = str_replace('where', '<b>WHERE</b>', $query);
        $query = str_replace('from', '<b>FROM</b>', $query);
        $query = str_replace('order by', '<b>ORDER BY</b>', $query);
        $query = str_replace('group by', '<b>GROUP BY</b>', $query);
        $query = str_replace('left join', '<b>LEFT</b> join', $query);
        $query = str_replace('join', '<b>JOIN</b>', $query);
        $query = str_replace('update ', '<b>UPDATE</b> ', $query);
        $query = str_replace(' set ', ' <b>SET</b> ', $query);
        $query = str_replace('delete from', '<b>DELETE FROM</b>', $query);

        return '<span class="query">'.nl2br($query).'</span>';
    }

    /**
     * Get debugger data.
     *
     * @param bool $clean
     * @param int $stackId
     *
     * @return array Array with data
     */
    public function &getDebuggerData($clean = false, $stackId = null)
    {
        $sm = SessionManager::getInstance();

        if ($stackId == null) {
            $stackId = $sm->atkStackID();
        }

        if (is_object($sm)) {
            $session = &$sm->getSession();
            if ($clean) {
                $session['debugger'] = [];
            }
            $var = &$session['debugger'][$stackId];

            return $var;
        }
        $data = [];

        return $data;
    }

    /**
     * Render query list.
     *
     * @param array $queries
     *
     * @return string HTML code with the query list
     */
    public function renderQueryList($queries)
    {
        $output = 'Number of queries performed: '.Tools::count($queries);
        if (Tools::count($queries)) {
            $output .= '<table border="1" width="100%"><tr><th>#</th><th>Details</th><th>Query</th></tr>';

            for ($i = 0, $_i = Tools::count($queries); $i < $_i; ++$i) {
                $query = $queries[$i]['query'];
                if ($query == '') {
                    $detaillink = 'EMPTY QUERY!';
                } else {
                    $detaillink = $this->consoleLink('details', 'query', array('query_id' => $i));
                }
                $output .= '<tr><td valign="top">'.($i + 1).'</td><td>'.$detaillink.'</td><td>'.$this->highlightQuery($query).'</td></tr>';
            }

            $output .= '</table>';

            return $output;
        }
    }

    /**
     * Render statement list.
     *
     * @param array $statements
     *
     * @return string HTML code with the statement list
     */
    public function renderStatementList($statements)
    {
        $output = 'Number of debug statements: '.Tools::count($statements);
        if (Tools::count($statements)) {
            $output .= '<table border="1" width="100%"><tr><th>#</th><th>Details</th><th>Statement</th></tr>';

            for ($i = 0, $_i = Tools::count($statements); $i < $_i; ++$i) {
                $detaillink = $this->consoleLink('details', 'statement', array('stmt_id' => $i));
                $output .= '<tr><td valign="top">'.($i + 1).'</td><td>'.$detaillink.'</td><td>'.$statements[$i]['statement'].'</td></tr>';
            }

            $output .= '</table>';

            return $output;
        }
    }

    /**
     * Renders error messages for the user.
     *
     * @return string error messages string
     * @private
     */
    public function renderPlainErrorMessages()
    {
        global $g_error_msg;

        if (php_sapi_name() == 'cli') {
            $output = 'error: '.implode("\nerror: ", $g_error_msg)."\n";
        } else {
            $output = '<br><div style="font-family: monospace; font-size: 11px; color: #FF0000" align="left">error: '.implode("<br>\nerror: ",
                    $g_error_msg).'</div>';
        }

        return $output;
    }

    /**
     * Render debug block for the current debug information.
     *
     * @param bool $expanded Display debugblock expanded?
     *
     * @return string debug block string
     * @private
     */
    public function renderDebugBlock($expanded)
    {
        global $g_debug_msg, $g_error_msg, $g_startTime;

        $time = strftime('%H:%M:%S', $g_startTime);
        $duration = sprintf('%02.05f', self::getMicroTime() - $g_startTime);
        $usage = function_exists('memory_get_usage') ? sprintf('%02.02f', (memory_get_usage() / 1024 / 1024)) : '? ';
        $method = $_SERVER['REQUEST_METHOD'];
        $protocol = empty($_SERVER['HTTPS']) || strtolower($_SERVER['HTTPS']) == 'off' ? 'http' : 'https';
        $url = $protocol.'://'.$_SERVER['SERVER_NAME'].($_SERVER['SERVER_PORT'] != 80 ? ':'.$_SERVER['SERVER_PORT'] : '').$_SERVER['REQUEST_URI'];
        $label = "[{$time}h / {$duration}s / {$usage}MB / ".self::$s_queryCount.' Queries / '.self::$s_systemQueryCount." System Queries] $method $url";

        $output = '
        <div class="atkDebugBlock'.(Tools::count($g_error_msg) > 0 ? ' atkDebugBlockContainsErrors' : '').' atkDebug'.($expanded ? 'Expanded' : 'Collapsed').'">
          <div class="atkDebugToggle" onclick="ATK.Debug.toggle(this)">
           '.$label.'
          </div>
          <div class="atkDebugData">
            '.(Tools::count($g_debug_msg) > 0 ? '<div class="atkDebugLine">'.implode('</div><div class="atkDebugLine">', $g_debug_msg).'</div>' : '').'
          </div>
        </div>';

        return $output;
    }

    /**
     * Set redirect URL.
     *
     * @param string $url The redirect url
     * @param bool $force Force to set this redirect url
     */
    public function setRedirectUrl($url, $force = false)
    {
        // normally we only save the first redirect url, but using the force
        // parameter you can force setting another redirect url
        if ($this->m_redirectUrl === null || $force) {
            $this->m_redirectUrl = $url;
        }
    }

    /**
     * Renders the redirect link if applicable.
     */
    public function renderRedirectLink()
    {
        if ($this->m_redirectUrl == null) {
            return '';
        }

        $output = '
        <div class="atkDebugRedirect">
           Non-debug version would have redirected to <a href="'.$this->m_redirectUrl.'">'.$this->m_redirectUrl.'</a>
        </div>';

        return $output;
    }

    /**
     * Renders the debug and error messages to a nice HTML string.
     *
     * @return string html string
     */
    public function renderDebugAndErrorMessages()
    {
        global $ATK_VARS, $g_debug_msg, $g_error_msg;

        // check if this is an Ajax request
        $isPartial = isset($ATK_VARS['atkpartial']);

        // only display error messages
        if (Tools::count($g_error_msg) > 0 && Config::getGlobal('display_errors') && Config::getGlobal('debug') <= 0 && !$isPartial) {
            return $this->renderPlainErrorMessages();
        } // no debug messages or error messages to output
        else {
            if (Config::getGlobal('debug') <= 0 || (Tools::count($g_debug_msg) == 0 && Tools::count($g_error_msg) == 0)) {
                return '';
            }
        }

        $expanded = !$isPartial;
        if ($expanded && array_key_exists('atkdebugstate', $_COOKIE) && @$_COOKIE['atkdebugstate'] == 'collapsed') {
            $expanded = false;
        }

        // render debug block
        $block = $this->renderDebugBlock($expanded);

        if ($isPartial) {
            $output = '<script type="text/javascript">
            ATK.Debug.addContent('.Json::encode($block).');
           </script>';
        } else {
            $script = Config::getGlobal('assets_url').'javascript/debug.js';

            $redirect = $this->renderRedirectLink();

            $output = '<script type="text/javascript" src="'.$script.'"></script>';

            //Page::getInstance()->register_loadscript($script);

            $output .= Ui::getInstance()->render('debugger.tpl', ['redirect' => $redirect, 'block' => $block]);

            /*
            $output = '
          <script type="text/javascript" src="'.$script.'"></script>
          <div id="debugger_wrapper" class="content-wrapper" style="padding-left: 10px; height: 100px !important; position: absolute; bottom:0;">
          <hr style="margin-top:0">

          <div id="atk_debugging_div" style="font-size: 10pt;">
            '.$redirect.'
            '.$block.'
          </div>
          </div>
          ';*/
        }

        return $output;
    }

    /**
     * Gets the microtime.
     *
     * @static
     *
     * @return int the microtime
     */
    public static function getMicroTime()
    {
        list($usec, $sec) = explode(' ', microtime());

        return (float)$usec + (float)$sec;
    }

    /**
     * Gets the elapsed time.
     *
     * @return string The elapsed time
     */
    public static function elapsed(): string
    {
        global $g_startTime;

        static $offset = null, $previous = null;

        if ($offset === null) {
            $offset = $g_startTime;
            $previous = $offset;
        }

        $new = self::getMicroTime();

        $execDuration = $new - $offset; //Duration of the executed instruction in absolute terms (instant time difference when each instruction starts)
        $relativeDuration =  $offset === $previous ? '0' : $new - $previous; //Relative time difference (how much time has passed since the last instruction was executed)

        $res =  '+' . sprintf('%02.05f', $execDuration).'s / '. sprintf('%02.05f', $relativeDuration) .'s';
        $previous = $new;

        return $res;
    }
}
