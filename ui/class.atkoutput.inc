<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Outputbuffering class.
   *
   * Any output sent to the browser should be directed through the atkOutput
   * class. Among other things, it buffers debugging output and errormessages,
   * and displays them below the actual output. If $config_output_gzip is set
   * to true, all output is sent gzipped to the browser (saves bandwith).
   *
   * If $config_mailreport is set to a valid email address, this class also
   * takes care of sending error reports to the email address, if any errors
   * occurred during script execution.
   *
   * The atkOutput class is a singleton. The one-and-only instance should be
   * retrieved with the getInstance() method.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage ui
   *
   */
  class atkOutput
  {
    /**
     * Store raw output data.
     * @access private
     */
    var $m_raw = "";

    /**
     * Store regular output data.
     * @access private
     */
    var $m_content = "";

    /**
     * Retrieve the one-and-only atkOutput instance.
     * @return atkOutput The instance.
     */
    function &getInstance()
    {
      static $s_instance = NULL;
      if ($s_instance == NULL)
      {
        atkdebug("Created a new atkOutput instance");
        $s_instance = new atkOutput();
      }
      return $s_instance;
    }
    
    function sendCachingHeaders($lastmodificationstamp='',$nocache=true)
    {
      // Since atk pages are always dynamic, we have to prevent that some browsers cache
      // the pages, unless $nocache was set to true.
      if ($nocache)
      {
        atkOutput::sendNoCacheHeaders();
      }
      else if ($lastmodificationstamp!=0)
      {
        $_last_modified_date = @substr($_SERVER['HTTP_IF_MODIFIED_SINCE'], 0, strpos($_SERVER['HTTP_IF_MODIFIED_SINCE'], 'GMT') + 3);
        $_gmt_mtime = gmdate('D, d M Y H:i:s', $lastmodificationstamp).' GMT';
        if($_last_modified_date == $_gmt_mtime)
        {
          header("HTTP/1.0 304 Not Modified");
          return;
        }
        else
        {
          header ("Last-Modified: " . gmdate("D, d M Y H:i:s", $lastmodificationstamp) . " GMT");
        }
      }
    }
    
    /**
     * Send no cache headers to the browser.
     * @static
     */
    function sendNoCacheHeaders()
    {
      atkdebug("Sending no-cache headers (lmd: ".gmdate("D, d M Y H:i:s")." GMT)");
      header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
      header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // always modified
      header("Cache-Control: no-store, no-cache, must-revalidate");
      header("Cache-Control: post-check=0, pre-check=0", false);
      header ("Pragma: no-cache");                          // HTTP/1.0
    }

    /**
     * Send all output to the browser.
     * @param boolean $nocache If true, sends no-cache headers to the browser,
     *                         so the browser will not cache the output in its
     *                         browsercache.
     * @param int $lastmodificationstamp Timestamp of last modification. If
     *                                   set, a last-modified header
     *                                   containing this stamp will be sent to
     *                                   the browser. If $nocache is true,
     *                                   this parameter is ignored.
     */
    function outputFlush($nocache=true, $lastmodificationstamp="", $charset="")
    {
      global $g_error_msg;
      $res = "";
      if (strlen($this->m_raw)>0)
      {
        $res = $this->m_raw;
      }
      else
      {
        // send some headers first..
        $this->sendCachingHeaders($lastmodificationstamp,$nocache);
        
        // Set the content type and the character set (as defined in the language files)
        header ("Content-Type: text/html; charset=".($charset==""?atktext("charset","atk"):$charset)); 

        $res = $this->m_content;

        if (count($g_error_msg)>0)
        {
          // send an mail report with errormessages..
          // (even when display of errors is turned off)
          mailreport();
        }
        
        $debugger = &atkinstance('atk.utils.atkdebugger');
        $res .= $debugger->renderDebugAndErrorMessages();
      }

      if (atkconfig("output_gzip") &&
          phpversion() >= '4.0.4pl1' &&
          (strstr($_SERVER["HTTP_USER_AGENT"],'compatible') || strstr($_SERVER["HTTP_USER_AGENT"],'Gecko')) &&
          isset($_SERVER['HTTP_ACCEPT_ENCODING']) && strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')
         )
      {
        header('Content-Encoding: gzip');
        echo $this->gzip($res);
      }
      else
      {
        echo $res;
      }
    }
    
    function getDebugging()
    {
      global $g_debug_msg;
      if (atkconfig("debug")>0)
      {
        $output = '<br><div style="font-family: monospace; font-size: 11px;" align="left" id="atk_debugging_div">'.implode("<br>\n ",$g_debug_msg).'</div>';
      return $output;
      }
      return "";
    }

    /**
     * Output raw, headerless text to the browser. If this method is called,
     * all regular output is suppressed and the contents of the rawoutput
     * is passed as-is to the browser when outputFlush() is called.
     *
     * @param String $txt The text to output.
     */
    function rawoutput($txt)
    {
      $this->m_raw.= $txt."\n";
    }

    /**
     * Output regular text to the browser.
     * @param String $txt The text to output.
     */
    function output($txt)
    {
      $this->m_content.= $txt."\n";
    }

    /**
     * Gzip a piece of text.
     *
     * Called internally by atkOutput when $config_output_gzip is set to true,
     * but can be used by other scripts too, if they need to gzip some data.
     * @param String $contents The string to gzip.
     * @return String The gzipped string.
     */
    function gzip($contents)
    {
      $gzip_size = strlen($contents);
      $gzip_crc  = crc32($contents);

      $contents = gzcompress($contents, 9);
      $contents = substr($contents, 0, strlen($contents) - 4);

      $res = "\x1f\x8b\x08\x00\x00\x00\x00\x00";
      $res.= $contents;
      $res.= pack('V', $gzip_crc);
      $res.= pack('V', $gzip_size);

      return $res;

    }

  }

?>
