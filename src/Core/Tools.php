<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Ui\Output;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Errors\ErrorHandlerBase;
use Sintattica\Atk\Utils\Debugger;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Utils\BrowserInfo;
use Sintattica\Atk\Handlers\ActionHandler;
use Exception;

class Tools
{
    /**
     * Don't escape html special characters before printing. Use this flag
     * if your message intentionnaly contains HTML tags (and has been
     * safely escaped for non-HTML parts).
     */
    const DEBUG_ESCAPED = 1;

    /**
     * Wraps the self::text into html bold tags in order to make warnings more
     * clearly visible.
     */
    const DEBUG_WARNING = 2;

    /**
     * Hides the debug unless in level 2.
     * This should be used for debug you only really want to see if you
     * are developing (like deprecation warnings).
     */
    const DEBUG_NOTICE = 4;

    /**
     * Error message.
     */
    const DEBUG_ERROR = 8;
    /**
     * Accepted charsets for htmlentities and html_entity_decode.
     *
     * @var array
     */
    public static $s_acceptedCharsets = array(
        'iso-8859-1',
        'iso-8859-15',
        'utf-8',
        'cp866',
        'ibm866',
        '866',
        'cp1251',
        'windows-1251',
        'win-1251',
        '1251',
        'cp1252',
        'windows-1252',
        '1252',
        'koi8-r',
        'koi8-ru',
        'koi8r',
        'big5',
        '950',
        'gb2312',
        '936',
        'big5-hkscs',
        'shift_jis',
        'sjis',
        '932',
        'euc-jp',
        'eucjp',
    );
    /**
     * Do we have multibyte support.
     *
     * @var bool
     */
    public static $s_hasMultiByteSupport = null;

    /**
     * Function self::atkErrorHandler
     * This function catches PHP parse errors etc, and passes
     * them to self::atkerror(), so errors can be mailed and output
     * can be regulated.
     * This funtion must be registered with set_error_handler("self::atkErrorHandler");.
     *
     * @param $errtype : One of the PHP errortypes (E_PARSE, E_USER_ERROR, etc)
     * (See http://www.php.net/manual/en/function.error-reporting.php)
     * @param $errstr : Error self::text
     * @param $errfile : The php file in which the error occured.
     * @param $errline : The line in the file on which the error occured.
     */
    public static function atkErrorHandler($errtype, $errstr, $errfile, $errline)
    {
        // probably suppressed error using the @ operator, simply ignore
        if (error_reporting() == 0) {
            return;
        }

        $errortype = array(
            E_ERROR => "Error",
            E_WARNING => "Warning",
            E_PARSE => "Parsing Error",
            E_NOTICE => "Notice",
            E_CORE_ERROR => "Core Error",
            E_CORE_WARNING => "Core Warning",
            E_COMPILE_ERROR => "Compile Error",
            E_COMPILE_WARNING => "Compile Warning",
            E_USER_ERROR => "User Error",
            E_USER_WARNING => "User Warning",
            E_USER_NOTICE => "User Notice",
            E_STRICT => "Strict Notice",
            'EXCEPTION' => 'Uncaught exception',
        );

        // E_RECOVERABLE_ERROR is available since 5.2.0
        if (defined('E_RECOVERABLE_ERROR')) {
            $errortype[E_RECOVERABLE_ERROR] = "Recoverable Error";
        }

        // E_DEPRECATED / E_USER_DEPRECATED are available since 5.3.0
        if (defined('E_DEPRECATED')) {
            $errortype[E_DEPRECATED] = "Deprecated";
            $errortype[E_USER_DEPRECATED] = "User Deprecated";
        }

        // Translate the given errortype into a string
        $errortypestring = $errortype[$errtype];

        if ($errtype == E_STRICT) {
            // ignore strict notices for now, there is too much stuff that needs to be fixed
            return;
        } else {
            if ($errtype == E_NOTICE) {
                // Just show notices
                self::atkdebug("[$errortypestring] $errstr in $errfile (line $errline)", self::DEBUG_NOTICE);

                return;
            } else {
                if (defined('E_DEPRECATED') && ($errtype & (E_DEPRECATED | E_USER_DEPRECATED)) > 0) {
                    // Just show deprecation warnings in the debug log, but don't influence the program flow
                    self::atkdebug("[$errortypestring] $errstr in $errfile (line $errline)", self::DEBUG_NOTICE);

                    return;
                } else {
                    if (($errtype & (E_WARNING | E_USER_WARNING)) > 0) {
                        // This is something we should pay attention to, but we don't need to die.
                        self::atkerror("[$errortypestring] $errstr in $errfile (line $errline)");

                        return;
                    } else {

                        self::atkerror("[$errortypestring] $errstr in $errfile (line $errline)", ($errtype == 'EXCEPTION'));
                        // we must die. we can't even output anything anymore..
                        // we can do something with the info though.
                        self::atkhalt($errstr, 'critical');
                    }
                }
            }
        }
    }

    /**
     * Default ATK exception handler, handles uncaught exceptions and calls atkHalt.
     *
     * @param Exception $exception uncaught exception
     */
    public static function atkExceptionHandler($exception)
    {
        self::atkErrorHandler('EXCEPTION', $exception->getMessage(), $exception->getFile(), $exception->getLine());
    }

    /**
     * Default ATK fatal handler
     */
    public static function atkFatalHandler()
    {

        $error = error_get_last();
        if ($error) {
            self::atkErrorHandler(E_ERROR, $error['message'], $error['file'], $error['line']);
        }
    }

    /**
     * Function self::atkhalt
     * Halts on critical errors and also on warnings if specified in the config file.
     *
     * @param string $msg The message to be displayed
     * @param string $level The level of the error,
     *                      ("critical"|"warning" (default))
     *
     * @return bool false if something goes horribly wrong
     */
    public static function atkhalt($msg, $level = 'warning')
    {
        if ($level == Config::getGlobal('halt_on_error') || $level == 'critical') {
            if ($level == 'warning') {
                $level_color = '#0000ff';
            } else {
                // critical
                $level_color = '#ff0000';
            }

            if (php_sapi_name() == 'cli') {
                $res = self::atktext($level, 'atk').': '.$msg."\n";
            } else {
                $res = '<html>';
                $res .= '<body style="background-color: #ffffff; color:#000000;">';
                $res .= "<span style=\"color:$level_color;\"><b>".self::atktext($level, 'atk')."</b></span>: $msg.<br />\n";
            }

            Output::getInstance()->output($res);
            Output::getInstance()->outputFlush();
            exit("Halted...\n");
        } else {
            self::atkerror("$msg");
        }

        return false;
    }

