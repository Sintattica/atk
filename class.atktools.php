<?php
/**
 * This file is part of the ATK distribution on GitHub.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 *
 * This file contains a set of general-purpose utility functions.
 * @todo Move all these functions to relevant classes.
 * @todo Document all of the functions
 *
 * @copyright (c)2000-2007 Ibuildings.nl BV
 * @copyright (c)2000-2007 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
/**
 * Converts applicable characters to html entities so they aren't
 * interpreted by the browser.
 */
define("DEBUG_HTML", 1);

/**
 * Wraps the self::text into html bold tags in order to make warnings more
 * clearly visible.
 */
define("DEBUG_WARNING", 2);

/**
 * Hides the debug unless in level 2.
 * This should be used for debug you only really want to see if you
 * are developing (like deprecation warnings).
 */
define("DEBUG_NOTICE", 4);

/**
 * Error message.
 */
define("DEBUG_ERROR", 8);


class Atk_Tools
{

    /**
     * Function self::atkErrorHandler
     * This function catches PHP parse errors etc, and passes
     * them to self::atkerror(), so errors can be mailed and output
     * can be regulated.
     * This funtion must be registered with set_error_handler("self::atkErrorHandler");
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
            E_STRICT => "Strict Notice"
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
        } else if ($errtype == E_NOTICE) {
            // Just show notices
            self::atkdebug("[$errortypestring] $errstr in $errfile (line $errline)", DEBUG_NOTICE);
            return;
        } else if (defined('E_DEPRECATED') && ($errtype & (E_DEPRECATED | E_USER_DEPRECATED)) > 0) {
            // Just show deprecation warnings in the debug log, but don't influence the program flow
            self::atkdebug("[$errortypestring] $errstr in $errfile (line $errline)", DEBUG_NOTICE);
            return;
        } else if (($errtype & (E_WARNING | E_USER_WARNING)) > 0) {
            // This is something we should pay attention to, but we don't need to die.
            self::atkerror("[$errortypestring] $errstr in $errfile (line $errline)");
            return;
        } else {
            self::atkerror("[$errortypestring] $errstr in $errfile (line $errline)");

            // we must die. we can't even output anything anymore..
            // we can do something with the info though.
            self::handleError();
            Atk_Output::getInstance()->outputFlush();
            die;
        }
    }

    /**
     * Default ATK exception handler, handles uncaught exceptions and calls atkHalt.
     *
     * @param Exception $exception uncaught exception
     */
    public static function atkExceptionHandler(Exception $exception)
    {
        self::atkdebug($exception->getMessage(), DEBUG_ERROR);
        self::atkdebug("Trace:<br/>" . nl2br($exception->getTraceAsString()), DEBUG_ERROR);
        self::atkhalt("Uncaught exception: " . $exception->getMessage(), 'critical');
    }

    /**
     * Function self::atkhalt
     * Halts on critical errors and also on warnings if specified in the config file.
     * @param string $msg The message to be displayed
     * @param string $level The level of the error,
     *                      ("critical"|"warning" (default))
     * @return bool false if something goes horribly wrong
     */
    public static function atkhalt($msg, $level = "warning")
    {
        if ($level == $GLOBALS['config_halt_on_error'] || $level == "critical") {
            if ($level == "warning") {
                $level_color = "#0000ff";
            } else {
                // critical
                $level_color = "#ff0000";
            }

            if (php_sapi_name() == 'cli') {
                $res = self::atktext($level, "atk") . ': ' . $msg . "\n";
            } else {
                $res = "<html>";
                $res .= '<body bgcolor="#ffffff" color="#000000">';
                $res .= "<font color=\"$level_color\"><b>" . self::atktext($level, "atk") . "</b></font>: $msg.<br />\n";
            }

            Atk_Output::getInstance()->output($res);
            Atk_Output::getInstance()->outputFlush();
            exit("Halted...\n");
        } else {
            self::atkerror("$msg");
        }
        return false;
    }

    /**
     * @deprecated Use Atk_Debugger::getMicroTime()
     * @return int the microtime
     */
    public static function getmicrotime()
    {
        self::atkimport("atk.utils.atkdebugger");
        return Atk_Debugger::getMicroTime();
    }

    /**
     * @deprecated Use Atk_Debugger::elapsed();
     *
     * @return string elapsed time in microseconds
     */
    public static function elapsed()
    {
        self::atkimport("atk.utils.atkdebugger");
        return Atk_Debugger::elapsed();
    }


    /**
     * Function atkdebug
     *
     * Adds debug self::text to the debug log
     * @param String $txt The self::text that will be added to the log
     * @param Integer $flags An optional combination of DEBUG_ flags
     */
    public static function atkdebug($txt, $flags = 0)
    {
        global $g_debug_msg;
        $level = Atk_Config::getGlobal("debug");
        if ($level >= 0) {
            if (self::hasFlag($flags, DEBUG_HTML))
                $txt = htmlentities($txt);
            if (self::hasFlag($flags, DEBUG_WARNING))
                $txt = "<b>" . $txt . "</b>";

            $line = self::atkGetTimingInfo() . $txt;
            self::atkWriteLog($line);

            if (self::hasFlag($flags, DEBUG_ERROR)) {
                $line = '<span class="atkDebugError">' . $line . '</span>';
            }

            if ($level > 2) {
                self::atkimport("atk.utils.self::atkdebugger");
                if (!Atk_Debugger::addStatement($line)) {
                    $g_debug_msg[] = $line;
                }
            } else if (!self::hasFlag($flags, DEBUG_NOTICE)) {
                $g_debug_msg[] = $line;
            }
        } else if ($level > -1) { // at 0 we still collect the info so we
            // have it in error reports. At -1, we don't collect
            $g_debug_msg[] = $txt;
        }
    }

    /**
     * Send a notice to the debug log.
     * A notice doesn't get show unless your debug level is 3 or higher.
     *
     * @param String $txt The self::text that will be added to the log
     */
    public static function atknotice($txt)
    {
        self::atkdebug($txt, DEBUG_NOTICE);
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
        self::atkdebug($txt, DEBUG_WARNING);
    }

