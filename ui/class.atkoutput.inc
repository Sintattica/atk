<?php

  class atkOutput
  {    
    var $m_raw = ""; 
    var $m_content = "";
    var $m_scripts = array();
  
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
    
    function outputFlush($nocache=true, $lastmodificationstamp="")
    {
      global $g_debug_msg, $g_error_msg;
      $res = "";
      if (strlen($this->m_raw)>0)
      {
        $res = $this->m_raw;
      }
      else
      {
        // echo some headers first..

        // Since atk pages are always dynamic, we have to prevent that some browsers cache
        // the pages, unless $nocache was set to true.
        if ($nocache)
        {
	        atkdebug("Sending no-cache headers (lmd: ".gmdate("D, d M Y H:i:s")." GMT)");
          header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
          header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // always modified
//          header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
          header ("Cache-Control: private, must-revalidate");  // HTTP/1.1
          header ("Pragma: no-cache");                          // HTTP/1.0
        }
        else if ($lastmodificationstamp!=0)
        {
          header ("Last-Modified: " . gmdate("D, d M Y H:i:s", $lastmodificationstamp) . " GMT");
        }

        // Set the content type.
        header ("Content-Type: text/html; charset=".text('charset')); // The character set (defined
                                                                      // in the language files

        $res = $this->m_content;            
        
        if (count($g_error_msg)>0)
        {
          if (atkconfig("display_errors")||atkconfig("debug")>0)
          {
            // only show errors in debugmode, or when display_errors is
            // true.
            $res.='<br><font color="#FF0000">error: '.implode("<br>\nerror: ",$g_error_msg).'</font>';
          }
          // also send a mail report with errormessages..
          // (even when display of errors is turned off)
          mailreport();
        }
        if (atkconfig("debug")>0)
        {
          $res.='<br>dbg: '.implode("<br>\ndbg: ",$g_debug_msg);
        }
      }

      if (atkconfig("output_gzip") && 
          phpversion() >= '4.0.4pl1' && 
          (strstr($_SERVER["HTTP_USER_AGENT"],'compatible') || strstr($_SERVER["HTTP_USER_AGENT"],'Gecko')) && 
          strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')
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
    
   /**
     * Output rawtext to page
     * @param $txt Text
     */
    function rawoutput($txt)
    {
      $this->m_raw.= $txt."\n";
    }
    
    /**
     * Output text to page
     * @param $txt Text
     */
    function output($txt)
    {
      $this->m_content.= $txt."\n";
    }
    
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