<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @author Dennis-Jan Broerse <dennisjan@ibuildings.nl>
 *
 * @copyright (c) 2006 Ibuildings.nl BV
 * @license see doc/LICENSE
 *
 * @version $Revision$
 * $Id$
 */


/**
 * This class provide functions to create, copy or check files or directories.
 *
 * @author dennisjan <dennisjan@ibuildings.nl>
 * @package ATK
 * @subpackage Utils
 *
 */
class atkFileUtils
{

  /**
   * This function copies everything what is in the source directory
   * to the destination directory.
   *
   * @static
   * @staticvar $orgdest to     store the first original destination.
   * @param string $source      path to the skell to copy
   * @param string $dest        path to where the skell has to be copied to
   * @param string $dirname     unique name for the first directory
   * @param octal  $privileges  octal number for the rights of the written
   * @return bool returns true when skell is copied to the destination.
   */
  function copyDirRecursive($source, $dest, $dirname='', $privileges=0777)
  {
    static $orgdest = null;

    if (is_null($orgdest))
      $orgdest = $dest;

    atkdebug("Checking write permission for ".$orgdest);

    if (!atkFileUtils::is_writable($orgdest))
    {
      atkdebug("Error no write permission!");
      return false;
    }

    atkdebug("Permission granted to write.");

    if ($dest == $orgdest && $dirname != '')
    {
      mkdir($orgdest . "/" . $dirname,$privileges);
      return atkFileUtils::copyDirRecursive($source,$orgdest."/".$dirname,'',$privileges);
    }

    // Simple copy for a file
    if (is_file($source))
    {
      $c = copy($source, $dest);

      chmod($dest, $privileges);

      return $c;
    }

    // Make destination directory
    if (!is_dir($dest))
    {
      if ($dest != $orgdest && !is_dir($orgdest.'/'.$dirname) && $dirname != '')
      $dest = $orgdest.'/'.$dirname;

      $oldumask = umask(0);

      mkdir($dest, $privileges);

      umask($oldumask);
    }

    // Loop through the folder
    $dir = dir($source);

    while (false !== $entry = $dir->read())
    {
      // Skip pointers
      if ($entry == '.' || $entry == '..')
        continue;

      // Deep copy directories
      if ($dest !== "$source/$entry")
        atkFileUtils::copyDirRecursive("$source/$entry", "$dest/$entry", $dirname, $privileges);
    }

    // Clean up
    $dir->close();

    return true;
  }

  /**
   * This function checks if the root of the destination is writeable.
   * The difference with php native function is that this functions accepts
   * non-existing directories.
   *
   * @static
   * @param string $orgdest document parh
   * @return bool returns true if the destination is writeable.
   */
  function is_writable($orgdest)
  {
    if ($orgdest{0} == '/')
    {
      if (count($orgdest) == 1)
        $testdest = $orgdest;
      else
        $testdest= substr($orgdest, 0, strpos($orgdest, '/', 1));
    }
    else
    {
      if ($orgdest{strlen($orgdest)-1} != '/' && !is_file($orgdest))
      $orgdest .= '/';

      $testdest = $orgdest;

      if (!is_dir($orgdest))
      {
        $orgdestArray = explode('/', $orgdest);

        $testdest = $orgdestArray[0].'/';
      }
    }

    atkdebug("Checking with: ".$testdest);

    return is_writable($testdest);
  }

  /**
   * This function creates recursively a destination. This fuction accepts
   * a full path ../dir/subdir/subdir2/subdir3 etc. It checks if the path is writeable
   * and replace mis typed slashes with forward slashes.
   *
   * @static
   * @param string $dir the fullpath
   * @param octal  $privileges  octal number for the rights of the written
   * @return bool returns true if the destination is written.
   */
  function mkdirRecursive($dir, $privileges=0777, $recursive=true)
   {
     $dir = preg_replace('/(\/){2,}|(\\\){1,}/','/',$dir); //only forward-slash

     if (!atkFileUtils::is_writable($dir))
     {
       atkdebug("Error no write permission!");
       return false;
     }

     atkdebug("Permission granted to write.");

     if( is_null($dir) || $dir === "" ){
       return FALSE;
     }
     if( is_dir($dir) || $dir === "/" ){
       return TRUE;
     }
     if( atkFileUtils::mkdirRecursive(dirname($dir), $privileges, $recursive) ){
       return mkdir($dir, $privileges);
     }
     return FALSE;
   }

