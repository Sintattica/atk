<?php
/*
********************************************************
TinyButStrongOOo (extend TinyButStrong to process OOo doc)
Author   : Olivier LOYNET (tbsooo@free.fr)
Version  : 0.7.8
Require  : PHP >= 4.0.6 and TBS >= 2.0.4
Date     : 2005-12-12
Web site : www.tinybutstrong.com
Doc      : http://www.tinybutstrong.com/apps/tbsooo/doc.html
Download : http://www.tinybutstrong.com/download/download.php?file=tbsooo.zip
********************************************************
Released under the GNU LGPL license
http://www.gnu.org/copyleft/lesser.html
********************************************************
*/

class clsTinyButStrongOOo extends clsTinyButStrong
{
  // private properties

  var $_process_path = 'tmp/';
  var $_zip_bin = '';
  var $_unzip_bin = '';
  var $_charset = '';
  var $_ooo_basename = '';
  var $_ooo_file_ext = '';
  var $_xml_filename = '';

  // public method

  function SetZipBinary($path_binary, $test=false)
  {
    atkdebug('clsTinyButStrongOOo->SetZipBinary: This method is deprecated', DEBUG_WARNING);
    return false;
  }

  function SetUnzipBinary($path_binary, $test=false)
  {
    atkdebug('clsTinyButStrongOOo->SetUnzipBinary: This method is deprecated', DEBUG_WARNING);
    return false;
  }

  function SetProcessDir($process_path)
  {
    clearstatcache();

    // set the directory for processing temporary OOo files
    if ($process_path == '') {
      atkerror('clsTinyButStrongOOo->SetProcessDir: Parameter is empty');
      return false;
    }
    // add a trailing / at the path
    $this->_process_path = $process_path.(substr($process_path, -1, 1) == '/' ? '' : '/');

    // test if 'dir' exists
    if (!is_dir($this->_process_path)) {
      atkerror('clsTinyButStrongOOo->SetProcessDir: Directory not found : '.$this->_process_path);
      return false;
    }

    // test if 'dir' is writable
    if (!is_writable($this->_process_path)) {
      atkerror('clsTinyButStrongOOo->SetProcessDir: Directory not writable : '.$this->_process_path);
      return false;
    }
    return true;
  }

  function SetDataCharset($charset)
  {
    $this->_charset = strtoupper($charset);
  }

  function NewDocFromTpl($ooo_template_filename)
  {
    // test if OOo source file exist
    if (!file_exists($ooo_template_filename)) {
      atkerror('clsTinyButStrongOOo->NewDocFromTpl: File not found : '.$ooo_template_filename);
      return false;
    }

    // create unique ID
    $unique = md5(microtime());
    
    // find path, file and extension
    $a_pathinfo = pathinfo($ooo_template_filename);
    $this->_ooo_file_ext  = $a_pathinfo['extension'];
    $this->_ooo_basename = $this->_process_path.$unique;

    // create unique temporary basename dir
    if (!mkdir($this->_ooo_basename, 0700)) {
      atkerror('clsTinyButStrongOOo->NewDocFromTpl: Can\'t create directory : '.$this->_ooo_basename);
      return false;
    }

    // copy the ooo template into the temporary basename dir
    if (!copy($ooo_template_filename, $this->_ooo_basename.'.'.$this->_ooo_file_ext)) {
      atkerror('clsTinyButStrongOOo->NewDocFromTpl: Can\'t copy file to process dir : '.$ooo_template_filename);
      return false;
    }
    return $this->_ooo_basename.'.'.$this->_ooo_file_ext;
  }

  function LoadXmlFromDoc($xml_file)
  {
    $this->_xml_filename = $xml_file;
    $xmlfilename = $this->_ooo_basename.'/'.$this->_xml_filename;
    $ooofilename = $this->_ooo_basename.'.'.$this->_ooo_file_ext;

    // unzip the XML files
    $atkzip = &atkNew("atk.utils.atkzip");
    $extracted = $atkzip->extract($ooofilename, $this->_ooo_basename, $xml_file);

    if (!$extracted) {
      atkerror("clsTinyButStrongOOo->LoadXmlFromDoc: Error while extracting the template from the document file");
      return false;
    }

    // test if XML file exist
    if (!file_exists($xmlfilename)) {
      atkerror('clsTinyButStrongOOo->LoadXmlFromDoc: File not found: '.$xmlfilename);
      return false;
    }
    else
    {
      atkdebug('clsTinyButStrongOOo->LoadXmlFromDoc: File exists: '.$xmlfilename);
    }

    // load the template
    $this->ObjectRef = &$this;
    $this->LoadTemplate($xmlfilename, '=~_CharsetEncode');

    // convert apostrophe from XML file for TBS functions
    $this->Source = str_replace('&apos;', '\'', $this->Source);

    return true;
  }

  function SaveXmlToDoc()
  {
    // get the source result
    $this->Show(TBS_NOTHING);
    
    // Some variables
    $xmlfilename = $this->_ooo_basename.'/'.$this->_xml_filename;
    $ooofilename = $this->_ooo_basename.'.'.$this->_ooo_file_ext;

    // store the merge result in place of the XML source file
    $fdw = fopen($xmlfilename, "w");
    fwrite($fdw, $this->Source, strlen($this->Source));
    fclose ($fdw);

    // test if XML file exist
    if (!file_exists($xmlfilename)) {
      atkerror('clsTinyButStrongOOo->SaveXmlToDoc: File not found : '.$xmlfilename);
      return false;
    }

    // test if ZIP file exist
    if (!file_exists($ooofilename)) {
      atkerror('clsTinyButStrongOOo->SaveXmlToDoc: File not found : '.$ooofilename);
      return false;
    }

    // zip and remove the file
    $atkzip = &atkNew("atk.utils.atkzip");
    $path_parts = pathinfo($this->_xml_filename);
    $added = $atkzip->add($ooofilename, $xmlfilename, $path_parts['dirname']."/");
    unlink($xmlfilename);

    if (!$added) {
      atkerror("clsTinyButStrongOOo->SaveXmlToDoc: Error while integrating data into the document file");
      return false;
    }

    return true;
  }