    /**
     * Function atkdebug.
     *
     * Adds debug self::text to the debug log
     *
     * @param string $txt The self::text that will be added to the log
     * @param int $flags An optional combination of Tools::DEBUG_ flags
     */
    public static function atkdebug($txt, $flags = 0)
    {
        global $g_debug_msg;
        $level = Config::getGlobal('debug');
        if ($level >= 0) {
            if (!self::hasFlag($flags, self::DEBUG_ESCAPED)) {
                $txt = htmlspecialchars($txt);
            }
            if (self::hasFlag($flags, self::DEBUG_WARNING)) {
                $txt = '<b>'.$txt.'</b>';
            }

            $line = self::atkGetTimingInfo().$txt;
            self::atkWriteLog($line);

            if (self::hasFlag($flags, self::DEBUG_ERROR)) {
                $line = '<span class="atkDebugError">'.$line.'</span>';
            }

            if ($level > 2) {
                if (!Debugger::addStatement($line)) {
                    $g_debug_msg[] = $line;
                }
            } else {
                if (!self::hasFlag($flags, self::DEBUG_NOTICE)) {
                    $g_debug_msg[] = $line;
                }
            }
        } else {
            if ($level > -1) { // at 0 we still collect the info so we
                // have it in error reports. At -1, we don't collect
                $g_debug_msg[] = $txt;
            }
        }
    }

    /**
     * Send a notice to the debug log.
     * A notice doesn't get show unless your debug level is 3 or higher.
     *
     * @param string $txt The self::text that will be added to the log
     */
    public static function atknotice($txt)
    {
        self::atkdebug($txt, self::DEBUG_NOTICE);
    }

    /**
     * Send a warning to the debug log.
     * A warning gets shown more prominently than a normal debug line.
     * However it does not trigger a mailreport
     * or anything else that an self::atkerror triggers.
     *
     * @param string $txt
     */
    public static function atkwarning($txt)
    {
        self::atkdebug($txt, self::DEBUG_WARNING);
    }

    public static function atkGetTimingInfo()
    {
        return '['.Debugger::elapsed().(Config::getGlobal('debug') > 0 && function_exists('memory_get_usage') ? ' / '.sprintf('%02.02f',
                (memory_get_usage() / 1024 / 1024)).'MB' : '').'] ';
    }

    /**
     * Like self::atkdebug, this displays a message at the bottom of the screen.
     * The difference is, that this is also displayed when debugging is turned
     * off.
     *
     * If error reporting by email is turned on, the error messages are also
     * send by e-mail.
     *
     * @param string|Exception $error the error self::text or exception to display
     * @param bool $skipThrow
     *
     * @throws \Exception if throw_exception_on_error
     */
    public static function atkerror($error, $skipThrow = false)
    {
        global $g_error_msg, $g_debug_msg;

        if ($error instanceof \Exception) {
            $g_error_msg[] = '['.Debugger::elapsed().'] '.$error->getMessage();
            self::atkdebug(nl2br($error->getMessage()."\n".$error->getTraceAsString()), self::DEBUG_ERROR);
        } else {
            $g_error_msg[] = '['.Debugger::elapsed().'] '.$error;
            self::atkdebug($error, self::DEBUG_ERROR);
        }

        if (function_exists('debug_backtrace')) {
            self::atkdebug('Trace:'.self::atkGetTrace('html'), self::DEBUG_ERROR|self::DEBUG_ESCAPED);
        }

        $default_error_handlers = [];
        $mailReport = Config::getGlobal('mailreport');
        if ($mailReport) {
            $default_error_handlers['Mail'] = array('mailto' => $mailReport);
        }

        $errorHandlers = Config::getGlobal('error_handlers', $default_error_handlers);
        foreach ($errorHandlers as $key => $value) {
            if (is_numeric($key)) {
                $key = $value;
            }
            $errorHandlerObject = ErrorHandlerBase::get($key, $value);
            $errorHandlerObject->handle($g_error_msg, $g_debug_msg);
        }

        if (!$skipThrow && Config::getGlobal('throw_exception_on_error')) {
            if ($error instanceof Exception) {
                throw $error;
            } else {
                throw new Exception($error);
            }
        }
    }

    /**
     * Returns a trace-route from all functions where-through the code has been executed.
     *
     * @param string $format (html|plaintext)
     *
     * @return string Backtrace in html or plaintext format
     */
    public static function atkGetTrace($format = 'html')
    {
        // Get the debug backtrace
        $traceArr = debug_backtrace();

        // Remove the call of self::atk_get_trace
        array_shift($traceArr);

        // Start with an empty result;
        $ret = '';

        $theSpacer = '';

        // Loop through all items found in the backtrace
        for ($i = 0, $_i = Tools::count($traceArr); $i < $_i; ++$i) {
            //for($i=Tools::count($traceArr)-1; $i >= 0; $i--)
            // Skip this item in the backtrace if empty
            if (empty($traceArr[$i])) {
                continue;
            }

            // Don't display an self::atkerror statement itself.
            if ($traceArr[$i]['function'] == 'self::atkerror') {
                continue;
            }

            // Read the source location
            if (isset($traceArr[$i]['file'])) {
                $location = $traceArr[$i]['file'].(isset($traceArr[$i]['line']) ? sprintf(', line %d', $traceArr[$i]['line']) : '[Unknown line]');
            } else {
                $location = '[PHP KERNEL]';
            }

            // Read the statement
            if (isset($traceArr[$i]['class'])) {
                $statement = $traceArr[$i]['class'];
                if (isset($traceArr[$i]['type'])) {
                    $statement .= $traceArr[$i]['type'];
                }
            } else {
                $statement = '';
            }
            $statement .= $traceArr[$i]['function'];

            // Initialize the functionParamArr array
            $functionParamArr = [];

            // Parse any arguments into the array
            if (isset($traceArr[$i]['args'])) {
                foreach ($traceArr[$i]['args'] as $val) {
                    if (is_array($val)) {
                        $valArr = [];
                        foreach ($val as $name => $value) {
                            if (is_object($value)) {
                                $valArr[] = sprintf("%s=Object(%s)", $name, get_class($value));
                            }elseif (is_array($value)) {
                                $valArr[] = $name.'=Array(...)';
                            }else {
                                $valArr[] = $name.'='.$value;
                            }
                        }
                        $stringval = 'array('.implode(', ', $valArr).')';
                    } else {
                        if (is_null($val)) {
                            $stringval = 'null';
                        } else {
                            if (is_object($val)) {
                                $stringval = sprintf('Object(%s)', get_class($val));
                            } else {
                                if (is_bool($val)) {
                                    $stringval = $val ? 'true' : 'false';
                                } else {
                                    if (strlen($val.$theSpacer) > 103) {
                                        $stringval = '"'.substr($val, 0, 100 - strlen($theSpacer)).'"...';
                                    } else {
                                        $stringval = '"'.$val.'"';
                                    }
                                }
                            }
                        }
                    }
                    $functionParamArr[] = $theSpacer.'  '.$stringval;
                }
            }
            $functionParams = implode(",\n", $functionParamArr);

            $ret .= $theSpacer.'@'.$location."\n";
            $ret .= $theSpacer.$statement;
            $ret .= (strlen($functionParams) ? "\n".$theSpacer."(\n".$functionParams."\n".$theSpacer.')' : '()')."\n";

            // Add indentation
            $theSpacer .= '  ';
        }

        // If html format should be used, replace the html special chars with html entities and put the backtrace within preformat tags.
        if ($format == 'html') {
            $ret = '<pre>'.htmlspecialchars($ret).'</pre>';
        }

        // Return the generated trace
        return $ret;
    }

    /**
     * Writes info to a given file.
     * Useful for writing to any log files.
     *
     * @param string $text self::text to write to the logfile
     * @param string $file the file name
     */
    public static function atkWriteToFile($text, $file = '')
    {
        $fp = @fopen($file, 'a');
        if ($fp) {
            fwrite($fp, $text."\n");
            fclose($fp);
        }
    }