   /**
    * This function parse a templatestring with the data and returns
    * a string with the data parsed in the template.
    *
    * @static
    * @param string $template the template to parse
    * @param array $data array which contains the data for the template
    * @return string returns the parsed string
    */
   function parseDirectoryName($template, $data)
   {
     atkimport("atk.utils.atkstringparser");
     $stringparser = new atkStringParser($template);
     return $stringparser->parse($data);
   }


  /**
   * Returns mimetype for the given filename, based on fileextension.
   * This function is different from mime_content_type because it
   * does not access the file but just looks at the fileextension and
   * returns the right mimetype.
   *
   * @static
   * @param string $filename Filename (or just the extention) we want
   *                         to know the mime type for.
   * @return string          The mimetype for this file extension.
   */
  function atkGetMimeTypeFromFileExtension($filename)
  {
    $ext = strtolower(end(explode('.',$filename )));

    $mimetypes = array(
    'ai' =>'application/postscript',
    'aif' =>'audio/x-aiff',
    'aifc' =>'audio/x-aiff',
    'aiff' =>'audio/x-aiff',
    'asc' =>'text/plain',
    'atom' =>'application/atom+xml',
    'avi' =>'video/x-msvideo',
    'bcpio' =>'application/x-bcpio',
    'bmp' =>'image/bmp',
    'cdf' =>'application/x-netcdf',
    'cgm' =>'image/cgm',
    'cpio' =>'application/x-cpio',
    'cpt' =>'application/mac-compactpro',
    'crl' =>'application/x-pkcs7-crl',
    'crt' =>'application/x-x509-ca-cert',
    'csh' =>'application/x-csh',
    'css' =>'text/css',
    'dcr' =>'application/x-director',
    'dir' =>'application/x-director',
    'djv' =>'image/vnd.djvu',
    'djvu' =>'image/vnd.djvu',
    'doc' =>'application/msword',
    'dtd' =>'application/xml-dtd',
    'dvi' =>'application/x-dvi',
    'dxr' =>'application/x-director',
    'eps' =>'application/postscript',
    'etx' =>'text/x-setext',
    'ez' =>'application/andrew-inset',
    'gif' =>'image/gif',
    'gram' =>'application/srgs',
    'grxml' =>'application/srgs+xml',
    'gtar' =>'application/x-gtar',
    'hdf' =>'application/x-hdf',
    'hqx' =>'application/mac-binhex40',
    'html' =>'text/html',
    'html' =>'text/html',
    'ice' =>'x-conference/x-cooltalk',
    'ico' =>'image/x-icon',
    'ics' =>'text/calendar',
    'ief' =>'image/ief',
    'ifb' =>'text/calendar',
    'iges' =>'model/iges',
    'igs' =>'model/iges',
    'jpe' =>'image/jpeg',
    'jpeg' =>'image/jpeg',
    'jpg' =>'image/jpeg',
    'js' =>'application/x-javascript',
    'kar' =>'audio/midi',
    'latex' =>'application/x-latex',
    'm3u' =>'audio/x-mpegurl',
    'man' =>'application/x-troff-man',
    'mathml' =>'application/mathml+xml',
    'me' =>'application/x-troff-me',
    'mesh' =>'model/mesh',
    'mid' =>'audio/midi',
    'midi' =>'audio/midi',
    'mif' =>'application/vnd.mif',
    'mov' =>'video/quicktime',
    'movie' =>'video/x-sgi-movie',
    'mp2' =>'audio/mpeg',
    'mp3' =>'audio/mpeg',
    'mpe' =>'video/mpeg',
    'mpeg' =>'video/mpeg',
    'mpg' =>'video/mpeg',
    'mpga' =>'audio/mpeg',
    'ms' =>'application/x-troff-ms',
    'msh' =>'model/mesh',
    'mxu m4u' =>'video/vnd.mpegurl',
    'nc' =>'application/x-netcdf',
    'oda' =>'application/oda',
    'ogg' =>'application/ogg',
    'pbm' =>'image/x-portable-bitmap',
    'pdb' =>'chemical/x-pdb',
    'pdf' =>'application/pdf',
    'pgm' =>'image/x-portable-graymap',
    'pgn' =>'application/x-chess-pgn',
    'php' =>'application/x-httpd-php',
    'php4' =>'application/x-httpd-php',
    'php3' =>'application/x-httpd-php',
    'phtml' =>'application/x-httpd-php',
    'phps' =>'application/x-httpd-php-source',
    'png' =>'image/png',
    'pnm' =>'image/x-portable-anymap',
    'ppm' =>'image/x-portable-pixmap',
    'ppt' =>'application/vnd.ms-powerpoint',
    'ps' =>'application/postscript',
    'qt' =>'video/quicktime',
    'ra' =>'audio/x-pn-realaudio',
    'ram' =>'audio/x-pn-realaudio',
    'ras' =>'image/x-cmu-raster',
    'rdf' =>'application/rdf+xml',
    'rgb' =>'image/x-rgb',
    'rm' =>'application/vnd.rn-realmedia',
    'roff' =>'application/x-troff',
    'rtf' =>'text/rtf',
    'rtx' =>'text/richtext',
    'sgm' =>'text/sgml',
    'sgml' =>'text/sgml',
    'sh' =>'application/x-sh',
    'shar' =>'application/x-shar',
    'shtml' =>'text/html',
    'silo' =>'model/mesh',
    'sit' =>'application/x-stuffit',
    'skd' =>'application/x-koan',
    'skm' =>'application/x-koan',
    'skp' =>'application/x-koan',
    'skt' =>'application/x-koan',
    'smi' =>'application/smil',
    'smil' =>'application/smil',
    'snd' =>'audio/basic',
    'spl' =>'application/x-futuresplash',
    'src' =>'application/x-wais-source',
    'sv4cpio' =>'application/x-sv4cpio',
    'sv4crc' =>'application/x-sv4crc',
    'svg' =>'image/svg+xml',
    'swf' =>'application/x-shockwave-flash',
    't' =>'application/x-troff',
    'tar' =>'application/x-tar',
    'tcl' =>'application/x-tcl',
    'tex' =>'application/x-tex',
    'texi' =>'application/x-texinfo',
    'texinfo' =>'application/x-texinfo',
    'tgz' =>'application/x-tar',
    'tif' =>'image/tiff',
    'tiff' =>'image/tiff',
    'tr' =>'application/x-troff',
    'tsv' =>'text/tab-separated-values',
    'txt' =>'text/plain',
    'ustar' =>'application/x-ustar',
    'vcd' =>'application/x-cdlink',
    'vrml' =>'model/vrml',
    'vxml' =>'application/voicexml+xml',
    'wav' =>'audio/x-wav',
    'wbmp' =>'image/vnd.wap.wbmp',
    'wbxml' =>'application/vnd.wap.wbxml',
    'wml' =>'text/vnd.wap.wml',
    'wmlc' =>'application/vnd.wap.wmlc',
    'wmlc' =>'application/vnd.wap.wmlc',
    'wmls' =>'text/vnd.wap.wmlscript',
    'wmlsc' =>'application/vnd.wap.wmlscriptc',
    'wmlsc' =>'application/vnd.wap.wmlscriptc',
    'wrl' =>'model/vrml',
    'xbm' =>'image/x-xbitmap',
    'xht' =>'application/xhtml+xml',
    'xhtml' =>'application/xhtml+xml',
    'xls' =>'application/vnd.ms-excel',
    'xml xsl' =>'application/xml',
    'xpm' =>'image/x-xpixmap',
    'xslt' =>'application/xslt+xml',
    'xul' =>'application/vnd.mozilla.xul+xml',
    'xwd' =>'image/x-xwindowdump',
    'xyz' =>'chemical/x-xyz',
    'zip' =>'application/zip'
    );

    $ext = trim(strtolower($ext));
    if (array_key_exists($ext,$mimetypes))
    {
      atkdebug("Filetype for $filename is {$mimetypes[$ext]}");
      return $mimetypes[$ext];
    }
    else
    {
      atkdebug("Filetype for $filename could not be found. Returning application/octet-stream.");
      return "application/octet-stream";
    }
  }

  /**
   * Delete complete directory and all of its contents.
   *
   * @todo If one of the subdirs/files cannot be removed, you end up with
   * a half deleted directory structure.
   * @param string $dir The directory to remove
   * @return True if succesful, false if not
   */
  function rmdirRecursive( $dir )
  {
    if ( !is_writable( $dir ) )
    {
      if ( !@chmod( $dir, 0777 ) )
      {
        return FALSE;
      }
    }

    $d = dir( $dir );
    while ( FALSE !== ( $entry = $d->read() ) )
    {
      if ( $entry == '.' || $entry == '..' )
      {
        continue;
      }
      $entry = $dir . '/' . $entry;
      if ( is_dir( $entry ) )
      {
        if ( !atkFileUtils::rmdirRecursive( $entry ) )
        {
          return FALSE;
        }
        continue;
      }
      if ( !@unlink( $entry ) )
      {
        $d->close();
        return FALSE;
      }
    }

    $d->close();

    rmdir( $dir );

    return TRUE;
  }
}

?>