  function GetPathnameDoc()
  {
    // return path
    return $this->_ooo_basename.'.'.$this->_ooo_file_ext;
  }

  function GetMimetypeDoc()
  {
    switch($this->_ooo_file_ext) {
      case 'sxw': return 'application/vnd.sun.xml.writer'; break;
      case 'stw': return 'application/vnd.sun.xml.writer.template'; break;
      case 'sxg': return 'application/vnd.sun.xml.writer.global'; break;
      case 'sxc': return 'application/vnd.sun.xml.calc'; break;
      case 'stc': return 'application/vnd.sun.xml.calc.template'; break;
      case 'sxi': return 'application/vnd.sun.xml.impress'; break;
      case 'sti': return 'application/vnd.sun.xml.impress.template'; break;
      case 'sxd': return 'application/vnd.sun.xml.draw'; break;
      case 'std': return 'application/vnd.sun.xml.draw.template'; break;
      case 'sxm': return 'application/vnd.sun.xml.math'; break;
      case 'odt': return 'application/vnd.oasis.opendocument.text'; break;
      case 'ott': return 'application/vnd.oasis.opendocument.text-template'; break;
      case 'oth': return 'application/vnd.oasis.opendocument.text-web'; break;
      case 'odm': return 'application/vnd.oasis.opendocument.text-master'; break;
      case 'odg': return 'application/vnd.oasis.opendocument.graphics'; break;
      case 'otg': return 'application/vnd.oasis.opendocument.graphics-template'; break;
      case 'odp': return 'application/vnd.oasis.opendocument.presentation'; break;
      case 'otp': return 'application/vnd.oasis.opendocument.presentation-template'; break;
      case 'ods': return 'application/vnd.oasis.opendocument.spreadsheet'; break;
      case 'ots': return 'application/vnd.oasis.opendocument.spreadsheet-template'; break;
      case 'odc': return 'application/vnd.oasis.opendocument.chart'; break;
      case 'odf': return 'application/vnd.oasis.opendocument.formula'; break;
      case 'odb': return 'application/vnd.oasis.opendocument.database'; break;
      case 'odi': return 'application/vnd.oasis.opendocument.image'; break;
      case 'docx': return 'application/vnd.openxmlformats '; break;
      default:    return ''; break;
    }
  }

  function FlushDoc()
  {
    // flush file
    $fp = @fopen($this->GetPathnameDoc(), 'rb'); // replace readfile()
    fpassthru($fp);
    fclose($fp);
  }

  function RemoveDoc()
  {
    // remove file
    unlink($this->GetPathnameDoc());
    // remove tmp dir
    $this->_RemoveTmpBasenameDir();
  }

  function ClearProcessDir($hour = '2', $minut = '0')
  {
    atkimport('atk.utils.atkfileutils');
    clearstatcache();
    $now = mktime(date("H")-abs((int)$hour), date("i")-abs((int)$minut), date("s"), date("m"), date("d"), date("Y"));
    if ($dir = @opendir($this->_process_path)) {
      while (($file = readdir($dir)) !== false)  {
        if ($file != ".." && $file != ".") {
          if (filemtime($this->_process_path.$file) < $now) {
            if (!(is_dir($this->_process_path.$file) ? @atkFileUtils::rmdirRecursive($this->_process_path.'/'.$file) : @unlink($this->_process_path.$file))) 
            {
              atkdebug('clsTinyButStrongOOo->ClearProcessDir: Can\'t remove directory or file : '.$this->_process_path.$file);
            }
          }
        }
      }
      closedir($dir);
    }
  }

  // private method

  function _PathQuote($path_quote)
  {
    if (strpos($path_quote, ' ') !== false) {
      $path_quote = (strpos($path_quote, '"') === 0 ? '' : '"').$path_quote;
      $path_quote = $path_quote.((strrpos($path_quote, '"') == strlen($path_quote)-1) ? '' : '"');
    }
    return $path_quote;
  }

  function _CharsetEncode($string_encode)
  {
    $string_encode = str_replace('&'   ,'&amp;', $string_encode);
    $string_encode = str_replace('<'   ,'&lt;',  $string_encode);
    $string_encode = str_replace('>'   ,'&gt;',  $string_encode);
    //$string_encode = str_replace("\n", '</text:p><text:p>', $string_encode); // '\n' by XML tags
    $string_encode = str_replace("\n", '<text:line-break/>', $string_encode); // '\n' by XML tags

    switch($this->_charset) {
      // OOo XML charset is utf8
      case 'UTF8': // no encode
        break;
      case 'ISO 8859-1': // encode ISO 8859-1 to UTF8
      default:
        $string_encode = utf8_encode($string_encode);
        break;
    }
    return $string_encode;
  }

  function _RemoveTmpBasenameDir()
  {
    clearstatcache();

    // remove the temporary directory
    if (is_dir($this->_ooo_basename) && !rmdir ($this->_ooo_basename)) {
      atkterror('clsTinyButStrongOOo->_RemoveTmpBasenameDir: Can\'t remove directory : '.$this->_ooo_basename);
    }
  }
}
?>