    /**
     * Writes info to the optional debug logfile.
     * Please notice this feature will heavily decrease the performance
     * and should therefore only be used for debugging and development
     * purposes.
     *
     * @param string $text self::text to write to the logfile
     */
    public static function atkWriteLog($text)
    {
        if (Config::getGlobal('debug') > 0 && Config::getGlobal('debuglog')) {
            self::atkWriteToFile($text, Config::getGlobal('debuglog'));
        }
    }

    /**
     * Replaces the [vars] with the values from the language files
     * Please note that it is important, for performance reasons,
     * that you pass along the module where the language files can be found.
     *
     * @param string|array $string string or array of strings containing the name(s) of the string to return
     *                               when an array of strings is passed, the second will be the fallback if
     *                               the first one isn't found, and so forth
     * @param string $module module in which the language file should be looked for,
     *                               defaults to core module with fallback to ATK
     * @param string $node the node to which the string belongs
     * @param string $lng ISO 639-1 language code, defaults to config variable
     * @param string $firstfallback the first module to check as part of the fallback
     * @param bool $nodefaulttext if true, then it doesn't return a default text when it can't find a translation
     * @param bool $modulefallback Wether or not to use all the modules of the application in the fallback,
     *                               when looking for strings
     *
     * @return string the string from the languagefile
     */
    public static function atktext(
        $string,
        $module = '',
        $node = '',
        $lng = '',
        $firstfallback = '',
        $nodefaulttext = false,
        $modulefallback = false
    ) {
        return Language::text($string, $module, $node, $lng, $firstfallback, $nodefaulttext, $modulefallback);
    }

    /**
     * array_merge without duplicates
     * Supports unlimited number of arrays as arguments.
     *
     * @param array $array1 <p>
     *                      Initial array to merge.
     *                      </p>
     * @param array $array2 [optional]
     * @param array $_ [optional]
     *
     * @return array The result of the merge between $array1 and $array2
     */
    public static function atk_array_merge(array $array1, array $array2 = null, array $_ = null)
    {
        return array_unique(call_user_func_array('array_merge', func_get_args()), SORT_REGULAR);
    }

    /**
     * Same as array_merge_recursive from PHP but without duplicates.
     * Supports unlimited number of arrays as arguments.
     *
     * @param array $array1 first array
     * @param array $array2 second array
     *
     * @return array merged arrays
     */
    public static function atk_array_merge_recursive($array1, $array2)
    {
        $arrays = func_get_args();

        $result = [];

        foreach ($arrays as $array) {
            foreach ($array as $key => $value) {
                if (isset($result[$key]) && is_array($result[$key]) && is_array($value)) {
                    $result[$key] = self::atk_array_merge_recursive($result[$key], $value);
                } else {
                    $result[$key] = $value;
                }
            }
        }

        return $result;
    }

    /**
     * Same as array_merge from php, but this function preserves key=>index
     * association in case of numerical indexes. Supports unlimited number
     * of arrays as arguments.
     *
     * @param array $array unlimited number of arrays
     *
     * @return array The result of the merge between the given arrays
     */
    public static function atk_array_merge_keys()
    {
        $args = func_get_args();
        $result = [];
        foreach ($args as $array) {
            foreach ($array as $key => $value) {
                $result[$key] = $value;
            }
        }

        return $result;
    }

    /**
     * Since php triggers an error if you perform an in_array on an
     * uninitialised array, we provide a small wrapper that performs
     * an is_array on the haystack first, just to make sure the user
     * doesn't get an error message.
     *
     * @param mixed $needle The value to search for.
     * @param array $haystack The array to search.
     * @param bool $strict If true, type must match.
     *
     * @return bool wether or not the value is in the array
     */
    public static function atk_in_array($needle, $haystack, $strict = false)
    {
        return is_array($haystack) && in_array($needle, $haystack, $strict);
    }

    /**
     * Function dataSetContains.
     *
     * Checks if a value is in a Array
     *
     * @param array $set the array
     * @param mixed $key the key in the array as in $array[$key]
     * @param mixed $value the value we are looking for
     *
     * @return bool wether or not the value is in the array
     */
    public static function dataSetContains($set, $key, $value)
    {
        for ($i = 0; $i < Tools::count($set); ++$i) {
            if ($set[$i][$key] == $value) {
                return true;
            }
        }

        return false;
    }

    /**
     * Same as strip_slashes from php, but if the passed value is an array,
     * all elements of the array are stripped. Recursive function.
     *
     * @param $var string|array the value/array to strip the slashes of
     */
    public static function atk_stripslashes(&$var)
    {
        if (is_array($var)) {
            foreach (array_keys($var) as $key) {
                self::atk_stripslashes($var[$key]);
            }
        } else {
            $var = stripslashes($var);
        }
    }

    /**
     * Performs stripslashes on all vars and translates:
     *                 something_AMDAE_other[] into something[][other]
     *                 something_AE_other into something[other]
     *                 (and a_AE_b_AE_c into a[b][c] and so on...
     *
     * @param array &$vars the array to be stripped and translated
     */
    public static function atkDataDecode(&$vars)
    {
        foreach (array_keys($vars) as $varname) {
            $value = &$vars[$varname];

            self::AE_decode($vars, $varname);

            if (strpos(strtoupper($varname), '_AMDAE_') > 0) { // Now I *know* that strpos could return 0 if _AMDAE_ *is* found
                // at the beginning of the string.. but since that's not a valid
                // encoded var, we do nothing with it.
                // This string is encoded.
                list($dimension1, $dimension2) = explode('_AMDAE_', strtoupper($varname));
                if (is_array($value)) {
                    // Multidimensional thing
                    for ($i = 0; $i < Tools::count($value); ++$i) {
                        $vars[strtolower($dimension1)][$i][strtolower($dimension2)] = $value[$i];
                    }
                } else {
                    $vars[strtolower($dimension1)][strtolower($dimension2)] = $value;
                }
            }
        }
    }

    /**
     * Weird function. $dest is an associative array, that may contain
     * stuff like $dest["a_AE_c_AE_b"] = 3.
     * Now if you run this function like this:
     *  AE_decode($dest, "a_AE_c_AE_b");
     * then $dest will contain a decoded array:
     *  echo $dest["a"]["b"]["c"]; <- this will display 3.
     *
     * @param array &$dest the array to put the decoded var in
     * @param string $var the var to decode
     */
    public static function AE_decode(&$dest, $var)
    {
        $items = explode('_AE_', $var);
        if (Tools::count($items) <= 1) {
            return;
        }

        $current = &$dest;
        foreach ($items as $key) {
            $current = &$current[$key];
        }

        if (is_array($dest[$var])) {
            $current = self::atk_array_merge_recursive((array)$current, $dest[$var]);
        } else {
            $current = $dest[$var];
        }

        unset($dest[$var]);
    }

