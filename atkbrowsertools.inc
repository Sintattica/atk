<?php
  /**
   * A collection of functions for browser detection
   *
   * @author Wim Kosten <wim@ibuildings.nl>
   *         Ivo Jansch <ivo@achievo.com>
   * @version $Revision$
   *
   * $Id$
   *
   */

  class browserInfo
  {
   var $ua           = "";
   var $full_version = "";
   var $browser      = "unknown";
   var $major        = 0;
   var $minor        = 0;
   var $os           = "";
   var $platform     = "";
   var $short        = "";
   var $brName       = "";
   var $osName       = "";
   var $hasGui       = 0;
   var $spider       = 0;
   var $family       = "";
   var $gecko        = 0;

   // constructor function
    function browserInfo($ua="")
    {
     global $HTTP_USER_AGENT;

     // set ua 
      if (trim($ua) == "")
      {
       $this->ua = $HTTP_USER_AGENT;
      }
      else
      {
       $this->ua = $ua;
      }

     // check the browser and set properties
      $this->detectAgent();

     // detect platform and OS
      $this->detectOS();

     // set the family
     if (eregi("gecko", $this->ua))
     {
      $this->gecko = 1;
     }

     switch (strtolower($this->browser))
     {
      case "msie":
       if ($this->major < 4)
       {
        $this->family = "ie3";
       }
       else if ($this->major >= 5)
       {
         $this->family = "ie5up";
       }
       else
       {
        $this->family = "ie4up";
       }
       break;

      case "opera":
       $this->family = "opera";
       break;

      case "netscape":
       if ($this->major < 6)
       {
        $this->family = "nn4";
       }

      default:
       if ($this->gecko == 1)
       {
        $this->family = "gecko";
       }
       else
       {
        $this->family = "unknown";
       }
     }  

     $this->short = $this->browser." ".$this->full_version;
    }

   // actual detection
    function detectAgent()
    {
     //MSIE
      if (eregi( 'MSIE ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info) ||  eregi( 'Microsoft Internet Explorer ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'MSIE';
       $this->brName       = "MS Internet Explorer";
       $this->hasGui       = 1;

       // check for Opera faking MSIE
        if (eregi( 'Opera ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info) || eregi( 'Opera/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
        {  
         $this->full_version = $info[1];
         $this->browser      = 'Opera';
         $this->brName       = "Opera";
         $this->hasGui       = 1;
        }

       // check for WebTV
        if (eregi( 'WebTV/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
        {
         $this->full_version = $info[1];
         $this->browser      = 'WebTV';
         $this->brName       = "WebTV";
         $this->hasGui       = 1;
        }

       // check for AOL
        if (eregi( 'AOL ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
        {
         $ver                = $this->full_version;
         echo $ver."<br>";
         $this->full_version = $info[1];
         $this->browser      = 'AOL (MSIE $ver)'; 
         $this->brName       = "AOL"; 
         $this->hasGui       = 1;
        }        
      }

     // Opera 
      if (eregi( 'Opera ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info) || eregi( 'Opera/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'Opera';
       $this->brName       = "Opera";
       $this->hasGui       = 1;
      }

     // iCab
      elseif (eregi( 'iCab ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info) || eregi( 'iCab/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'iCab';
       $this->brName       = "iCAB";
       $this->hasGui       = 1;
      }

     // Lynx
      elseif (eregi( 'Lynx ([0-9].[0-9a-zA-Z.]{1,9})',$this->ua,$info) || eregi( 'Lynx/([0-9].[0-9a-zA-Z.]{1,9})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'Lynx';
       $this->platform     = 'Unix';
       $this->brName       = "Lynx (textbrowser)";
       $this->hasGui       = 0;
      }

     // Wget
      elseif (eregi( 'Wget/([0-9].[0-9a-zA-Z.]{1,9})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'Wget';
       $this->platform     = 'Unix';
       $this->brName       = "Wget";
       $this->hasGui       = 0;
      }

     // Snoopy (PHP HTTP stuff)
      elseif (eregi( 'Snoopy v([0-9].[0-9a-zA-Z.]{1,9})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'Snoopy';
       $this->brName       = "Snoopy";
       $this->hasGui       = 0;
      }

     // GetRight
      elseif (eregi( 'GetRight/([0-9].[0-9a-zA-Z.]{1,9})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'GetRight';
       $this->platform     = 'Win9xNT';
       $this->brName       = "GetRight";
       $this->hasGui       = 0;
      }

     // KDE Konqueror
      elseif (eregi( 'Konqueror ([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info) || eregi( 'Konqueror/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'Konqueror';
       $this->platform     = 'Unix';
       $this->brName       = "KDE Konqueror";
       $this->hasGui       = 1;
      }

     // Netscape 6
      elseif (eregi( 'Netscape6/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {  
       $this->full_version = $info[1];
       $this->browser      = 'Netscape';
       $this->brName       = "Netscape";
       $this->hasGui       = 1;
      }

     // NCSA Mosaic
      elseif (eregi( 'nsca mosaic ([0-9].[0-9a-zA-Z]{1,4})',strtolower($this->ua),$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'NSCA Mosaic';
       $this->brName       = "NSCA Mosaic";
       $this->hasGui       = 1;
      }

     // Prodigy Classic
      elseif (eregi( 'prodigy-wb/([0-9].[0-9a-zA-Z]{1,4})',strtolower($this->ua),$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Prodigy Classic';
       $this->brName       = $this->browser;   
       $this->hasGui       = 1;
      }

     // Nokia Communicator
      elseif (eregi( 'Nokia-Communicator-WWW-Browser ([0-9].[0-9a-zA-Z]{1,4})',strtolower($this->ua),$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Nokia Communicator';
       $this->brName       = $this->browser;
       $this->hasGui       = 0;
      }

     // 
     // CRAWLERS, SPIDERS ETC.
     //

     // Inktomi
      elseif (eregi( 'Slurp/',$this->ua,$info))
      {
       $this->full_version = 0;
       $this->browser      = 'Inktomi Web Robot';
       $this->brName       = "Inktomi Web Robot";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // Google
      elseif (eregi( 'Googlebot/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Google WebCrawler';
       $this->brName       = "Google WebCrawler";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // ASPSeek
      elseif (eregi( 'ASPSeek/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'ASPSeek';
       $this->brName       = "ASPSeek";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // ArchitextSpider
      elseif (eregi( 'ArchitextSpider',$this->ua,$info))
      {
       $this->full_version = 0;
       $this->browser      = 'ArchitextSpider';
       $this->brName       = "ArchitextSpider";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // Lycos spider
      elseif (eregi( 'Lycos_Spider',$this->ua,$info))
      {
       $this->full_version = 0;
       $this->browser      = 'Lycos Spider';
       $this->brName       = "Lycos Spider";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // Offline Explorer
      elseif (eregi( 'Offline Explorer/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Offline Explorer';
       $this->brName       = "Offline Explorer";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // Openbot/3.0
      elseif (eregi( 'Openbot/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Openfind Data Gatherer';
       $this->brName       = "Openfind Data Gatherer";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // Scooter
      elseif (eregi( 'Scooter-([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info) || eregi( 'Scooter-',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Scooter';
       $this->brName       = "Scooter";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // Sqworm/2.9.70-BETA 
      elseif (eregi( 'Sqworm/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'Sqworm';
       $this->brName       = "Sqworm";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // True_Robot/1.0
      elseif (eregi( 'True_Robot/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'True_Robot';
       $this->brName       = "True Robot";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // libwww-perl/5.52 
      elseif (eregi( 'libwww-perl/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'libwww-perl';
       $this->brName       = "libwww-perl";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // psbot/0.1 
      elseif (eregi( 'psbot/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
      {
       $this->full_version = $info[1];
       $this->browser      = 'picsearch crawler';
       $this->brName       = "PicSearch Crawler";
       $this->hasGui       = 0;
       $this->spider       = 1;
      }

     // if we still haven't detected something
     // it might be just an old Netscape (pre Gecko)
      if ($this->browser == "unknown")
      {
       // if contains Mozilla then it is Netscape or just the Mozilla browser
       if (eregi( 'Mozilla/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$info))
       {
        // if it also contains Gecko/ then it's a new Mozilla
        if (eregi( 'Gecko/([0-9].[0-9a-zA-Z]{1,4})',$this->ua,$dummy))
        {
         eregi('rv:([0-9].[0-9a-zA-Z].[0-9]{1,4})', $this->ua, $info_1);
         $this->full_version = $info_1[1];  
         $this->browser      = 'Mozilla';
         $this->brName       = "Mozilla";
         $this->hasGui       = 1;
        }
        else
        {
         $this->full_version = $info[1];
         $this->browser      = 'Netscape'; 
         $this->brName       = "Netscape"; 
         $this->hasGui       = 1;
        }
       }
      }

     // now extract version
      if($this->full_version > 0)
      {  
       $pos = strpos($this->full_version,".");
       if ($pos > 0)
       {  
        $this->major = substr($this->full_version, 0, $pos);
        $this->minor = substr($this->full_version, $pos+1, strlen($this->full_version));
       }
       else $this->major = $this->full_version;
      }
    }

    function detectOS()
    {
     // Windows 3.x
      if( eregi('Win16',$this->ua) || eregi('windows 3.1',$this->ua) || eregi('windows 16-bit',$this->ua) || eregi('16bit',$this->ua))
      {  
       $this->platform = "Win16";
       $this->os       = "Win31";
       $this->osname   = "Windows 3.x";
      }

     // Windows 95
      if(eregi('Win95',$this->ua) || eregi('windows 95',$this->ua))
      { 
       $this->platform = "Win32";
       $this->os       = "Win95";
       $this->osname   = "Windows 95";
      }

     // Windows 98
      elseif(eregi('Win98',$this->ua) || eregi('windows 98',$this->ua))
      {  
       $this->platform = "Win32";
       $this->os       = "Win98";
       $this->osname   = "Windows 98";    
      }

     // Windows NT
      elseif(eregi('WinNT',$this->ua) || eregi('windows NT',$this->ua))
      {  
       $this->platform = "Win32";
       $this->os       = "WinNT";
       $this->osname   = "Windows NT";    

       if(eregi('NT 5.0',$this->ua))
       {  
        $this->platform = "Win32";
        $this->os       = "Win2000";
        $this->osname   = "Windows 2000";    
       }
       
       if(eregi('NT 5.1',$this->ua))
       {
        $this->platform = "Win32";
        $this->os       = "WinXP";
        $this->osname   = "Windows XP";
       }
      }

     // Windows ME
      elseif(eregi('win 9x 4.90',$this->ua))
      {  
       $this->platform = "Win32";
       $this->os       = "Win ME";
       $this->osname   = "Windows Millenium";    
      }

     // Other Win 32
      elseif(eregi('Win',$this->ua))
      {  
       $this->platform = "Win32";
       $this->os = "Win9xNT";
      }

     // Check for os/2
      if( eregi('os/2',$this->ua) || eregi('ibm-webexplorer',$this->ua))
      {  
       $this->platform = "OS/2";
       $this->os = "OS/2";
      }

     // Check for Mac 68000
      if( eregi('68k',$this->ua) || eregi('68000',$this->ua))
      {  
       $this->platform = "Mac";
       $this->os = "Mac68k";
      }

     // Check for Mac PowerPC
      if( eregi('ppc',$this->ua) || eregi('powerpc',$this->ua))
      {  
       $this->platform = "Mac";
       $this->os = "MacPPC";
      }

     // SunOS (All versions)
      if(eregi('sunos',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "sun";
      }

     // SUN versions     
      if(eregi('sunos 4',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "SUN 4";
      }
      elseif(eregi('sunos 5',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "SUN 5";
      }
      elseif(eregi('i86',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "SUN i86";
      }

     // Irix
      if(eregi('irix',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "Irix";
      }
      if(eregi('irix 6',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "Irix 6";
      }
      elseif(eregi('irix 5',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "Irix 5";
      }

     // HP-UX
      if(eregi('hp-ux',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "HP UX";
      }
      if(eregi('hp-ux',$this->ua) && ereg('10.',$this-UA))
      { 
       $this->platform = "Unix";
       $this->os = "HP UX 10.x";
      }
      elseif(eregi('hp-ux',$this->ua) && ereg('09.',$this-UA))
      {  
       $this->platform = "Unix";
       $this->os = "HP UX 9.x";
      }

     //AIX
      if(eregi('aix',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "IBM AIX";
      }
      if(eregi('aix1',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "IBM AIX 1.x";
      }
      elseif(eregi('aix2',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "IBM AIX 2.x";
      }
      elseif(eregi('aix3',$this->ua))
      {
       $this->platform = "Unix";
       $this->os = "IBM AIX 3.x";
      }
      elseif(eregi('aix4',$this->ua))
      {
       $this->platform = "Unix";
       $this->os = "IBM AIX 4.x";
      }

     // Linux
      if(eregi('inux',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "Linux";
      }

     // Unixware
      if(eregi('unix_system_v',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "UnixWare";
      }

      // mpras
      if(eregi('ncr',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "NCR mpras";
      }

      // Reliant
      if(eregi('reliantunix',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "Reliant";
      }

      // DEC
      if(eregi('dec',$this->ua) || eregi('osfl',$this->ua) || eregi('alphaserver',$this->ua) || eregi('ultrix',$this->ua)  || eregi('alphastation',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "DEC";

       if (eregi('ultrix',$this->ua))
       {
        $this->os = "DEC Ultrix";
       }
      }

     // Sinix
      if(eregi('sinix',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "Sinix";
      }

     // BSD
      if(eregi('bsd',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "BSD";
      }

     // FreeBSD
      if(eregi('freebsd',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "FreeBSD";
      }

      // VMS
      if(eregi('vax',$this->ua) || eregi('openvms',$this->ua))
      {  
       $this->platform = "Unix";
       $this->os = "VAX VMS";
      }

      // SCO
      if(eregi('sco ',$this->ua) || eregi('unix_sv',$this->ua))
      { 
       $this->platform = "Unix";
       $this->os = "SCO";
      }
    }        
 }
 
 function getBrowserInfo($useragent="")
 {
      $tmp = new browserInfo($useragent);
      return array
          (
           "ua"=>$tmp->ua,
           "version"=>$tmp->full_version,
           "browser"=>$tmp->browser,
           "major"=>$tmp->major,
           "minor"=>$tmp->minor,
           "os"=>$tmp->os,
           "platform"=>$tmp->platform,
           "short"=>$tmp->short,
           "brName"=>$tmp->brName,
           "osName"=>$tmp->osName,
           "hasGui"=>$tmp->hasGui,
           "spider"=>$tmp->spider,
           "family"=>$tmp->family,
           "gecko"=>$tmp->gecko
          );
           
 }
 
 /**
   * This function can be used to determine if we are using ie specific extensions.
   * The function checks the value of the $config_enable_ie_extensions variable 
   * and also the browser that the user is using.
   */
  function ieExtensions()
  {
    $browser = getBrowserInfo();
    return (atkconfig("enable_ie_extensions") && $browser["family"]=="ie5up");
  }

?>