    public static function atkGetTimingInfo()
    {
        return "[" . self::elapsed() . (Atk_Config::getGlobal('debug') > 0 && function_exists("memory_get_usage")
            ? " / " . sprintf("%02.02f", (memory_get_usage() / 1024 / 1024)) . "MB"
            : "") . "] ";
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
     */
    public static function atkerror($error)
    {
        global $g_error_msg;

        if ($error instanceof Exception) {
            $g_error_msg[] = "[" . self::elapsed() . "] " . $error->getMessage();
            self::atkdebug(nl2br($error->getMessage() . "\n" . $error->getTraceAsString()), DEBUG_ERROR);
        } else {
            $g_error_msg[] = "[" . self::elapsed() . "] " . $error;
            self::atkdebug($error, DEBUG_ERROR);
        }

        if (function_exists('debug_backtrace')) {
            self::atkdebug("Trace:" . self::atk_get_trace(), DEBUG_ERROR);
        }

        if (Atk_Config::getGlobal('throw_exception_on_error') && $error instanceof Exception) {
            throw $error;
        } else if (Atk_Config::getGlobal('throw_exception_on_error')) {
            throw new Exception($error);
        }
    }

    /**
     * Returns a trace-route from all functions where-through the code has been executed
     *
     * @param string $format (html|plaintext)
     * @return string Backtrace in html or plaintext format
     */
    public static function atkGetTrace($format = "html")
    {
        // Return if the debug_backtrace function doesn't exist
        if (!function_exists("debug_backtrace"))
            return "Incorrect php-version for self::atk_get_trace()";

        // Get the debug backtrace
        $traceArr = debug_backtrace();

        // Remove the call of self::atk_get_trace
        array_shift($traceArr);

        // Start with an empty result;
        $ret = "";

        $theSpacer = "";

        // Loop through all items found in the backtrace
        for ($i = 0, $_i = count($traceArr); $i < $_i; $i++) {
            //for($i=count($traceArr)-1; $i >= 0; $i--)
            // Skip this item in the backtrace if empty
            if (empty($traceArr[$i]))
                continue;

            // Don't display an self::atkerror statement itself.
            if ($traceArr[$i]["function"] == "self::atkerror")
                continue;

            // Read the source location
            if (isset($traceArr[$i]["file"]))
                $location = $traceArr[$i]["file"] . (isset($traceArr[$i]["line"]) ? sprintf(", line %d", $traceArr[$i]["line"])
                        : "[Unknown line]");
            else
                $location = "[PHP KERNEL]";

            // Read the statement
            if (isset($traceArr[$i]["class"])) {
                $statement = $traceArr[$i]["class"];
                if (isset($traceArr[$i]["type"]))
                    $statement .= $traceArr[$i]["type"];
            } else {
                $statement = "";
            }
            $statement .= $traceArr[$i]["function"];

            // Initialize the functionParamArr array
            $functionParamArr = array();

            // Parse any arguments into the array
            if (isset($traceArr[$i]["args"])) {
                foreach ($traceArr[$i]["args"] as $val) {
                    if (is_array($val)) {
                        $valArr = array();
                        foreach ($val as $name => $value) {
                            if (is_numeric($name))
                                $valArr[] = $name;
                            else {
                                if (is_object($value))
                                    $valArr[] = sprintf("%s=Object(%s)", $name, get_class($value));
                                else
                                    $valArr[] = $name . "=" . @json_encode($value);
                            }
                        }
                        $stringval = "array(" . implode(", ", $valArr) . ")";
                    } else if (is_null($val))
                        $stringval = 'null';
                    else if (is_object($val))
                        $stringval = sprintf("Object(%s)", get_class($val));
                    else if (is_bool($val))
                        $stringval = $val ? 'true' : 'false';
                    else {
                        if (strlen($val . $theSpacer) > 103)
                            $stringval = '"' . substr($val, 0, 100 - strlen($theSpacer)) . '"...';
                        else
                            $stringval = '"' . $val . '"';
                    }
                    $functionParamArr[] = $theSpacer . "  " . $stringval;
                }
            }
            $functionParams = implode(",\n", $functionParamArr);

            $ret .= $theSpacer . "@" . $location . "\n";
            $ret .= $theSpacer . $statement;
            $ret .= (strlen($functionParams) ? "\n" . $theSpacer . "(\n" . $functionParams . "\n" . $theSpacer . ")"
                    : "()") . "\n";

            // Add indentation
            $theSpacer .= "  ";
        }

        // If html format should be used, replace the html special chars with html entities and put the backtrace within preformat tags.
        if ($format == "html")
            $ret = "<pre>" . htmlspecialchars($ret) . "</pre>";

        // Return the generated trace
        return $ret;
    }

    /**
     * @deprecated Use atkGetTrace instead
     */
    public static function atk_get_trace($format = "html")
    {
        return self::atkGetTrace($format);
    }

    /**
     * Writes info to a given file.
     * Useful for writing to any log files.
     * @param String $text self::text to write to the logfile
     * @param String $file the file name
     */
    public static function atkWriteToFile($text, $file = "")
    {
        $fp = @fopen($file, "a");
        if ($fp) {
            fwrite($fp, $text . "\n");
            fclose($fp);
        }
    }

    /**
     * Writes info to the optional debug logfile.
     * Please notice this feature will heavily decrease the performance
     * and should therefore only be used for debugging and development
     * purposes.
     * @param String $text self::text to write to the logfile
     */
    public static function atkWriteLog($text)
    {
        if (Atk_Config::getGlobal("debug") > 0 && Atk_Config::getGlobal("debuglog")) {
            self::atkWriteToFile($text, Atk_Config::getGlobal("debuglog"));
        }
    }

    /**
     * Replaces the [vars] with the values from the language files
     * Please note that it is important, for performance reasons,
     * that you pass along the module where the language files can be found
     * @param mixed $string string or array of strings containing the name(s) of the string to return
     *                                when an array of strings is passed, the second will be the fallback if
     *                                the first one isn't found, and so forth
     * @param String $module module in which the language file should be looked for,
     *                                defaults to core module with fallback to ATK
     * @param String $node the node to which the string belongs
     * @param String $lng ISO 639-1 language code, defaults to config variable
     * @param String $firstfallback the first module to check as part of the fallback
     * @param boolean $nodefaulttext if true, then it doesn't return a default self::text
     *                                when it can't find a translation
     * @return String the string from the languagefile
     * @deprecated Use self::atktext instead
     */
    /*
    public static function text($string, $node = "", $module = "", $lng = "", $firstfallback = "", $nodefaulttext = false)
    {
        self::atkdebug("Call to deprecated self::text() function", DEBUG_WARNING);
        self::atkimport("atk.atklanguage");
        return Atk_Language::text($string, $module, $node, $lng, $firstfallback, $nodefaulttext);
    }
    */

    /**
     * Replaces the [vars] with the values from the language files
     * Please note that it is important, for performance reasons,
     * that you pass along the module where the language files can be found
     * @param mixed $string string or array of strings containing the name(s) of the string to return
     *                                when an array of strings is passed, the second will be the fallback if
     *                                the first one isn't found, and so forth
     * @param String $module module in which the language file should be looked for,
     *                                defaults to core module with fallback to ATK
     * @param String $node the node to which the string belongs
     * @param String $lng ISO 639-1 language code, defaults to config variable
     * @param String $firstfallback the first module to check as part of the fallback
     * @param boolean $nodefaulttext if true, then it doesn't return a default self::text
     *                                when it can't find a translation
     * @param boolean $modulefallback Wether or not to use all the modules of the application in the fallback,
     *                                when looking for strings
     * @return String the string from the languagefile
     */
    public static function atktext($string, $module = "", $node = "", $lng = "", $firstfallback = "", $nodefaulttext = false, $modulefallback = false)
    {
        self::atkimport("atk.atklanguage");
        return Atk_Language::text($string, $module, $node, $lng, $firstfallback, $nodefaulttext, $modulefallback);
    }


    /**
     * @deprecated Use Atk_SessionManager::formState instead.
     */
    public static function session_form($sessionstatus = SESSION_DEFAULT, $returnbehaviour = null, $fieldprefix = '')
    {
        return Atk_SessionManager::formState($sessionstatus, $returnbehaviour, $fieldprefix);
    }

    /**
     * @deprecated Use Atk_SessionManager::sessionVars() instead.
     */
    public static function session_vars($sessionstatus = SESSION_DEFAULT, $levelskip = null, $url = "")
    {
        return Atk_SessionManager::sessionVars($sessionstatus, $levelskip, $url);
    }

    /**
     * @deprecated use Atk_SessionManager::sessionUrl() instead.
     */
    public static function session_url($url, $sessionstatus = SESSION_DEFAULT, $levelskip = null)
    {
        return Atk_SessionManager::sessionUrl($url, $sessionstatus, $levelskip);
    }

    /**
     * @deprecated use self::atkHref or Atk_SessionManager::href instead.
     */
    public static function  href($url, $name = "", $sessionstatus = SESSION_DEFAULT, $saveform = false, $extraprops = "")
    {
        return Atk_SessionManager::href($url, $name, $sessionstatus, $saveform, $extraprops);
    }

    /**
     * Convenience wrapper for Atk_SessionManager::href().
     * @see Atk_SessionManager::href
     */
    public static function atkHref($url, $name = "", $sessionstatus = SESSION_DEFAULT, $saveform = false, $extraprops = "")
    {
        return Atk_SessionManager::href($url, $name, $sessionstatus, $saveform, $extraprops);
    }

    /**
     * array_merge without duplicates
     * Supports unlimited number of arrays as arguments.
     *
     * @param array $array1 <p>
     * Initial array to merge.
     * </p>
     * @param array $array2 [optional]
     * @param array $_ [optional]
     * @return Array The result of the merge between $array1 and $array2
     */
    public static function atk_array_merge(array $array1, array $array2 = null, array $_ = null)
    {
        return array_unique(call_user_func_array("array_merge", func_get_args()), SORT_REGULAR);
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

        $result = array();

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
     * @param Array $array unlimited number of arrays
     * @return Array The result of the merge between the given arrays
     */
    public static function atk_array_merge_keys()
    {
        $args = func_get_args();
        $result = array();
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
     * @param Array $haystack The array to search.
     * @param boolean $strict If true, type must match.
     * @return bool wether or not the value is in the array
     */
    public static function atk_in_array($needle, $haystack, $strict = false)
    {
        return (is_array($haystack) && in_array($needle, $haystack, $strict));
    }

    /**
     * Function dataSetContains
     *
     * Checks if a value is in a Array
     * @param array $set the array
     * @param var $key the key in the array as in $array[$key]
     * @param var $value the value we are looking for
     * @return bool wether or not the value is in the array
     */
    public static function dataSetContains($set, $key, $value)
    {
        for ($i = 0; $i < count($set); $i++) {
            if ($set[$i][$key] == $value)
                return true;
        }
        return false;
    }

    /**
     * Strips ' or  " from the begin and end of a string (only if they are
     * on both sides, e.g. foo' remains foo' but 'bar' becomes bar.
     * @param string $string the string we need to strip
     * @return string the stripped string
     */
    public static function stripQuotes($string)
    {
        $temp = trim($string);
        if (substr($temp, 0, 1) == "'" && substr($temp, -1) == "'")
            return substr($temp, 1, -1);
        if (substr($temp, 0, 1) == '"' && substr($temp, -1) == '"')
            return substr($temp, 1, -1);
        return $string;
    }

    /**
     * Translates a string like id='3' into Array("id"=>3)
     * @param string $pair the string which is to be decoded
     * @return array the decoded array
     */
    public static function decodeKeyValuePair($pair)
    {
        $operators = array("==", "!=", "<>", ">=", "<=", "=", "<", ">");

        static $s_regex = null;
        if ($s_regex === null) {
            $s_regex = '/' . implode('|', array_map('preg_quote', $operators)) . '/';
        }

        list($key, $value) = preg_split($s_regex, $pair);

        return array($key => self::stripQuotes($value));
    }

    /**
     * Translates a string like id=3 AND name='joe' into Array("id"=>3,"name"=>"joe")
     * @todo we should also support <=>, >=, >, <=, <, <>
     * @param string $set the string to decode
     * @return array the decoded array
     */
    public static function decodeKeyValueSet($set)
    {
        $result = array();
        $items = explode(" AND ", $set);
        for ($i = 0; $i < count($items); $i++) {
            $items[$i] = trim($items[$i], '()'); // trim parenthesis if present, e.g. (id=3) AND (name='joe')
            if (strstr($items[$i], '!=') !== false) {
                list($key, $value) = explode("!=", $items[$i]);
                $result[trim($key)] = self::stripQuotes($value);
            } elseif (strstr($items[$i], '=') !== false) {
                list($key, $value) = explode("=", $items[$i]);
                $result[trim($key)] = self::stripQuotes($value);
            } elseif (stristr($items[$i], 'IS NULL') !== false) {
                list($key) = preg_split('/IS NULL/i', $items[$i]);
                $result[trim($key)] = NULL;
            }
        }
        return $result;
    }

    /**
     * Translates Array("id"=>3,"name"=>"joe") into a string like id='3 AND name='joe''
     * @param array $set the array to be encoded
     * @return string the encoded string
     */
    public static function encodeKeyValueSet($set)
    {
        reset($set);
        $items = Array();
        while (list($key, $value) = each($set)) {
            $items[] = $key . "=" . $value;
        }
        return implode(" AND ", $items);
    }

    /**
     * Same as strip_slashes from php, but if the passed value is an array,
     * all elements of the array are stripped. Recursive function.
     * @param var &$var the value/array to strip the slashes of
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
     * @param array &$vars the array to be stripped and translated
     */
    public static function atkDataDecode(&$vars)
    {
        $magicQuotes = get_magic_quotes_gpc();

        foreach (array_keys($vars) as $varname) {
            $value = &$vars[$varname];
            // We must strip all slashes from the input, since php puts slashes
            // in front of quotes that are passed by the url. (magic_quotes_gpc)
            if ($value !== NULL && $magicQuotes)
                self::atk_stripslashes($value);

            self::AE_decode($vars, $varname);

            if (strpos(strtoupper($varname), '_AMDAE_') > 0) { // Now I *know* that strpos could return 0 if _AMDAE_ *is* found
                // at the beginning of the string.. but since that's not a valid
                // encoded var, we do nothing with it.
                // This string is encoded.
                list($dimension1, $dimension2) = explode("_AMDAE_", strtoupper($varname));
                if (is_array($value)) {
                    // Multidimensional thing
                    for ($i = 0; $i < count($value); $i++) {
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
     *  echo $dest["a"]["b"]["c"]; <- this will display 3
     * @param array &$dest the array to put the decoded var in
     * @param string $var the var to decode
     */
    public static function AE_decode(&$dest, $var)
    {
        $items = explode("_AE_", $var);
        if (count($items) <= 1)
            return;

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
     * Get the [ ] Fields out of a String
     * @deprecated please use the atkStringParser class
     */
    public static function stringfields($string)
    {
        self::atkdebug("Warning: deprecated use of self::stringfields(). Use atkStringParser class instead");
        $tmp = "";
        $adding = false;
        $fields = array();
        for ($i = 0; $i < strlen($string); $i++) {
            if ($string[$i] == "]") {
                $adding = false;
                $fields[] = $tmp;
                $tmp = "";
            } else if ($string[$i] == "[") {
                $adding = true;
            } else {
                if ($adding)
                    $tmp .= $string[$i];
            }
        }

        return $fields;
    }

    /**
     * Parse strings
     * @deprecated please use the atkStringParser class
     */
    public static function stringparse($string, $data, $encode = false)
    {
        self::atkdebug("Warning: deprecated use of stringparse(). Use atkStringParser class instead");
        $fields = self::stringfields($string);
        for ($i = 0; $i < count($fields); $i++) {
            $elements = explode(".", $fields[$i]);
            $databin = $data;
            for ($j = 0; $j < count($elements); $j++) {
                if (array_key_exists($elements[$j], $databin)) {
                    $value = $databin[$elements[$j]];
                    $databin = $databin[$elements[$j]];
                }
            }
            if ($encode) {
                $string = str_replace("[" . $fields[$i] . "]", rawurlencode($value), $string);
            } else {
                $string = str_replace("[" . $fields[$i] . "]", $value, $string);
            }
        }
        return $string;
    }

    /**
     * Safe urlencode function. Note, you can reencode already encoded strings, but
     * not more than 9 times!
     * If you encode a string more than 9 times, you won't be able to decode it
     * anymore
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
     * @return string the encoded url
     */
    public static function atkurlencode($string, $pref = true)
    {
        $string = rawurlencode($string);
        for ($i = 8; $i >= 1; $i--) {
            $string = str_replace("_" . $i, "_" . ($i + 1), $string);
        }
        return ($pref ? "__" : "") . str_replace("%", "_1", $string);
    }

    public static function atkurldecode($string)
    {
        if (substr($string, 0, 2) != "__")
            return $string;
        else {
            $string = str_replace("_1", "%", substr($string, 2));
            for ($i = 1; $i <= 8; $i++) {
                $string = str_replace("_" . ($i + 1), "_" . $i, $string);
            }
            return rawurldecode($string);
        }
    }


    /**
     * Send a detailed error report to the maintainer.
     *
     */
    public static function mailreport()
    {
        global $g_error_msg, $g_debug_msg;
        include_once(Atk_Config::getGlobal('atkroot') . 'atk/errors/class.atkerrorhandlerbase.php');
        $errorHandlerObject = Atk_ErrorHandlerBase::get('mail', array('mailto' => Atk_Config::getGlobal('mailreport')));
        $errorHandlerObject->handle($g_error_msg, $g_debug_msg);
    }

    /**
     * Handle errors that occurred in ATK, available handlers from /atk/errors/ can be added to
     * the error_handlers config.
     *
     */
    public static function handleError()
    {
        global $g_error_msg, $g_debug_msg;
        include_once(Atk_Config::getGlobal('atkroot') . 'atk/errors/class.atkerrorhandlerbase.php');
        $errorHandlers = Atk_Config::getGlobal('error_handlers', array('mail' => array('mailto' => Atk_Config::getGlobal('mailreport'))));
        foreach ($errorHandlers as $key => $value) {
            if (is_numeric($key))
                $key = $value;
            $errorHandlerObject = Atk_ErrorHandlerBase::get($key, $value);
            $errorHandlerObject->handle($g_error_msg, $g_debug_msg);
        }
    }

    /**
     * Wrapper for escapeSQL function
     * @param String $string The string to escape.
     * @param boolean $wildcard Set to true to convert wildcard chars ('%').
     *                          False (default) will leave them unescaped.
     * @return String A SQL compatible version of the input string.
     */
    public static function escapeSQL($string, $wildcard = false)
    {
        $db = self::atkGetDb();
        return $db->escapeSQL($string, $wildcard);
    }

    /**
     * Return the atk version number.
     * @return string the version number of ATK
     */
    public static function atkversion()
    {
        include_once("version.php");
        return $atk_version;
    }

    /**
     * Convenience wrapper for Atk_Db::getInstance()
     * @param String $conn The name of the connection to retrieve
     * @return Atk_Db Database connection instance
     */
    public static function &atkGetDb($conn = 'default', $reset = false, $mode = "r")
    {
        self::atkimport("atk.db.atkdb");
        $db = &Atk_Db::getInstance($conn, $reset, $mode);
        return $db;
    }

    /**
     * Returns a url to open a popup window
     * @param string $target the target of the popup
     * @param string $params extra params to pass along
     * @param string $winName the name of the window
     * @param int $width the width of the popup
     * @param int $height the height of the popup
     * @param string $scroll allow scrolling? (no (default)|yes)
     * @param string $resize allow resizing? (no (default)|yes)
     * return string the url for the popup window
     */
    public static function atkPopup($target, $params, $winName, $width, $height, $scroll = 'no', $resize = 'no')
    {
        $url = self::session_url("include.php?file=" . $target . "&" . $params, SESSION_NESTED);
        $popupurl = "javascript:NewWindow('" . $url . "','" . $winName . "'," . $height . "," . $width . ",'" . $scroll . "','" . $resize . "')";
        return $popupurl;
    }

    /**
     * Adds new element to error array en $record. When
     * $msg is empty the multilange error string is used.
     * @param array &$rec var in which to add element to error array
     * @param var $attrib attributename or an array with attribute names
     * @param string $err multilanguage error string
     * @param string $msg optinal error string
     */
    public static function triggerError(&$rec, $attrib, $err, $msg = "", $tab = "", $label = '', $module = 'atk')
    {
        if ($msg == "") {
            $msg = self::atktext($err, $module);
        }
        $rec['atkerror'][] = array("attrib_name" => $attrib, "err" => $err, "msg" => $msg, "tab" => $tab, "label" => $label);
    }

    /**
     * Adds new element to the record error array. When no message
     * is given the multi-language error string is used.
     *
     * @param array $record record
     * @param atkAttribute|array $attrib attribute or array of attributes
     * @param string $error multi-language error string
     * @param string $message error message (optional)
     */
    public static function atkTriggerError(&$record, $attrib, $error, $message = '')
    {
        if (is_array($attrib)) {
            $attribName = array();
            $label = array();

            for ($i = 0; $i < count($attrib); $i++) {
                $attribName[$i] = $attrib[$i]->fieldName();
                $label[$i] = $attrib[$i]->label($record);
            }

            $tab = $attrib[0]->m_tabs[0];
            if (!$message) {
                $message = $attrib[0]->text($error);
            }
        } else {
            $attribName = $attrib->fieldName();
            $label = $attrib->label($record);
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
     * @param $a data to be displayed
     * @param $d name of the data that's being displayed.
     */
    public static function atk_var_dump($a, $d = "")
    {
        ob_start();
        var_dump($a);
        $data = ob_get_contents();
        self::atkdebug("vardump: " . ($d != "" ? $d . " = " : "") . "<pre>" . $data . "</pre>");
        ob_end_clean();
    }

    /**
     * This function writes data to the browser for download.
     * $data is the data to download.
     * $filename is the name the file will get when the user downloads it.
     * $compression can be "zip", "gzip" or "bzip", which causes the data
     *              to be compressed before transmission.
     */
    public static function exportData($data, $filename, $compression = "")
    {
        $browser = self::getBrowserInfo();
        if (preg_match("/ie/i", $browser["browser"])) {
            $mime = "application/octetstream";
            $disp = 'inline';
        } else if (preg_match("/opera/i", $browser["browser"])) {
            $mime = "application/octetstream";
            $disp = 'attachment';
        } else {
            $mime = "application/octet-stream";
            $disp = 'attachment';
        }

        if ($compression == "bzip") {
            $mime = 'application/x-bzip';
            $filename .= ".bz2";
        } else if ($compression == "gzip") {
            $mime = 'application/x-gzip';
            $filename .= ".gz";
        } else if ($compression == "zip") {
            $mime = 'application/x-zip';
            $filename .= ".zip";
        }

        header('Content-Type: ' . $mime);
        header('Content-Disposition:  ' . $disp . '; filename="' . $filename . '"');
        if (preg_match("/ie/i", $browser["browser"]))
            header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
        header('Pragma: no-cache');
        header('Expires: 0');

        // 1. as a bzipped file
        if ($compression == "bzip") {
            if (@function_exists('bzcompress')) {
                echo bzcompress($data);
            }
        } // 2. as a gzipped file
        else if ($compression == 'gzip') {
            if (@function_exists('gzencode')) {
                echo gzencode($data);
            }
        } else if ($compression == 'zip') {
            if (@function_exists('gzcompress')) {
                echo gzcompress($data);
            }
        } // 3. on screen
        else {
            echo $data;
        }
        exit;
    }

    /**
     * This function writes a binary file to the browser for download.
     * @param string $file the local filename (the file you want to open
     *                         on the serverside)
     * @param string $filename the name the file will get when the user downloads it.
     * @param string $mimetype the mimetype of the file
     * @return bool wether or not the export worked
     */
    public static function exportFile($file, $filename, $mimetype = "", $detectmime = true)
    {
        include_once(Atk_Config::getGlobal("atkroot") . "atk/utils/class.atkbrowsertools.php");
        $browser = self::getBrowserInfo();
        if (preg_match("/ie/i", $browser["browser"])) {
            $mime = "application/octetstream";
            $disp = 'attachment';
        } else if (preg_match("/opera/i", $browser["browser"])) {
            $mime = "application/octetstream";
            $disp = 'inline';
        } else {
            $mime = "application/octet-stream";
            $disp = 'attachment';
        }
        if ($mimetype != "")
            $mime = $mimetype;
        else if ($mimetype == "" && $detectmime && function_exists('mime_content_type'))
            $mime = mime_content_type($file);

        $fp = @fopen($file, "rb");
        if ($fp != NULL) {
            header('Content-Type: ' . $mime);
            header("Content-Length: " . filesize($file));
            header('Content-Disposition:  ' . $disp . '; filename="' . $filename . '"');
            if (preg_match("/ie/i", $browser["browser"]))
                header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
            if (($_SERVER["SERVER_PORT"] == "443" || $_SERVER['HTTP_X_FORWARDED_PROTO'] === 'https') && preg_match("/msie/i", $_SERVER["HTTP_USER_AGENT"])) {
                header('Pragma: public');
            } else {
                header('Pragma: no-cache');
            }
            header('Expires: 0');

            header("Content-Description: File Transfer");
            header("Content-Transfer-Encoding: binary");

            fpassthru($fp);
            return true;
        }
        return false;
    }

    /**
     * Includes the file containing the specified attribute
     *
     * @param string $attribute The attribute to include in the format "module.attribute". ATK will
     *                          search in [moduledir]/attributes/ for the attribute file.
     *                          When no modulename is specified ATK will search for the attribute
     *                          in [atkdir]/attributes/
     */
    public static function useattrib($attribute)
    {
        self::atkuse("attribute", $attribute);
    }

    /**
     * Includes the file containing the specified relation
     *
     * @param string $relation The relation to include in the format "module.relation". ATK will
     *                          search in [moduledir]/relations/ for the relation file.
     *                          When no modulename is specified ATK will search for the relation
     *                          in [atkdir]/relations/
     */
    public static function userelation($relation)
    {
        self::atkuse("relation", $relation);
    }

    public static function usefilter($filter)
    {
        self::atkuse("filter", $filter);
    }

    /**
     * Returns the include file for an atk class (attribute, relation or filter)
     * @param string $type the type of the class (attribute|relation|filter)
     * @param string $name the name of the class
     */
    public static function atkgetinclude($type, $name)
    {
        global $config_atkroot;
        $a = explode(".", $name);
        if (count($a) == 2)
            $include = Atk_Module::moduleDir(strtolower($a[0])) . $type . "s/class." . strtolower($a[1]) . ".php";
        else
            $include = $config_atkroot . "atk/" . $type . "s/class." . strtolower($name) . ".php";
        return $include;
    }

    /**
     * Check if an atk class exists (attribute, relation or filter)
     * @param string $type the type of the class (attribute|relation|filter)
     * @param string $name the name of the class
     */
    public static function atkexists($type, $name)
    {
        return file_exists(self::atkgetinclude($type, $name));
    }

    /**
     * Use an atk class (attribute, relation or filter)
     * @param string $type the type of the class (attribute|relation|filter)
     * @param string $name the name of the class
     */
    public static function atkuse($type, $name)
    {
        include_once(self::atkgetinclude($type, $name));
    }

    /**
     * Returns the (virtual) hostname of the server.
     * @return string the hostname of the server
     */
    public static function atkHost()
    {
        $atkHost = $_SERVER["HTTP_HOST"] != "" ? $_SERVER["HTTP_HOST"] : $_SERVER["SERVER_NAME"];

        // if we're running on our cluster environment
        // we seem to have a specific portid within the HTTP_HOST
        // If so, remove it from the hostname

        $dummy = explode(":", $atkHost);
        return $dummy[0];
    }

    /**
     * Returns the next unique ID for the given sequence.
     * NOTE: ID's are only unique for the script execution!
     * @param string $sequence the sequence name
     * @return int next unique ID for the given sequence
     */
    public static function getUniqueID($sequence)
    {
        static $unique = array();
        if (!isset($unique[$sequence])) {
            $unique[$sequence] = 0;
        }
        return ++$unique[$sequence];
    }

    /**
     * Checks if the variable $var contains the given flag ($flag).
     * @param string $var the variable which might contain flags
     * @param var $flag the flag you want to check for
     * @return bool result of check
     */
    public static function hasFlag($var, $flag)
    {
        return ($var & $flag) == $flag;
    }

    /**
     * Makes an url from the target var and all postvars
     * @param string $target the path of the file to open
     * @param string the url with the postvars
     */
    public static function makeUrlFromPostvars($target)
    {
        global $ATK_VARS;

        if (count($ATK_VARS)) {
            $url = $target . "?";
            foreach ($ATK_VARS as $key => $val) {
                $url .= $key . "=" . rawurlencode($val) . "&";
            }
            return $url;
        }
        return "";
    }

    /**
     * Makes an string with hidden input fields containing all posted vars
     * @param array $excludes array with the vars to exclude, default empty
     */
    public static function makeHiddenPostvars($excludes = array())
    {
        global $ATK_VARS;
        $str = "";

        if (count($ATK_VARS)) {
            foreach ($ATK_VARS as $key => $val) {
                if (!in_array($key, $excludes)) {
                    $inputs = array();
                    self::atkMakeHiddenPostVarsRecursion($key, $val, $inputs);
                    $str .= implode('', $inputs);
                }
            }
            return $str;
        }
        return "";
    }

    public static function atkMakeHiddenPostVarsRecursion($key, $val, &$inputs, $name = null)
    {
        if ($name == null) {
            $name = htmlentities($key);
        } else {
            $name .= '[' . htmlentities($key) . ']';
        }

        if (is_array($val)) {
            foreach ($val as $rKey => $rVal) {
                self::atkMakeHiddenPostVarsRecursion($rKey, $rVal, $inputs, $name);
            }
        } else {
            $inputs[] = "<input type='hidden' name=\"" . $name . "\" value=\"" . htmlentities(strval($val)) . "\">\n";
        }
    }

    /**
     * Returns a string representation of an action status.
     * @param var $status status of the action
     *                    (ACTION_FAILED|ACTION_SUCCESS|ACTION_CANCELLED)
     */
    public static function atkActionStatus($status)
    {
        switch ($status) {
            case ACTION_CANCELLED:
                return "cancelled";
            case ACTION_FAILED:
                return "failed";
            case ACTION_SUCCESS:
                return "success";
        }
    }

    /**
     * Build query string based on an array of parameters.
     *
     * @param array $params array of parameters
     */
    public static function buildQueryString($params, $parent = "")
    {
        $query = "";

        foreach ($params as $key => $value) {
            if (!empty($query))
                $query .= '&';

            if (!empty($parent))
                $key = "{$parent}[{$key}]";

            if (!is_array($value)) {
                $query .= "$key=" . rawurlencode($value);
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
     * Note that this does not necessarily create a link to the current php
     * file (dispatch.php, index.php). It asks the controller which one to use.
     *
     * @param string $node the (module.)node name
     * @param string $action the atk action the link will perform
     * @param string $params : A key/value array with extra options for the url
     * @param string $phpfile The php file to use for dispatching, if not set we look at the theme for the dispatchfile
     * @return string url for the node with the action
     */
    public static function dispatch_url($node, $action, $params = array(), $phpfile = '')
    {
        $c = self::atkinstance("atk.atkcontroller");
        if (!$phpfile)
            $phpfile = $c->getPhpFile();
        $url = $phpfile;
        $atkparams = array();
        if ($node != "")
            $atkparams["atknodetype"] = $node;
        if ($action != "")
            $atkparams["atkaction"] = $action;
        $params = array_merge($atkparams, $params);

        if ($params != "" && is_array($params) && count($params) > 0)
            $url .= '?' . self::buildQueryString($params);

        return $url;
    }

    /**
     * @deprecated Use Atk_controller::getPhpFile() instead.
     */
    public static function getDispatchFile()
    {
        $c = self::atkinstance("atk.atkcontroller");
        return $c->getPhpFile();
    }

    /**
     * Generate a partial url.
     *
     * @param string $node the (module.)node name
     * @param string $action the atkaction
     * @param string $partial the partial name
     * @param array $params a key/value array with extra params
     * @param int $sessionStatus session status (default SESSION_PARTIAL)
     * @return string url for the partial action
     */
    public static function partial_url($node, $action, $partial, $params = array(), $sessionStatus = SESSION_PARTIAL)
    {
        if (!is_array($params))
            $params = array();
        $params['atkpartial'] = $partial;

        return self::session_url(self::dispatch_url($node, $action, $params), $sessionStatus);
    }

    /**
     * Writes trace file to system tmp directory
     * @param string $msg message to display in the trace
     */
    public static function atkTrace($msg = "")
    {
        global $HTTP_SERVER_VARS, $HTTP_SESSION_VARS, $HTTP_GET_VARS, $HTTP_COOKIE_VARS, $HTTP_POST_VARS;

        $log = "\n" . str_repeat("=", 5) . "\n";
        $log .= "Trace triggered: " . $msg . "\n";
        $log .= date("r") . "\n";
        $log .= $HTTP_SERVER_VARS["REMOTE_ADDR"] . "\n";
        $log .= $HTTP_SERVER_VARS["SCRIPT_URL"] . "\n";
        $log .= "\nSessioninfo: " . "session_name(): " . session_name() . " session_id(): " . session_id() . " SID: " . SID . " REQUEST: " . $_REQUEST[session_name()] . " COOKIE: " . $_COOKIE[session_name()] . "\n";
        $log .= "\n\nHTTP_SERVER_VARS:\n";
        $log .= var_export($HTTP_SERVER_VARS, true);
        $log .= "\n\nHTTP_SESSION_VARS:\n";
        $log .= var_export($HTTP_SESSION_VARS, true);
        $log .= "\n\nHTTP_COOKIE_VARS:\n";
        $log .= var_export($HTTP_COOKIE_VARS, true);
        $log .= "\n\nHTTP_POST_VARS:\n";
        $log .= var_export($HTTP_POST_VARS, true);
        $log .= "\n\nHTTP_GET_VARS:\n";
        $log .= var_export($HTTP_GET_VARS, true);

        $log .= "\n\nSession file info:\n";
        $log .= var_export(stat(session_save_path() . "/sess_" . session_id()), true);

        $tmpfile = tempnam("/tmp", Atk_Config::getGlobal("identifier") . "_trace_");
        $fp = fopen($tmpfile, "a");
        fwrite($fp, $log);
        fclose($fp);
    }

    /**
     * Creates a session aware button
     * @param string $text the self::text to display on the button
     * @param string $url the url to use for the button
     * @param var $sessionstatus the session flags
     *              (SESSION_DEFAULT (default)|SESSION_NEW|SESSION_REPLACE|
     *               SESSION_NESTED|SESSION_BACK)
     * @param string $cssclass the css class the button should get
     * @param bool $embeded wether or not it's an embedded button
     */
    public static function atkButton($text, $url = "", $sessionstatus = SESSION_DEFAULT, $embedded = true, $cssclass = "")
    {
        $page = &Atk_Page::getInstance();
        $page->register_script(Atk_Config::getGlobal("atkroot") . "atk/javascript/formsubmit.js");
        static $cnt = 0;

        if ($cssclass == "")
            $cssclass = "btn";

        $cssclass = ' class="' . $cssclass . '"';
        $script = 'atkSubmit("' . self::atkurlencode(self::session_url($url, $sessionstatus)) . '")';
        $button = '<input type="button" name="atkbtn' . (++$cnt) . '" value="' . $text . '" onClick=\'' . $script . '\'' . $cssclass . '>';

        if (!$embedded) {
            $res = '<form name="entryform">';
            $res .= self::session_form();
            $res .= $button . '</form>';
            return $res;
        } else {
            return $button;
        }
    }

    /**
     * Imports a file
     * @param string $fullclassname Name of class in atkformat (map1.map2.classfile)
     * @param bool $failsafe If $failsafe is true (default), the class is required.  Otherwise, the
     *                                class is included.
     * @param bool $path Whether or not it is NOT an ATK classname
     *                                 ("map.class"), if true it will interpret classname
     *                                 as: "map/class.classname.php", default false.
     * @return bool whether the file we want to import was actually imported or not
     */
    public static function atkimport($fullclassname, $failsafe = true, $path = false)
    {
        return Atk_ClassLoader::import($fullclassname, $failsafe, $path);
    }

    /**
     * Imports a Zend Framework class.
     * @param string $classname name of class in zend-format (starting with a Capital)
     */
    public static function zendimport($classname)
    {
        if (Atk_Config::getGlobal("zend_framework_path") == null) {
            throw new Exception("Zend Framework path not set (" . Atk_Config::getGlobal('zend_framework_path') . ")!");
        }

        $current_path = getcwd();
        chdir(Atk_Config::getGlobal('atkroot') . Atk_Config::getGlobal("zend_framework_path") . "/");

        $filename = $classname . '.php';

        if (file_exists($filename)) {
            require_once $filename;
        }

        chdir($current_path);
    }

    /**
     * Clean-up the given path.
     *
     * @param string $path
     * @return cleaned-up path
     *
     * @see http://nl2.php.net/manual/en/function.realpath.php (comment of 21st of September 2005)
     */
    public static function atkCleanPath($path)
    {
        return Atk_ClassLoader::cleanPath($path);
    }

    /**
     * Converts an ATK classname ("map1.map2.classname")
     * to a pathname ("/map1/map2/class.classname.php")
     * @param string $fullclassname ATK classname to be converted
     * @param bool $class is the file a class? defaults to true
     * @return string converted filename
     */
    public static function getClassPath($fullclassname, $class = true)
    {
        return Atk_ClassLoader::getClassPath($fullclassname, $class);
    }

    /**
     * Converts a pathname ("/map1/map2/class.classname.php")
     * to an ATK classname ("map1.map2.classname")
     * @param string $classpath pathname to be converted
     * @param bool $class is the file a class? defaults to true
     * @return string converted filename
     */
    public static function getClassName($classpath, $class = true)
    {
        return Atk_ClassLoader::getClassName($classpath, $class);
    }

    /**
     * Returns a new instance of a class
     * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
     * @return obj instance of the class
     */
    public static function atknew($fullclassname)
    {
        $args = func_get_args();
        array_shift($args);
        $args = array_values($args);
        return Atk_ClassLoader::newInstanceArgs($fullclassname, $args);
    }

    /**
     * Return a singleton instance of the specified class.
     *
     * This works for all singletons that implement the getInstance() method.
     *
     * @param string $fullclassname the ATK classname of the class ("map1.map2.classname")
     * @return obj instance of the class
     * */
    public static function &atkinstance($fullclassname, $reset = false)
    {
        return Atk_ClassLoader::getSingletonInstance($fullclassname, $reset);
    }

    /**
     * Compares two assosiative multi dimensonal array's
     * if arrays differ, return true, otherwise it returns false
     * @param array $array1 original array
     * @param array $array2 new array
     * @return boolean wether or not the arrays differ
     */
    public static function atkArrayCompare($array1, $array2)
    {
        $difference = self::atkArrayDiff($array1, $array2);

        return !is_array($difference) ? false : true;
    }

    /**
     * Compares two assosiative multi dimensonal array's
     * if arrays differ, return differences, otherwise it returns false
     * @param array $array1 original array
     * @param array $array2 new array
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
                    if ($new_diff != FALSE) {
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
     * @param Array $array The array that
     * @return bool Wether or not we found anything
     */
    public static function atk_value_in_array($array)
    {
        if (is_array($array) && !empty($array)) {
            foreach ($array as $key => $value) {
                if (is_array($value)) {
                    if (self::atk_value_in_array($value))
                        return true;
                } else if ($value)
                    return true;
            }
        }
        return false;
    }

    /**
     * Recursive function to look if the needle exists in the haystack
     *
     * WARNING: take care with using this function as it is recursive
     * and if you have a value linking back to it's self in one way or another,
     * you may spend a loooong time waiting on your application
     *
     * @param String $needle The value which will be searched in the haystack
     * @param Array $haystack Array with values
     * @return Boolean True if needle exists in haystack
     */
    public static function atk_in_array_recursive($needle, $haystack)
    {
        foreach ($haystack as $key => $value) {
            if ($value == $needle)
                return true;
            else if (is_array($value)) {
                if (self::atk_in_array_recursive($needle, $value))
                    return true;
            }
        }
        return false;
    }

    /**
     * Escapes the predefined characters
     *
     * When there are predefined characters used this function will escape them
     * and returns right pattern.
     *
     * @param String $pattern Raw string to be escaped
     * @return String Returns a pattern with the predefined pattern escaped
     */
    public static function escapeForRegex($pattern)
    {
        $escaped = '';
        $escapechars = array("/", "?", '"', "(", ")", "'", "*", ".", "[", "]");
        for ($counter = 0; $counter < strlen($pattern); $counter++) {
            $curchar = substr($pattern, $counter, 1);
            if (in_array($curchar, $escapechars))
                $escaped .= "\\";
            $escaped .= $curchar;
        }
        return $escaped;
    }

    /*
     * Returns the postvars
     * Returns a value or an array with all values
     */

    public static function atkGetPostVar($key = "")
    {
        if (empty($key) || $key == "") {
            return $_REQUEST;
        } else {
            if (array_key_exists($key, $_REQUEST) && $_REQUEST[$key] != "")
                return $_REQUEST[$key];
            return "";
        }
    }

    /**
     * ATK version of the PHP htmlentities function. Works just like PHP's
     * htmlentities function, but falls back to atkGetCharset() instead of
     * PHP's default charset, if no charset is given.
     *
     * @param String $string string to convert
     * @param int $quote_style quote style (defaults to ENT_COMPAT)
     * @param String $charset character set to use (default to atkGetCharset())
     *
     * @return String encoded string
     */
    public static function atk_htmlentities($string, $quote_style = ENT_COMPAT, $charset = null)
    {
        return Atk_String::htmlentities($string, $quote_style, $charset);
    }

    /**
     * ATK version of the PHP html_entity_decode function. Works just like PHP's
     * html_entity_decode function, but falls back to atkGetCharset() instead of
     * PHP's default charset, if no charset is given.
     *
     * @param String $string string to convert
     * @param int $quote_style quote style (defaults to ENT_COMPAT)
     * @param String $charset character set to use (default to atkGetCharset())
     *
     * @return String encoded string
     */
    public static function atk_html_entity_decode($string, $quote_style = ENT_COMPAT, $charset = null)
    {
        return Atk_String::html_entity_decode($string, $quote_style, $charset);
    }

    /**
     * Get string length
     * @param string $str The string being checked for length
     * @return int
     */
    public static function atk_strlen($str)
    {
        return Atk_String::strlen($str);
    }

    /**
     * Get part of string
     * @param string $str The string being checked.
     * @param int $start The first position used in $str
     * @param int $length [optional] The maximum length of the returned string
     * @return string
     */
    public static function atk_substr($str, $start, $length = '')
    {
        return Atk_String::substr($str, $start, $length);
    }

    /**
     *  Find position of first occurrence of string in a string
     * @param object $haystack The string being checked.
     * @param object $needle The position counted from the beginning of haystack .
     * @param object $offset [optional] The search offset. If it is not specified, 0 is used.
     * @return int|boolean
     */
    public static function atk_strpos($haystack, $needle, $offset = 0)
    {
        return Atk_String::strpos($haystack, $needle, $offset);
    }

    /**
     * Make a string lowercase
     * @param string $str The string being lowercased.
     * @return string
     */
    public static function atk_strtolower($str)
    {
        return Atk_String::strtolower($str);
    }

    /**
     *
     * Make a string uppercase
     * @param string $str The string being uppercased.
     * @return string
     */
    public static function atk_strtoupper($str)
    {
        return Atk_String::strtoupper($str);
    }

    /**
     * Return the default charset, first we look if the
     * config_default_charset is set, else we use the
     * charset in the languge file;
     * @return string
     */
    public static function atkGetCharset()
    {
        return Atk_Config::getGlobal('default_charset', self::atktext('charset', 'atk'));
    }

    /**
     * Looks up a value using the given key in the given array and returns
     * the value if found or a default value if not found.
     *
     * @param array $array Array to be searched for key
     * @param string $key Key for which we are looking in array
     * @param mixed $defaultvalue Value we will return if key was not found in array
     * @return mixed Value retrieved from array or default value if not found in array
     */
    public static function atkArrayNvl($array, $key, $defaultvalue = null)
    {
        return (isset($array[$key]) ? $array[$key] : $defaultvalue);
    }

    /**
     * Resolve a classname to its final classname.
     *
     * An application can overload a class with a custom version. This
     * method resolves the initial classname to its overloaded version
     * (if any).
     *
     * @param String $class The name of the class to resolve
     * @return String The resolved classname
     */
    public static function atkResolveClass($class)
    {
        self::atkimport("atk.utils.atkclassloader");
        return Atk_ClassLoader::resolveClass($class);
    }

    /**
     * Returns the IP of the remote client.
     *
     * @return string ip address
     */
    public static function atkGetClientIp()
    {
        static $s_ip = NULL;

        if ($s_ip === NULL) {
            if (getenv("HTTP_CLIENT_IP"))
                $s_ip = getenv("HTTP_CLIENT_IP");
            elseif (getenv("HTTP_X_FORWARDED_FOR")) {
                $ipArray = explode(",", getenv("HTTP_X_FORWARDED_FOR"));
                $s_ip = $ipArray[0];
            } elseif (getenv("REMOTE_ADDR"))
                $s_ip = getenv("REMOTE_ADDR");
            else
                $s_ip = 'x.x.x.x';
        }

        return $s_ip;
    }

    /**
     * Function checks php version and clones the
     * given attribute in the right way
     *
     * @param object $attribute The attribute to clone
     * @return object $attr
     */
    public static function atkClone($attribute)
    {
        if (intval(substr(phpversion(), 0, 1)) < 5)
            $attr = $attribute;
        else
            $attr = clone($attribute);

        return $attr;
    }

    /**
     * Return the current script file. Like $_SERVER['PHP_SELF'], but
     * sanitized for security reasons
     *
     * @return String
     */
    public static function atkSelf()
    {
        $self = $_SERVER['PHP_SELF'];
        if (strpos($self, '"') !== false)
            $self = substr($self, 0, strpos($self, '"')); //XSS attempt
        return htmlentities(strip_tags($self)); // just in case..
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
        return Atk_String::iconv($in_charset, $out_charset, $str);
    }

    /**
     * Returns the first argument that is not null.
     *
     * @param mixed ... arguments
     * @return mixed first argument that is not null
     */
    public static function atkNvl()
    {
        for ($i = 0; $i < func_num_args(); $i++) {
            $arg = func_get_arg($i);
            if (!is_null($arg)) {
                return $arg;
            }
        }

        return null;
    }

    public static function atkEcho($message)
    {
        if (strpos(atkSelf(), "runcron")) {
            echo $message;
        }
    }

    /**
     * Format date according to a format string, uses ATK's language files to translate
     * months, weekdays etc.
     *
     * @param $date    timestamp or date array (gotten with getdate())
     * @param $format  format string, compatible with PHP's date format functions
     * @param $weekday always include day-of-week or not
     *
     * @return string formatted date
     */
    public static function atkFormatDate($date, $format, $weekday = false)
    {
        static $langcache = array();

        if (!is_array($date)) {
            $date = getdate($date);
        }

        /* format month */
        $format = str_replace("M", "%-%", $format);
        $format = str_replace("F", "%=%", $format);

        /* format day */
        $format = str_replace("D", "%&%", $format);
        $format = str_replace("l", "%*%", $format);

        if ($weekday && strpos($format, '%&%') === FALSE && strpos($format, '%*%') === FALSE) {
            $format = str_replace("d", "%*% d", $format);
            $format = str_replace("j", "%*% j", $format);
        }

        /* get date string */
        require_once(Atk_Config::getGlobal('atkroot') . "atk/utils/adodb-time.inc.php");
        $str_date = adodb_date($format, $date[0]);

        $month = $date['month'];
        $shortmonth = substr(strtolower($date["month"]), 0, 3);

        /* store the self::text calls */
        if (!isset($langcache[$month])) {
            $langcache[$month] = self::atktext(strtolower($month), "atk");
        }

        if (!isset($langcache[$shortmonth])) {
            $langcache[$shortmonth] = self::atktext($shortmonth);
        }

        /* replace month/week name */
        $str_date = str_replace("%-%", $langcache[$shortmonth], $str_date);
        $str_date = str_replace("%=%", $langcache[$month], $str_date);
        $str_date = str_replace("%*%", self::atktext(strtolower($date["weekday"]), "atk"), $str_date);
        $str_date = str_replace("%&%", self::atktext(substr(strtolower($date["weekday"]), 0, 3), "atk"), $str_date);

        /* return string */
        return $str_date;
    }


    /**
     * Tells ATK that a node exists, and what actions are available to
     * perform on that node.  Note that registerNode() is not involved in
     * deciding which users can do what, only in establishing the full set
     * of actions that can potentially be performed on the node.
     *
     * @param string $node name of the node
     * @param $action array with actions that can be performed on the node
     * @param $tabs array of tabnames for which security should be handled.
     *              Note that tabs that every user may see need not be
     *              registered.
     */
    public static function registerNode($node, $action, $tabs = array(), $section = null)
    {
        if (!is_array($tabs)) {
            $section = $tabs;
            $tabs = array();
        }

        global $g_nodes;
        $module = Atk_Module::getNodeModule($node);
        $type = Atk_Module::getNodeType($node);

        // prefix tabs with tab_
        for ($i = 0, $_i = count($tabs); $i < $_i; $i++)
            $tabs[$i] = "tab_" . $tabs[$i];

        if ($module == "")
            $module = "main";
        if ($section == null)
            $section = $module;
        $g_nodes[$section][$module][$type] = array_merge($action, $tabs);
    }


    public static function getBrowserInfo($useragent = "")
    {
        $tmp = new Atk_BrowserInfo($useragent);
        return array
        (
            "ua" => $tmp->ua,
            "version" => $tmp->full_version,
            "browser" => $tmp->browser,
            "major" => $tmp->major,
            "minor" => $tmp->minor,
            "os" => $tmp->os,
            "platform" => $tmp->platform,
            "short" => $tmp->short,
            "brName" => $tmp->brName,
            "osName" => $tmp->osName,
            "hasGui" => $tmp->hasGui,
            "spider" => $tmp->spider,
            "family" => $tmp->family,
            "gecko" => $tmp->gecko
        );
    }


    /**
     * Create a new menu item
     *
     * Both main menu items, separators, submenus or submenu items can be
     * created, depending on the parameters passed.
     *
     * @param String $name The menuitem name. The name that is displayed in the
     *                     userinterface can be influenced by putting
     *                     "menu_something" in the language files, where 'something'
     *                     is equal to the $name parameter.
     *                     If "-" is specified as name, the item is a separator.
     *                     In this case, the $url parameter should be empty.
     * @param String $url The url to load in the main application area when the
     *                    menuitem is clicked. If set to "", the menu is treated
     *                    as a submenu (or a separator if $name equals "-").
     *                    The dispatch_url() method is a useful function to
     *                    pass as this parameter.
     * @param String $parent The parent menu. If omitted or set to "main", the
     *                       item is added to the main menu.
     * @param mixed $enable This parameter supports the following options:
     *                      1: menuitem is always enabled
     *                      0: menuitem is always disabled
     *                         (this is useful when you want to use a function
     *                         call to determine when a menuitem should be
     *                         enabled. If the function returns 1 or 0, it can
     *                         directly be passed to this method in the $enable
     *                         parameter.
     *                      array: when an array is passed, it should have the
     *                             following format:
     *                             array("node","action","node","action",...)
     *                             When an array is passed, the menu checks user
     *                             privileges. If the user has any of the
     *                             node/action privileges, the menuitem is
     *                             enabled. Otherwise, it's disabled.
     * @param int $order The order in which the menuitem appears. If omitted,
     *                   the items appear in the order in which they are added
     *                   to the menu, with steps of 100. So, if you have a menu
     *                   with default ordering and you want to place a new
     *                   menuitem at the third position, pass 250 for $order.
     * @param $module The name of the module that added this menuitem. It is usually
     *                not necessary to pass this parameter, but is present for
     *                backwardscompatibility reasons.
     */
    public static function menuitem($name = "", $url = "", $parent = "main", $enable = 1, $order = 0, $module = "")
    {
        global $g_menu, $g_menu_parent;
        static $order_value = 100, $s_dupelookup = array();
        if ($order == 0) {
            $order = $order_value;
            $order_value+=100;
        }

        $item = array("name" => $name,
            "url" => $url,
            "enable" => $enable,
            "order" => $order,
            "module" => $module);

        if (isset($s_dupelookup[$parent][$name]) && ($name != "-")) {
            $g_menu[$parent][$s_dupelookup[$parent][$name]] = $item;
        } else {
            $s_dupelookup[$parent][$name] = isset($g_menu[$parent]) ? count($g_menu[$parent])
                : 0;
            $g_menu[$parent][] = $item;
        }
        $g_menu_parent[$name] = $parent;
    }

    /**
     * Creates multiple (sub)menu items and/or submenu(s) at once.
     * @param array $menu Array with menu/submenu items, in the following
     *                    format: array($parent=>array(0=>
     *                                              array("url"=>$url,
     *                                                    "name"=>$name)))
     */
    public static function menuitems($menu)
    {
        while (list($parent, $items) = each($menu))
            for ($i = 0; $i < count($items); $i++) {
                $GLOBALS["g_menu"][$parent][] = $items[$i];
                if (empty($items[$i]["url"]))
                    $GLOBALS["g_menu_parent"][$items[$i]["name"]] = $parent;
            }
    }

}