    /**
     * Safe urlencode function. Note, you can reencode already encoded strings, but
     * not more than 9 times!
     * If you encode a string more than 9 times, you won't be able to decode it
     * anymore.
     *
     * An atkurlencoded string is normaly prefixed with '__', so self::atkurldecode can
     * determine whether the string was encoded or not. Sometimes however, if you
     * need to reencode part of a string (used in recordlist), you don't want the
     * prefix. Pass false as second parameter, and you won't get a prefix. (Note
     * that you can't self::atkurldecode that string anymore, so only use this on
     * substrings of already encoded strings)
     *
     * @todo Fix a problem where a string containing "_9" will be altered after encoding + decoding it.
     *
     * @param string $string the url to encode
     * @param bool $pref wether or not to use a prefix, default true
     *
     * @return string the encoded url
     */
    public static function atkurlencode($string, $pref = true)
    {
        $string = rawurlencode($string);
        for ($i = 8; $i >= 1; --$i) {
            $string = str_replace('_'.$i, '_'.($i + 1), $string);
        }

        return ($pref ? '__' : '').str_replace('%', '_1', $string);
    }

    public static function atkurldecode($string)
    {
        if (substr($string, 0, 2) != '__') {
            return $string;
        } else {
            $string = str_replace('_1', '%', substr($string, 2));
            for ($i = 1; $i <= 8; ++$i) {
                $string = str_replace('_'.($i + 1), '_'.$i, $string);
            }

            return rawurldecode($string);
        }
    }

    /**
     * Adds new element to error array en $record. When
     * $msg is empty the multilange error string is used.
     *
     * @param array &$rec var in which to add element to error array
     * @param array|string $attrib attributename or an array with attribute names
     * @param string $err multilanguage error string
     * @param string $msg optional error string
     */
    public static function triggerError(&$rec, $attrib, $err, $msg = '', $tab = '', $label = '', $module = 'atk')
    {
        if ($msg == '') {
            $msg = self::atktext($err, $module);
        }
        $rec['atkerror'][] = array(
            'attrib_name' => $attrib,
            'err' => $err,
            'msg' => $msg,
            'tab' => $tab,
            'label' => $label,
        );
    }

    /**
     * Adds new element to the record error array. When no message
     * is given the multi-language error string is used.
     *
     * @param array $record record
     * @param Attribute|Attribute[] $attrib attribute or array of attributes
     * @param string $error multi-language error string
     * @param string $message error message (optional)
     */
    public static function atkTriggerError(&$record, $attrib, $error, $message = '')
    {
        if (is_array($attrib)) {
            $attribName = [];
            $label = [];

            for ($i = 0; $i < Tools::count($attrib); ++$i) {
                $attribName[$i] = $attrib[$i]->fieldName();
                $label[$i] = $attrib[$i]->label();
            }

            $tab = $attrib[0]->m_tabs[0];
            if (!$message) {
                $message = $attrib[0]->text($error);
            }
        } else {
            $attribName = $attrib->fieldName();
            $label = $attrib->label();
            $tab = $attrib->m_tabs[0];
            if (!$message) {
                $message = $attrib->text($error);
            }
        }

        self::triggerError($record, $attribName, $error, $message, $tab, $label);
    }

    /**
     * Does a var dump of an array. Makes use of self::atkdebug for displaying the values.
     *
     * @param $a array to be displayed
     * @param $d string name of the data that's being displayed.
     */
    public static function atk_var_dump($a, $d = '')
    {
        ob_start();
        var_dump($a);
        $data = htmlspecialchars(ob_get_contents());
        $d .= ($d != '' ? ' = ':'');
        self::atkdebug('vardump: '.htmlspecialchars($d).'<pre>'.$data.'</pre>', self::DEBUG_ESCAPED);
        ob_end_clean();
    }

    /**
     * This function writes data to the browser for download.
     * $data is the data to download.
     * $filename is the name the file will get when the user downloads it.
     * $compression can be "zip", "gzip" or "bzip", which causes the data
     *              to be compressed before transmission.
     */
    public static function exportData($data, $filename, $compression = '')
    {
        $browser = self::getBrowserInfo();
        if (preg_match('/ie/i', $browser['browser'])) {
            $mime = 'application/octetstream';
            $disp = 'inline';
        } else {
            if (preg_match('/opera/i', $browser['browser'])) {
                $mime = 'application/octetstream';
                $disp = 'attachment';
            } else {
                $mime = 'application/octet-stream';
                $disp = 'attachment';
            }
        }

        if ($compression == 'bzip') {
            $mime = 'application/x-bzip';
            $filename .= '.bz2';
        } else {
            if ($compression == 'gzip') {
                $mime = 'application/x-gzip';
                $filename .= '.gz';
            } else {
                if ($compression == 'zip') {
                    $mime = 'application/x-zip';
                    $filename .= '.zip';
                }
            }
        }

        header('Content-Type: '.$mime);
        header('Content-Disposition:  '.$disp.'; filename="'.$filename.'"');
        if (preg_match('/ie/i', $browser['browser'])) {
            header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
        }
        header('Pragma: no-cache');
        header('Expires: 0');

        // 1. as a bzipped file
        if ($compression == 'bzip') {
            if (@function_exists('bzcompress')) {
                echo bzcompress($data);
            }
        } // 2. as a gzipped file
        else {
            if ($compression == 'gzip') {
                if (@function_exists('gzencode')) {
                    echo gzencode($data);
                }
            } else {
                if ($compression == 'zip') {
                    if (@function_exists('gzcompress')) {
                        echo gzcompress($data);
                    }
                } // 3. on screen
                else {
                    echo $data;
                }
            }
        }
        exit;
    }

    /**
     * This function writes a binary file to the browser for download.
     *
     * @param string $file the local filename (the file you want to open
     *                         on the serverside)
     * @param string $filename the name the file will get when the user downloads it.
     * @param string $mimetype the mimetype of the file
     *
     * @return bool wether or not the export worked
     */
    public static function exportFile($file, $filename, $mimetype = '', $detectmime = true)
    {
        $browser = self::getBrowserInfo();
        if (preg_match('/ie/i', $browser['browser'])) {
            $mime = 'application/octetstream';
            $disp = 'attachment';
        } else {
            if (preg_match('/opera/i', $browser['browser'])) {
                $mime = 'application/octetstream';
                $disp = 'inline';
            } else {
                $mime = 'application/octet-stream';
                $disp = 'attachment';
            }
        }
        if ($mimetype != '') {
            $mime = $mimetype;
        } else {
            if ($mimetype == '' && $detectmime && function_exists('mime_content_type')) {
                $mime = mime_content_type($file);
            }
        }

        $fp = @fopen($file, 'rb');
        if ($fp != null) {
            header('Content-Type: '.$mime);
            header('Content-Length: '.filesize($file));
            header('Content-Disposition:  '.$disp.'; filename="'.$filename.'"');
            if (preg_match('/ie/i', $browser['browser'])) {
                header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
            }
            if (($_SERVER['SERVER_PORT'] == '443' || $_SERVER['HTTP_X_FORWARDED_PROTO'] === 'https') && preg_match('/msie/i', $_SERVER['HTTP_USER_AGENT'])) {
                header('Pragma: public');
            } else {
                header('Pragma: no-cache');
            }
            header('Expires: 0');

            header('Content-Description: File Transfer');
            header('Content-Transfer-Encoding: binary');

            fpassthru($fp);

            return true;
        }

        return false;
    }

