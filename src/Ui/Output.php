<?php

namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Utils\Debugger;

/**
 * Outputbuffering class.
 *
 * Any output sent to the browser should be directed through the Output
 * class. Among other things, it buffers debugging output and errormessages,
 * and displays them below the actual output. If $config_output_gzip is set
 * to true, all output is sent gzipped to the browser (saves bandwith).
 *
 * If $config_mailreport is set to a valid email address, this class also
 * takes care of sending error reports to the email address, if any errors
 * occurred during script execution.
 *
 * The Output class is a singleton. The one-and-only instance should be
 * retrieved with the getInstance() method.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class Output
{
    /*
     * Store raw output data.
     * @access private
     */
    public $m_raw = '';

    /*
     * Store regular output data.
     * @access private
     */
    public $m_content = '';

    /**
     * Retrieve the one-and-only Output instance.
     *
     * @return Output The instance.
     */
    public static function &getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            Tools::atkdebug('Created a new Output instance');
            $s_instance = new self();
        }

        return $s_instance;
    }

    /**
     * Output header.
     *
     * @param string $header
     */
    public static function header($header)
    {
        if (php_sapi_name() == 'cli') {
            return;
        }

        header($header);
    }

    /**
     * Send the caching headers.
     *
     * @param mixed $lastmodificationstamp Timestamp of the last modification
     * @param bool $nocache Send cache headers?
     */
    public function sendCachingHeaders($lastmodificationstamp = '', $nocache = true)
    {
        // Since atk pages are always dynamic, we have to prevent that some browsers cache
        // the pages, unless $nocache was set to true.
        if ($nocache) {
            self::sendNoCacheHeaders();
        } else {
            if ($lastmodificationstamp != 0) {
                $_last_modified_date = @substr($_SERVER['HTTP_IF_MODIFIED_SINCE'], 0, strpos($_SERVER['HTTP_IF_MODIFIED_SINCE'], 'GMT') + 3);
                $_gmt_mtime = gmdate('D, d M Y H:i:s', $lastmodificationstamp).' GMT';
                if ($_last_modified_date == $_gmt_mtime) {
                    self::header('HTTP/1.0 304 Not Modified');

                    return;
                } else {
                    self::header('Last-Modified: '.gmdate('D, d M Y H:i:s', $lastmodificationstamp).' GMT');
                }
            }
        }
    }

    /**
     * Send no cache headers to the browser.
     *
     * @static
     */
    public static function sendNoCacheHeaders()
    {
        Tools::atkdebug('Sending no-cache headers (lmd: '.gmdate('D, d M Y H:i:s').' GMT)');
        self::header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');    // Date in the past
        self::header('Last-Modified: '.gmdate('D, d M Y H:i:s').' GMT'); // always modified
        self::header('Cache-Control: no-store, no-cache, must-revalidate');
        self::header('Cache-Control: post-check=0, pre-check=0');
        self::header('Pragma: no-cache');                          // HTTP/1.0
    }

    /**
     * Send all output to the browser.
     *
     * @param bool $nocache If true, sends no-cache headers to the browser,
     *                                      so the browser will not cache the output in its
     *                                      browsercache.
     * @param mixed $lastmodificationstamp Timestamp of last modification. If
     *                                      set, a last-modified header
     *                                      containing this stamp will be sent to
     *                                      the browser. If $nocache is true,
     *                                      this parameter is ignored.
     * @param string $charset The character set
     */
    public function outputFlush($nocache = true, $lastmodificationstamp = '', $charset = '')
    {
        global $g_error_msg;

        if (strlen($this->m_raw) > 0) {
            $res = $this->m_raw;
        } else {
            // send some headers first..
            $this->sendCachingHeaders($lastmodificationstamp, $nocache);

            // Set the content type and the character set (as defined in the language files)
            self::header('Content-Type: text/html; charset='.($charset == '' ? Tools::atkGetCharset() : $charset));

            $res = $this->m_content;

            if (count($g_error_msg) > 0) {
                // send an mail report with errormessages..
                // (even when display of errors is turned off)
                Tools::mailreport();
            }

            $debugger = Debugger::getInstance();
            $res .= $debugger->renderDebugAndErrorMessages();
        }

        if (Config::getGlobal('output_gzip') && phpversion() >= '4.0.4pl1' && (strstr($_SERVER['HTTP_USER_AGENT'],
                    'compatible') || strstr($_SERVER['HTTP_USER_AGENT'],
                    'Gecko')) && isset($_SERVER['HTTP_ACCEPT_ENCODING']) && strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')
        ) {
            self::header('Content-Encoding: gzip');
            echo $this->gzip($res);
        } else {
            echo $res;
        }
    }

    /**
     * Display the debug statements.
     *
     * @return string htmlcode with debugging
     */
    public function getDebugging()
    {
        global $g_debug_msg;
        if (Config::getGlobal('debug') > 0) {
            $output = '<br><div style="font-family: monospace; font-size: 11px;" align="left" id="atk_debugging_div">'.implode("<br>\n ",
                    $g_debug_msg).'</div>';

            return $output;
        }

        return '';
    }

    /**
     * Output raw, headerless text to the browser. If this method is called,
     * all regular output is suppressed and the contents of the rawoutput
     * is passed as-is to the browser when outputFlush() is called.
     *
     * @param string $txt The text to output.
     */
    public function rawoutput($txt)
    {
        $this->m_raw .= $txt."\n";
    }

    /**
     * Output regular text to the browser.
     *
     * @param string $txt The text to output.
     */
    public function output($txt)
    {
        $this->m_content .= $txt."\n";
    }

    /**
     * Gzip a piece of text.
     *
     * Called internally by Output when $config_output_gzip is set to true,
     * but can be used by other scripts too, if they need to gzip some data.
     *
     * @param string $contents The string to gzip.
     *
     * @return string The gzipped string.
     */
    public function gzip($contents)
    {
        $gzip_size = strlen($contents);
        $gzip_crc = crc32($contents);

        $contents = gzcompress($contents, 9);
        $contents = substr($contents, 0, strlen($contents) - 4);

        $res = "\x1f\x8b\x08\x00\x00\x00\x00\x00";
        $res .= $contents;
        $res .= pack('V', $gzip_crc);
        $res .= pack('V', $gzip_size);

        return $res;
    }
}