    /**
     * Returns the (virtual) hostname of the server.
     *
     * @return string the hostname of the server
     */
    public static function atkHost()
    {
        $atkHost = $_SERVER['HTTP_HOST'] != '' ? $_SERVER['HTTP_HOST'] : $_SERVER['SERVER_NAME'];

        // if we're running on our cluster environment
        // we seem to have a specific portid within the HTTP_HOST
        // If so, remove it from the hostname

        $dummy = explode(':', $atkHost);

        return $dummy[0];
    }

    /**
     * Returns the next unique ID for the given sequence.
     * NOTE: ID's are only unique for the script execution!
     *
     * @param string $sequence the sequence name
     *
     * @return int next unique ID for the given sequence
     */
    public static function getUniqueId($sequence)
    {
        static $unique = [];
        if (!isset($unique[$sequence])) {
            $unique[$sequence] = 0;
        }

        return ++$unique[$sequence];
    }

    /**
     * Checks if the variable $var contains the given flag ($flag).
     *
     * @param string $var the variable which might contain flags
     * @param string $flag the flag you want to check for
     *
     * @return bool result of check
     */
    public static function hasFlag($var, $flag)
    {
        return ($var & $flag) == $flag;
    }

    /**
     * Makes an url from the target var and all postvars.
     *
     * @param string $target the path of the file to open
     *
     * @return string the url with the postvars
     */
    public static function makeUrlFromPostvars($target)
    {
        global $ATK_VARS;

        if (Tools::count($ATK_VARS)) {
            $url = $target.'?';
            foreach ($ATK_VARS as $key => $val) {
                $url .= $key.'='.rawurlencode($val).'&';
            }

            return $url;
        }

        return '';
    }

    /**
     * Makes an string with hidden input fields containing all posted vars.
     *
     * @param array $excludes array with the vars to exclude, default empty
     */
    public static function makeHiddenPostvars($excludes = array())
    {
        global $ATK_VARS;
        $str = '';

        if (Tools::count($ATK_VARS)) {
            foreach ($ATK_VARS as $key => $val) {
                if (!in_array($key, $excludes)) {
                    $inputs = [];
                    self::atkMakeHiddenPostVarsRecursion($key, $val, $inputs);
                    $str .= implode('', $inputs);
                }
            }

            return $str;
        }

        return '';
    }

    public static function atkMakeHiddenPostVarsRecursion($key, $val, &$inputs, $name = null)
    {
        if ($name == null) {
            $name = htmlentities($key);
        } else {
            $name .= '['.htmlentities($key).']';
        }

        if (is_array($val)) {
            foreach ($val as $rKey => $rVal) {
                self::atkMakeHiddenPostVarsRecursion($rKey, $rVal, $inputs, $name);
            }
        } else {
            $inputs[] = "<input type='hidden' name=\"".$name.'" value="'.htmlentities(strval($val))."\">\n";
        }
    }

    /**
     * Returns a string representation of an action status.
     *
     * @param int $status status of the action (ActionHandler::ACTION_FAILED|ActionHandler::ACTION_SUCCESS|ActionHandler::ACTION_CANCELLED)
     *
     * @return string String representation of an action status.
     */
    public static function atkActionStatus($status)
    {
        switch ($status) {
            case ActionHandler::ACTION_CANCELLED:
                return 'cancelled';
            case ActionHandler::ACTION_FAILED:
                return 'failed';
            case ActionHandler::ACTION_SUCCESS:
                return 'success';
        }

        return '';
    }

    /**
     * Build query string based on an array of parameters.
     *
     * @param array $params array of parameters
     * @param string $parent
     *
     * @return string query string
     */
    public static function buildQueryString($params, $parent = '')
    {
        $query = '';

        foreach ($params as $key => $value) {
            if (!empty($query)) {
                $query .= '&';
            }

            if (!empty($parent)) {
                $key = "{$parent}[{$key}]";
            }

            if (!is_array($value)) {
                $query .= "$key=".rawurlencode($value);
            } else {
                $query .= self::buildQueryString($value, $key);
            }
        }

        return $query;
    }

    /**
     * Generate a dispatch menu URL for use with nodes and their specific
     * actions.
     *
     *
     * @param string $nodeUri the $nodeUri
     * @param string $action the atk action the link will perform
     * @param array $params A key/value array with extra options for the url
     * @param string $phpfile The php file to use for dispatching
     *
     * @return string url for the node with the action
     */
    public static function dispatch_url($nodeUri, $action, $params = [], $phpfile = '')
    {
        $phpfile = ($phpfile != '') ?: Config::getGlobal('dispatcher');
        $atkParams = [];
        if ($nodeUri != '') {
            $atkParams['atknodeuri'] = $nodeUri;
        }
        if ($action != '') {
            $atkParams['atkaction'] = $action;
        }
        $params = array_merge($atkParams, $params);

        if ($params != '' && is_array($params) && Tools::count($params) > 0) {
            $phpfile .= '?'.self::buildQueryString($params);
        }

        return $phpfile;
    }

    /**
     * Generate a partial url.
     *
     * @param string $node the (module.)node name
     * @param string $action the atkaction
     * @param string $partial the partial name
     * @param array $params a key/value array with extra params
     * @param int $sessionStatus session status (default SessionManager::SESSION_PARTIAL)
     *
     * @return string url for the partial action
     */
    public static function partial_url(
        $node,
        $action,
        $partial,
        $params = [],
        $sessionStatus = SessionManager::SESSION_PARTIAL
    ) {
        if (!is_array($params)) {
            $params = [];
        }
        $params['atkpartial'] = $partial;
        $sm = SessionManager::getInstance();

        return $sm->sessionUrl(self::dispatch_url($node, $action, $params), $sessionStatus);
    }

    /**
     * Creates a session aware button.
     *
     * @param string $text the self::text to display on the button
     * @param string $url the url to use for the button
     * @param int $sessionstatus the session flags
     *                              (SessionManager::SESSION_DEFAULT (default)|SessionManager::SESSION_NEW|SessionManager::SESSION_REPLACE|
     *                              SessionManager::SESSION_NESTED|SessionManager::SESSION_BACK)
     * @param string $cssclass the css class the button should get
     *
     * @return string html button
     */
    public static function atkButton(
        $text,
        $url = '',
        $sessionstatus = SessionManager::SESSION_DEFAULT,
        $cssclass = ''
    ) {
        $sm = SessionManager::getInstance();
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/formsubmit.js');
        static $cnt = 0;

        if ($cssclass == '') {
            $cssclass = 'btn btn-default';
        }

        $cssclass = ' class="'.$cssclass.'"';
        $script = 'ATK.FormSubmit.atkSubmit("'.self::atkurlencode($sm->sessionUrl($url, $sessionstatus)).'")';
        $button = '<input type="button" name="atkbtn'.(++$cnt).'" value="'.$text.'" onClick=\''.$script.'\''.$cssclass.'>';

        return $button;
    }

    /**
     * Compares two assosiative multi dimensonal array's
     * if arrays differ, return true, otherwise it returns false.
     *
     * @param array $array1 original array
     * @param array $array2 new array
     *
     * @return bool wether or not the arrays differ
     */
    public static function atkArrayCompare($array1, $array2)
    {
        $difference = self::atkArrayDiff($array1, $array2);

        return !is_array($difference) ? false : true;
    }

    /**
     * Compares two assosiative multi dimensonal array's
     * if arrays differ, return differences, otherwise it returns false.
     *
     * @param array $array1 original array
     * @param array $array2 new array
     *
     * @return mixed differences or false if they do not differ
     */
    public static function atkArrayDiff($array1, $array2)
    {
        foreach ($array1 as $key => $value) {
            if (is_array($value)) {
                if (!is_array($array2[$key])) {
                    $difference[$key] = $value;
                } else {
                    $new_diff = self::atkArrayDiff($value, $array2[$key]);
                    if ($new_diff != false) {
                        $difference[$key] = $new_diff;
                    }
                }
            } elseif (!isset($array2[$key]) || $array2[$key] != $value) {
                $difference[$key] = $value;
            }
        }

        return !isset($difference) ? false : $difference;
    }

    /**
     * Recursive function that checks an array for values
     * because sometimes arrays will be filled with other empty
     * arrays and therefore still show up filled.
     *
     * WARNING: take care with using this function as it is recursive
     * and if you have a value linking back to it's self in one way or another,
     * you may spend a loooong time waiting on your application
     *
     * @param array $array The array that
     *
     * @return bool Wether or not we found anything
     */
    public static function atk_value_in_array($array)
    {
        if (is_array($array) && !empty($array)) {
            foreach ($array as $key => $value) {
                if (is_array($value)) {
                    if (self::atk_value_in_array($value)) {
                        return true;
                    }
                } else {
                    if ($value) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    /**
     * Recursive function to look if the needle exists in the haystack.
     *
     * WARNING: take care with using this function as it is recursive
     * and if you have a value linking back to it's self in one way or another,
     * you may spend a loooong time waiting on your application
     *
     * @param string $needle The value which will be searched in the haystack
     * @param array $haystack Array with values
     *
     * @return bool True if needle exists in haystack
     */
    public static function atk_in_array_recursive($needle, $haystack)
    {
        foreach ($haystack as $key => $value) {
            if ($value == $needle) {
                return true;
            } else {
                if (is_array($value)) {
                    if (self::atk_in_array_recursive($needle, $value)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    /**
     * Escapes the predefined characters.
     *
     * When there are predefined characters used this function will escape them
     * and returns right pattern.
     *
     * @param string $pattern Raw string to be escaped
     *
     * @return string Returns a pattern with the predefined pattern escaped
     */
    public static function escapeForRegex($pattern)
    {
        $escaped = '';
        $escapechars = array('/', '?', '"', '(', ')', "'", '*', '.', '[', ']');
        for ($counter = 0; $counter < strlen($pattern); ++$counter) {
            $curchar = substr($pattern, $counter, 1);
            if (in_array($curchar, $escapechars)) {
                $escaped .= '\\';
            }
            $escaped .= $curchar;
        }

        return $escaped;
    }

    /*
     * Returns the postvars
     * Returns a value or an array with all values
     */

    public static function atkGetPostVar($key = '')
    {
        if (empty($key) || $key == '') {
            return $_REQUEST;
        } else {
            if (array_key_exists($key, $_REQUEST) && $_REQUEST[$key] != '') {
                return $_REQUEST[$key];
            }

            return '';
        }
    }

    /**
     * ATK version of the PHP htmlentities function. Works just like PHP's
     * htmlentities function, but falls back to atkGetCharset() instead of
     * PHP's default charset, if no charset is given.
     *
     * @param string $string string to convert
     * @param int $quote_style quote style (defaults to ENT_COMPAT)
     * @param string $charset character set to use (default to atkGetCharset())
     *
     * @return string encoded string
     */
    public static function atk_htmlentities($string, $quote_style = ENT_COMPAT, $charset = null)
    {
        return self::htmlentities($string, $quote_style, $charset);
    }

    /**
     * ATK version of the PHP html_entity_decode function. Works just like PHP's
     * html_entity_decode function, but falls back to atkGetCharset() instead of
     * PHP's default charset, if no charset is given.
     *
     * @param string $string string to convert
     * @param int $quote_style quote style (defaults to ENT_COMPAT)
     * @param string $charset character set to use (default to atkGetCharset())
     *
     * @return string encoded string
     */
    public static function atk_html_entity_decode($string, $quote_style = ENT_COMPAT, $charset = null)
    {
        return self::html_entity_decode($string, $quote_style, $charset);
    }

    /**
     * Get string length.
     *
     * @param string $str The string being checked for length
     *
     * @return int
     */
    public static function atk_strlen($str)
    {
        return self::strlen($str);
    }

    /**
     * Get part of string.
     *
     * @param string $str The string being checked.
     * @param int $start The first position used in $str
     * @param int $length [optional] The maximum length of the returned string
     *
     * @return string
     */
    public static function atk_substr($str, $start, $length = '')
    {
        return self::substr($str, $start, $length);
    }

    /**
     *  Find position of first occurrence of string in a string.
     *
     * @param object $haystack The string being checked.
     * @param object $needle The position counted from the beginning of haystack .
     * @param object $offset [optional] The search offset. If it is not specified, 0 is used.
     *
     * @return int|bool
     */
    public static function atk_strpos($haystack, $needle, $offset = 0)
    {
        return self::strpos($haystack, $needle, $offset);
    }

    /**
     * Make a string lowercase.
     *
     * @param string $str The string being lowercased.
     *
     * @return string
     */
    public static function atk_strtolower($str)
    {
        return self::strtolower($str);
    }

    /**
     * Make a string uppercase.
     *
     * @param string $str The string being uppercased.
     *
     * @return string
     */
    public static function atk_strtoupper($str)
    {
        return self::strtoupper($str);
    }

    /**
     * Return the default charset, first we look if the
     * config_default_charset is set, else we use the
     * charset in the languge file;.
     *
     * @return string
     */
    public static function atkGetCharset()
    {
        return Config::getGlobal('default_charset', self::atktext('charset', 'atk'));
    }

    /**
     * Looks up a value using the given key in the given array and returns
     * the value if found or a default value if not found.
     *
     * @param array $array Array to be searched for key
     * @param string $key Key for which we are looking in array
     * @param mixed $defaultvalue Value we will return if key was not found in array
     *
     * @return mixed Value retrieved from array or default value if not found in array
     */
    public static function atkArrayNvl($array, $key, $defaultvalue = null)
    {
        return isset($array[$key]) ? $array[$key] : $defaultvalue;
    }

    /**
     * Returns the IP of the remote client.
     *
     * @return string ip address
     */
    public static function atkGetClientIp()
    {
        static $s_ip = null;

        if ($s_ip === null) {
            if (getenv('HTTP_CLIENT_IP')) {
                $s_ip = getenv('HTTP_CLIENT_IP');
            } elseif (getenv('HTTP_X_FORWARDED_FOR')) {
                $ipArray = explode(',', getenv('HTTP_X_FORWARDED_FOR'));
                $s_ip = $ipArray[0];
            } elseif (getenv('REMOTE_ADDR')) {
                $s_ip = getenv('REMOTE_ADDR');
            } else {
                $s_ip = 'x.x.x.x';
            }
        }

        return $s_ip;
    }

    /**
     * Function checks php version and clones the
     * given attribute in the right way.
     *
     * @param object $attribute The attribute to clone
     *
     * @return object $attr
     */
    public static function atkClone($attribute)
    {
        if (intval(substr(phpversion(), 0, 1)) < 5) {
            $attr = $attribute;
        } else {
            $attr = clone $attribute;
        }

        return $attr;
    }

    /**
     * ATK wrapper of the PHP iconv function. Check if iconv function is present in
     * the system. If yes - use it for converting string, if no - save string untouch
     * and make warning about it.
     *
     * @param string $in_charset from charset
     * @param string $out_charset to charset
     * @param string $str string to convert
     *
     * @return string encoded string
     */
    public static function atk_iconv($in_charset, $out_charset, $str)
    {
        return self::iconv($in_charset, $out_charset, $str);
    }

    /**
     * Returns the first argument that is not null.
     *
     * @param mixed ... arguments
     *
     * @return mixed first argument that is not null
     */
    public static function atkNvl()
    {
        for ($i = 0; $i < func_num_args(); ++$i) {
            $arg = func_get_arg($i);
            if (!is_null($arg)) {
                return $arg;
            }
        }

        return;
    }

    /**
     * Format date according to a format string, uses ATK's language files to translate
     * months, weekdays etc.
     *
     * @param $date    mixed  timestamp or date array (gotten with getdate())
     * @param $format  string format string, compatible with PHP's date format functions
     * @param $weekday bool always include day-of-week or not
     *
     * @return string formatted date
     */
    public static function atkFormatDate($date, $format, $weekday = false)
    {
        static $langcache = [];

        if (!is_array($date)) {
            $date = getdate($date);
        }

        /* format month */
        $format = str_replace('M', '%-%', $format);
        $format = str_replace('F', '%=%', $format);

        /* format day */
        $format = str_replace('D', '%&%', $format);
        $format = str_replace('l', '%*%', $format);

        if ($weekday && strpos($format, '%&%') === false && strpos($format, '%*%') === false) {
            $format = str_replace('d', '%*% d', $format);
            $format = str_replace('j', '%*% j', $format);
        }

        /* get date string */
        $str_date = adodb_date($format, $date[0]);

        $month = $date['month'];
        $shortmonth = substr(strtolower($date['month']), 0, 3);

        /* store the self::text calls */
        if (!isset($langcache[$month])) {
            $langcache[$month] = self::atktext(strtolower($month), 'atk');
        }

        if (!isset($langcache[$shortmonth])) {
            $langcache[$shortmonth] = self::atktext($shortmonth);
        }

        /* replace month/week name */
        $str_date = str_replace('%-%', $langcache[$shortmonth], $str_date);
        $str_date = str_replace('%=%', $langcache[$month], $str_date);
        $str_date = str_replace('%*%', self::atktext(strtolower($date['weekday']), 'atk'), $str_date);
        $str_date = str_replace('%&%', self::atktext(substr(strtolower($date['weekday']), 0, 3), 'atk'), $str_date);

        /* return string */

        return $str_date;
    }

    public static function getBrowserInfo($useragent = '')
    {
        $tmp = new BrowserInfo($useragent);

        return array(
            'ua' => $tmp->ua,
            'version' => $tmp->full_version,
            'browser' => $tmp->browser,
            'major' => $tmp->major,
            'minor' => $tmp->minor,
            'os' => $tmp->os,
            'platform' => $tmp->platform,
            'short' => $tmp->short,
            'brName' => $tmp->brName,
            'osName' => $tmp->osName,
            'hasGui' => $tmp->hasGui,
            'spider' => $tmp->spider,
            'family' => $tmp->family,
            'gecko' => $tmp->gecko,
        );
    }

    /**
     * Invoke a method on a class based on a string definition.
     * The string must be in the format
     * "packagename.subpackage.classname#methodname".
     *
     * @static
     *
     * @param string $str The "classname#method" to invoke.
     * @param array $params Any params to be passed to the invoked method.
     *
     * @return bool false if the call failed. In all other cases, it
     *              returns the output of the invoked method. (be
     *              careful with methods that return false).
     */
    public static function invokeFromString($str, $params = array())
    {
        if (strpos($str, '#') === false) {
            return false;
        }

        list($class, $method) = explode('#', $str);
        if ($class != '' && $method != '') {
            $handler = new $class();
            if (is_object($handler)) {
                return call_user_func_array(array($handler, $method), $params);
            }

            return false;
        } else {
            return false;
        }
    }

    /**
     * Makes a session-aware href url.
     * When using hrefs in the editform, you can set saveform to true. This will save your
     * form variables in the session and restore them whenever you come back.
     *
     * @param string $url the url to make session aware
     * @param string $name the name to display (will not be escaped!)
     * @param int $sessionstatus the session flags
     *                              (SessionManager::SESSION_DEFAULT (default)|SessionManager::SESSION_NEW|SessionManager::SESSION_REPLACE|
     *                              SessionManager::SESSION_NESTED|SessionManager::SESSION_BACK)
     * @param bool $saveform wether or not to save the form
     * @param string $extraprops extra props you can add in the link such as
     *                              'onChange="doSomething()"'
     * @static
     *
     * @return string the HTML link for the session aware URI
     */
    public static function href(
        $url,
        $name = '',
        $sessionstatus = SessionManager::SESSION_DEFAULT,
        $saveform = false,
        $extraprops = ''
    ) {
        $sm = SessionManager::getInstance();
        if ($saveform) {
            $str = 'ATK.FormSubmit.atkSubmit("'.self::atkurlencode($sm->sessionUrl($url, $sessionstatus)).'", true);';

            return '<a href="javascript:void(0)" onclick="'.htmlentities($str).'" '.$extraprops.'>'.$name.'</a>';
        } else {
            $str = $sm->sessionUrl($url, $sessionstatus);

            return '<a href="'.htmlentities($str).'" '.$extraprops.'>'.$name.'</a>';
        }
    }

    public static function redirect($location, $exit = true)
    {
        // The actual redirect.
        if (Config::getGlobal('debug') >= 2) {
            $debugger = Debugger::getInstance();
            $debugger->setRedirectUrl($location);
            self::atkdebug('Non-debug version would have redirected to <a href="'.htmlspecialchars($location).'">'.htmlspecialchars($location).'</a>', self::DEBUG_ESCAPED);
            if ($exit) {
                $output = Output::getInstance();
                $output->outputFlush();
                exit();
            }
        } else {
            self::atkdebug('redirecting to: '.htmlspecialchars($location));

            if (substr($location, -1) == '&') {
                $location = substr($location, 0, -1);
            }
            if (substr($location, -1) == '?') {
                $location = substr($location, 0, -1);
            }

            header('Location: '.$location);
            if ($exit) {
                exit();
            }
        }
    }

    /**
     * Gets the module of the node.
     *
     * @param string $nodeUri the node uri
     *
     * @return string the node's module
     */
    public static function getNodeModule($nodeUri)
    {
        $arr = explode('.', $nodeUri);
        if (Tools::count($arr) == 2) {
            return $arr[0];
        } else {
            return '';
        }
    }

    /**
     * Gets the node type of a node string.
     *
     * @param string $nodeUri the node uri
     *
     * @return string the node type
     */
    public static function getNodeType($nodeUri)
    {
        $arr = explode('.', $nodeUri);
        if (Tools::count($arr) == 2) {
            return $arr[1];
        } else {
            return $nodeUri;
        }
    }

    /**
     * Check if the system has multibyte support.
     *
     * @return bool
     */
    public static function hasMultiByteSupport()
    {
        if (self::$s_hasMultiByteSupport === null) {
            if (function_exists('mb_strlen') && Config::getGlobal('use_mbstring', true)) {
                mb_internal_encoding(self::atkGetCharset());
                self::$s_hasMultiByteSupport = true;
            } else {
                self::$s_hasMultiByteSupport = false;
            }
        }

        return self::$s_hasMultiByteSupport;
    }

    /**
     * Get string length.
     *
     * @param string $str The string being checked for length
     *
     * @return int
     */
    public static function strlen($str)
    {
        if (self::hasMultiByteSupport()) {
            return mb_strlen($str);
        } elseif (strtolower(self::atkGetCharset()) == 'utf-8') {
            preg_match_all('/./su', $str, $matches);
            $chars = $matches[0];

            return Tools::count($chars);
        } else {
            return strlen($str);
        }
    }

    /**
     * Get part of string.
     *
     * @param string $str The string being checked.
     * @param int $start The first position used in $str
     * @param int $length [optional] The maximum length of the returned string
     *
     * @return string
     */
    public static function substr($str, $start, $length = '')
    {
        if (self::hasMultiByteSupport()) {
            return mb_substr($str, $start, $length);
        } else {
            return substr($str, $start, $length);
        }
    }

    /**
     * Return char on given position.
     *
     * @param string $str The string being checked
     * @param int $pos The position of the char
     *
     * @return string
     */
    public static function charAt($str, $pos)
    {
        return self::substr($str, $pos, 1);
    }

    /**
     *  Find position of first occurrence of string in a string.
     *
     * @param string $haystack The string being checked.
     * @param string $needle The position counted from the beginning of haystack .
     * @param int $offset [optional] The search offset. If it is not specified, 0 is used.
     *
     * @return int|bool
     */
    public static function strpos($haystack, $needle, $offset = 0)
    {
        if (self::hasMultiByteSupport()) {
            return mb_strpos($haystack, $needle, $offset);
        } else {
            return substr($haystack, $needle, $offset);
        }
    }

    /**
     * Make a string lowercase.
     *
     * @param string $str The string being lowercased.
     *
     * @return string
     */
    public static function strtolower($str)
    {
        if (self::hasMultiByteSupport()) {
            return mb_strtolower($str);
        } else {
            return strtolower($str);
        }
    }

    /**
     * Make a string uppercase.
     *
     * @param string $str The string being uppercased.
     *
     * @return string
     */
    public static function strtoupper($str)
    {
        if (self::hasMultiByteSupport()) {
            return mb_strtoupper($str);
        } else {
            return strtoupper($str);
        }
    }

    /**
     * ATK version of the PHP html_entity_decode function. Works just like PHP's
     * html_entity_decode function, but falls back to the in the language file
     * configured charset instead of PHP's default charset, if no
     * charset is given.
     *
     * @param string $str string to convert
     * @param int $quote_style quote style (defaults to ENT_COMPAT)
     * @param string $charset character set to use (default to Tools::atktext('charset', 'atk'))
     *
     * @return string encoded string
     */
    public static function html_entity_decode($str, $quote_style = ENT_COMPAT, $charset = null)
    {
        if ($charset === null) {
            $charset = self::atkGetCharset();
        }

        // check if charset is allowed, else use default charset for this function
        if (!in_array(strtolower($charset), self::$s_acceptedCharsets)) {
            $charset = 'iso-8859-1';
        }

        return html_entity_decode($str, $quote_style, $charset);
    }

    /**
     * ATK version of the PHP htmlentities function. Works just like PHP's
     * htmlentities function, but falls back to Tools::atkGetCharset() instead of
     * PHP's default charset, if no charset is given.
     *
     * @param string $str string to convert
     * @param int $quote_style quote style (defaults to ENT_COMPAT)
     * @param string $charset character set to use (default to Tools::atkGetCharset())
     *
     * @return string encoded string
     */
    public static function htmlentities($str, $quote_style = ENT_COMPAT, $charset = null)
    {
        if ($charset === null) {
            $charset = self::atkGetCharset();
        }

        // check if charset is allowed, else use default charset for this function
        if (!in_array(strtolower($charset), self::$s_acceptedCharsets)) {
            $charset = 'iso-8859-1';
        }

        return htmlentities($str, $quote_style, $charset);
    }

    /**
     * Return an identifier suitable for html name attribute
     *
     * constraints : « begin with a letter ([A-Za-z]) and may be followed by
     * any number of letters, digits ([0-9]), hyphens ("-"), underscores ("_"),
     * colons (":") » (https://www.w3.org/TR/html4/types.html#type-id)
     * periods are excluded because PHP replace them with underscores.
     *
     * Note : this encoding is not reversible.
     *
     * @param string $name The name you wish to convert. If it already fulfill
     *                     constraints, the same string will be returned.
     *
     * @return string the HTML identifier.
     */
    public static function htmlName(string $name) : string
    {
        // Valid characters constraints :
        $htmlName = preg_replace('/[^A-Za-z0-9_:\-]/', '_', $name);
        // If we replaced some characters, then we append a checksum part to it
        // to avoid that '首页' and '典范' return the same name.
        if ($htmlName != $name) {
            $htmlName .= '_'.substr(md5($name), 0, 8);
        }
        // "Begin with a letter" constraints :
        $firstChar = substr($htmlName, 0, 1);
        if (!(($firstChar >= 'a' and $firstChar <= 'z') or ($firstChar >= 'A' and $firstChar <= 'Z'))) {
            $htmlName = 'a'.$htmlName;
        }
        return $htmlName;
    }

    /**
     * ATK version of the PHP html_entity_decode function. Works just like PHP's
     * html_entity_decode function, but falls back to Tools::atkGetCharset() instead of
     * PHP's default charset, if no charset is given.
     *
     * @param string $in_charset The input charset
     * @param string $out_charset The output charset
     * @param string $str The string to convert
     *
     * @return string encoded string
     */
    public static function iconv($in_charset, $out_charset, $str)
    {
        if (function_exists('iconv')) {
            $str = iconv($in_charset, $out_charset, $str);
        } else {
            self::atkwarning(self::atktext('error_iconv_not_install'));
        }

        return $str;
    }

    /**
     * ATK version of the Smarty truncate function, multibyte safe.
     *
     * @param string $string text to truncate
     * @param int $max Maximum length of the total result string
     * @param string $replace text to append to the end of the truncated string
     *
     * @return string truncated sting
     */
    public static function truncate($string, $max, $replace)
    {
        if (self::strlen($string) <= $max) {
            return $string;
        } else {
            $length = $max - self::strlen($replace);

            return self::substr($string, 0, $length).$replace;
        }
    }

    /**
     * Count $value items.
     *
     * Does not use is_iterable() because >= php7.1
     *
     * @param $value
     *
     * @return int number of items. Zero if $value is not iterable.
     */
    public static function count($value)
    {
        if(is_array($value) || $value instanceof \Countable){
            return count($value);
        }
        return 0;
    }
}
