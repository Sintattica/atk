<?php
  /**
   * With this you can upload, select and remove files in a given directory   
   *
   * <b>Todo</b>        : Code clean up (del variable)
   *                    : check of ext is jpg, iets anders en ie knalt uit elkaar !!!!
   *                    : proper previewdisplay
   *
   * @author paul verhoeff paul@ibuildings.nl
   * @version $Revision$
   *
   * $Id$
   *     
   */
  class atkjpg2swfAttribute extends atkAttribute
  {
    /** 
     * Directory with images  
     */
    var $m_dir = "";
    var $m_url = "";

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     * @param $flags Flags for this attribute
     */
    function atkjpg2swfAttribute($name, $dir, $flags=0)
    {
      global $config_atkroot;
      $this->atkAttribute($name,$flags|AF_CASCADE_DELETE); // base class constructor
      if(is_array($dir))
      {
        $this->m_dir = $this->AddSlash($dir[0]);
        $this->m_url = $this->AddSlash($dir[1]);
      }
      else
      {
        $this->m_dir = $this->AddSlash($dir);
	      $this->m_url = $this->AddSlash($dir);
      }
      $GLOBALS['g_layout']->register_script($config_atkroot."atk/javascript/newwindow.js");
	
    }


    /**
     * returns a string with a / on the end
     * @param $string String with the url/dir
     * @return string with a / on the end
     */
    function AddSlash($dir_url)
    {
      if(substr($dir_url,strlen($dir_url)-1,strlen($dir_url))!="/") { $dir_url.="/"; }
      return $dir_url;
    }


    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @param $size Not used
     * @param $maxsize Not used
     * @return piece of html code with a browsebox
     */
    function edit($record="", $fieldprefix="")
    {
      global $del;
      
      if ($del[$this->fieldName()]!="on")
      {
        $result=$this->display($record);
      }

      if(is_dir($this->m_dir))
      {
        $d = dir($this->m_dir);
        while($item = $d->read())
        {
          if(is_file($this->m_dir.$item)) $file_arr[] = $item;
        }
        $d->close();
        if(count($file_arr)>0) natcasesort($file_arr);
      }
      else
      {
        return text("no_valid_directory");
      }

      if ($result!="")
      {
        $result.="<br>";
        $result.='<input type="hidden" name="'.$fieldprefix.$this->fieldName().'_orgfilename" value="'.$record[$this->fieldName()]['orgfilename'].'">';
      }
      
      $result.='<input type="hidden" name="'.$fieldprefix.$this->fieldName().'_postfileskey" value="'.$fieldprefix.$this->fieldName().'">';

      if(!$this->hasFlag(AF_FILE_NO_UPLOAD)) $result .= '<input type="file" name="'.$fieldprefix.$this->fieldName().'">';

      if(!$this->hasFlag(AF_FILE_NO_SELECT) AND count($file_arr) > 0)
      {
        $result .= "<select name=\"select[".$fieldprefix.$this->fieldName()."]\">";
        $result .= "<option value=\"\">Selectie";                    // Zorg voor een default met waarde NULL
        while (list ($key, $val) = each ($file_arr))
        {
          if(is_file($this->m_dir.$val)) $result .= "<option value=\"".$val.$sel."\">".$val;
        }
        if(count($file_arr) > 0) $result .= "</select>";
      }
      if(!$this->hasFlag(AF_FILE_NO_DELETE) && $GLOBALS['atkaction']=="edit" && $record[$this->fieldname()]['orgfilename']!='')
      {      
       $result .= '<br><input type="checkbox" name="del['.$fieldprefix.$this->fieldName().']">&nbsp;'.text("remove_current_file");
      }
      return $result;
    }

    /**
     * Convert value to record for database
     * @param $rec Array with Fields
     * @return Nothing or Fieldname or Original filename
     */
    function value2db($rec)
    {
      global $del;
      global $select;            

      if($select[$this->fieldName()] != "" && !$this->hasFlag(AF_FILE_NO_SELECT))
      {
        return $select[$this->fieldName()];
      }
      if($this->hasFlag(AF_FILE_NO_DELETE)) unset($del);  // Make sure if flag is set $del unset!
      if($del[$this->fieldName()] == "on")
      {    
        if($this->hasFlag(AF_FILE_PHYSICAL_DELETE))
	      {
	        unlink($this->m_dir.$rec[$this->fieldName()]["orgfilename"]);
	      }
        return "";
      }
      else
      {
        $filename = str_replace(' ','_',$rec[$this->fieldName()]["filename"]);
        if ($filename!="")
        {
          if (@copy($rec[$this->fieldName()]["tmpfile"],$this->m_dir.$filename) OR die("<br><br><center><b>Save failed!</b></center><br>"))
          {

        global $currentByte;
		global $bytePos;
		
    	$currentByte = 0; // the value of the current byte being created
 	    $bytePos     = 8; // the number of bits left to fill in the current byte
		
		define( "Fixed1", 0x00010000 );
        define( "SCoord1", 20 );

		
		$swf_filename = $filename;
		$swf_filename = str_replace('.jpg','.swf',$swf_filename);
		$imageSize 	  = GetImageSize( $this->m_dir.$filename );
		$width        = $imageSize[0];
		$height       = $imageSize[1];
		unset( $imageSize );

atkdebug(":: orgwidth en orgheight :: ".$width." :-: ".$height);
		
// 2nd footer
atkdebug("footer 2");

$footer2 .= $this->WriteWord( 0x02 );
$footer2 .= $this->WriteRect( $this->MinBits( $width, $height) + 1, 0, $width * SCoord1, 0, $height * SCoord1 ); // rect

$footer2 .= $this->WriteByte( 0x01 ); // FillStyleCount (1 style)
$footer2 .= $this->WriteByte( 0x41 ); // clipped bitmap fill
$footer2 .= $this->WriteWord( 1 );    // bitmap-id

$footer2 .= $this->WriteMatrix( true, 20*Fixed1, 20*Fixed1, false, 0, 0, 0, 0);

$footer2 .= $this->WriteByte( 0x00 ); // LineStyleCount (no line)

$footer2 .= $this->WriteByte( 0x10 ); // number of fill/line index bits

$footer2 .= $this->WriteBits( 0, 1 ); // Non-edge record flag
$footer2 .= $this->WriteBits( 0, 1 ); // New styles flag
$footer2 .= $this->WriteBits( 0, 1 ); // Line style change flag
$footer2 .= $this->WriteBits( 1, 1 ); // Fill style 1 change flag
$footer2 .= $this->WriteBits( 0, 1 ); // Fill style 0 change flag
$footer2 .= $this->WriteBits( 1, 1 ); // Move to flag

$footer2 .= $this->WriteBits( $this->MinBits( $width, $height) + 1, 5 );
$footer2 .= $this->WriteBits( $width * SCoord1, $this->MinBits( $width, $height) + 1);
$footer2 .= $this->WriteBits( $height * SCoord1, $this->MinBits( $width, $height) + 1);

$footer2 .= $this->WriteBits( 1, 1); // Fill 1 Style = 1 (this is our bitmap-style)

$footer2 .= $this->WriteLine( 0, -$width  * SCoord1, $width, $height );
$footer2 .= $this->WriteLine( 1, -$height * SCoord1, $width, $height );
$footer2 .= $this->WriteLine( 0,  $width  * SCoord1, $width, $height );
$footer2 .= $this->WriteLine( 1,  $height * SCoord1, $width, $height );

$footer2 .= $this->WriteBits( 0, 1); // Non-edge record flag
$footer2 .= $this->WriteBits( 0, 5); // End of shape flag

$footer2 .= $this->FlushBits(); // flush bits to keep byte aligned

// 1st footer
atkdebug("footer 1");
$footer1 .= $this->WriteWord( 0xbf );
$footer1 .= $this->WriteDWord( strlen( $footer2 ) );

// 3rd footer
atkdebug("footer 3");
$footer3 .= $this->WriteWord( 0x0686 ); // placeObject2
$footer3 .= $this->WriteByte( 0x06 );   // body has a transform matrix and object has a character ID
$footer3 .= $this->WriteWord( 0x01 );   // depth = 1
$footer3 .= $this->WriteWord( 0x02 );   // character-id
$footer3 .= $this->WriteByte( 0x00 );   // no transformation

$footer3 .= $this->WriteWord( 0x40 );
$footer3 .= $this->WriteWord( 0x00 );

// calculate entire file-size


$filesize  = 29;
$filesize += filesize( $this->m_dir.$filename );
$filesize += strlen( $footer1 );
$filesize += strlen( $footer2 );
$filesize += strlen( $footer3 );
$filesize += strlen( $this->WriteRect( $this->MinBits( $width, $height) + 1, 0, SCoord1*$width, 0, SCoord1*$height));


/*****************************************************************************
 *
 *  Actual output
 *
 *****************************************************************************/

$swf_file = "FWS";						   // reversed SWF signature (little endian/big endian issue)
$swf_file .= $this->WriteByte( 3 );          // file version, flash 3 is all we need
$swf_file .= $this->WriteDWord( $filesize ); // length of entire file in bytes

$swf_file .= $this->WriteRect( $this->MinBits( $width, $height) + 1,
                0, SCoord1*$width,
                0, SCoord1*$height ); // frame size in TWIPS
$swf_file .= $this->WriteByte( 0 );          // this one is ignored!
$swf_file .= $this->WriteByte( 12 );         // frame delay in 8.8 fixed number of frames per second
$swf_file .= $this->WriteWord( 1 );          // total number of frames in movie
$swf_file .= $this->WriteWord( 0x0243 );     // setBackgroundColor
$swf_file .= $this->WriteByte( 0xff );       // red
$swf_file .= $this->WriteByte( 0xff );       // green
$swf_file .= $this->WriteByte( 0xff );       // blue

$swf_file .= $this->WriteWord( 0x057f );     // DefineBitsJPEG2
$swf_file .= $this->WriteDWord( filesize( $this->m_dir.$filename ) + 6 ); // +6, for 0xff 0xd9 0xff 0xd8 [imagedata] 0xff 0xd8
$swf_file .= $this->WriteWord( 1 );          // character-id

$swf_file .= $this->WriteByte( 0xff );       // startImage
$swf_file .= $this->WriteByte( 0xd9 );

$swf_file .= $this->WriteByte( 0xff );       // endImage
$swf_file .= $this->WriteByte( 0xd8 );

$ip = fopen( $this->m_dir.$filename, "r" );
$swf_file .= fread( $ip, filesize( $this->m_dir.$filename ) );
fclose($ip);


atkdebug(" -::image filename:: ".$this->m_dir.$filename);
atkdebug(" -::flash filename:: ".$this->m_dir.$swf_filename);


$swf_file .= $footer1;
$swf_file .= $footer2;
$swf_file .= $footer3;
if ($swf_filename!="")
{
$fp = fopen($this->m_dir.$swf_filename, "w+");
fwrite($fp,$swf_file);
fclose($fp); 
@unlink($this->m_dir.$filename);
}
else
{
@unlink($this->m_dir.$filename);
}
	   return $swf_filename;
      } 
		  
        }
		$rec[$this->fieldName()]["orgfilename"]=$swf_filename;
        return $rec[$this->fieldName()]["orgfilename"];
      }
   }

    
    /** 
     * Convert value to string
     * @param $rec Array with fields
     * @return Array with tmpfile, orgfilename,filesize
     */
    function db2value($rec)
    {
    return Array("tmpfile"=>$this->m_dir.$rec[$this->fieldName()],"orgfilename"=>$rec[$this->fieldName()],"filename"=>$rec[$this->fieldName()],"filesize"=>"?");
    }

    /**
     * Get filename out of Array
     * @param $rec
     * @return Array with tmpfile,filename,filesize,orgfilename
     */
    function fetchValue($rec)
    {    
      global $HTTP_POST_FILES;
      global $select;
      
      $postfiles_basename = $rec[$this->fieldName()."_postfileskey"];
     
      $basename = $this->fieldName();      
      if (is_array($HTTP_POST_FILES)||($select[$this->fieldName()] != "" && !$this->hasFlag(AF_FILE_NO_SELECT))) // php4
      {   
        // if no new file has been uploaded..    
        if($select[$this->fieldName()] != "" && !$this->hasFlag(AF_FILE_NO_SELECT))
        {
          $result = array("tmpfile"=>"",
                          "filename"=>$select[$this->fieldName()],
                          "filesize"=>0,
                          "orgfilename"=>$rec[$basename."_orgfilename"]);
        
        }
        elseif (count($HTTP_POST_FILES)==0||$HTTP_POST_FILES[$postfiles_basename]["tmp_name"]=="none")
        { 
      
          $result = array("tmpfile"=>"",
                          "filename"=>"",
                          "filesize"=>0,
                          "orgfilename"=>$rec[$basename."_orgfilename"]);
        }
        else
        {
          $result = array("tmpfile"=>$HTTP_POST_FILES[$postfiles_basename]["tmp_name"],
                          "filename"=>$HTTP_POST_FILES[$postfiles_basename]["name"],
                          "filesize"=>$HTTP_POST_FILES[$postfiles_basename]["size"],
                          "orgfilename"=>$HTTP_POST_FILES[$postfiles_basename]["name"]);
        }                      
        return $result;
      }      
    }
    
    /**
     * Check if the attribute is empty..
     * @returns true if no image was uploaded
     */    
    function isEmpty($postvars)
    {
      global $HTTP_POST_FILES;
      if (is_array($HTTP_POST_FILES))
      {
        // php4
        return ($HTTP_POST_FILES[$this->fieldName()]["tmp_name"]=="none");
      }
      else
      {
        // php3
        return ($postvars[$this->fieldName()]=="none");
      }
    }   

    /**
     * Deletes file from HD
     * @param $record Array with fields
     */
    function delete($record)
    {
      if($this->hasFlag(AF_FILE_PHYSICAL_DELETE))
      {
        if(!@unlink($this->m_dir.$record[$this->fieldName()]["orgfilename"]))
	      {
	        return text("cannot_remove_file");
	      }
      }
    }


    /**
     * Display values
     * @param $record Array with fields
     * @return Filename or Nothing
     */
    function display($record)
    {
      $filename    = $record[$this->fieldName()]["orgfilename"];
    
      $prev_type     = Array("jpg","jpeg","gif","tif","png","bmp","htm","html","txt");  // file types for preview
      $imgtype_prev = Array("jpg","jpeg","gif","png");  // types whitch are supported by GetImageSize
      if($filename != "")
      {
        if(is_file($this->m_dir.$filename))
        {
          $ext = strtolower(substr($filename, strrpos($filename,'.')+1,strlen($filename)));
          if((in_array($ext, $prev_type) && $this->hasFlag(AF_FILE_NO_AUTOPREVIEW))||(!in_array($ext, $prev_type)))
          { 
            return "<a href=\"".$this->m_url."$filename\" target=\"_blank\">$filename</a>";
          }
          elseif(in_array($ext, $prev_type) && $this->hasFlag(AF_FILE_POPUP))
          {
            if(in_array($ext, $imgtype_prev))
            { 
              $imagehw = GetImageSize($this->m_dir.$filename);
            }
            else
            {
              $imagehw=Array("0"=>"640","1"=>"480");
            }
            return '<a href="'.$this->m_url.$filename.'" alt="'.$filename.'" onclick="NewWindow(this.href,\'name\',\''.($imagehw[0]+50).'\',\''.($imagehw[1]+50).'\',\'yes\');return false;">'.$filename.'</a>';
          }
          return '<img src="'.$this->m_url.$filename.'" alt="'.$filename.'">';
        }
        else
        {
          return $filename."(<font color=\"#ff0000\">".text("file_not_exist")."</font>)";
        }
      }
	
    }
  
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("exact","substring","wildcard","regexp");
    }
	
/*****************************************************************************
 *
 *  FlashWriter Toolkit v1.0
 *  Copyright (c) 2002 Stefan Schüßler
 *  This software comes with ABSOLUTELY NO WARRANTY. It is free, and
 *  you are welcome to redistribute it.
 *  The name "Stefan Schüßler" may not be used to endorse or promote
 *  products derived from this software without specific prior
 *  written permission.
 *  For written permission, please contact info@stefanschuessler.de
 *  Redistributions of source code must retain the above notices.
 *  Abstract:
 *     PHP function library for generating Shockwave Flash files
 *
 *  Environment:
 *     Shockwave Flash / PHP
 *
 *****************************************************************************/

/*****************************************************************************
 *
 *  @func   DWORD | WriteDWord |
  *          Converts a given DWORD into its binary representation.
 *
 *  @parm   DWORD | o |
  *          Value to be converted
 *
 *  @rvalue binary representation
 *
 *****************************************************************************/
function WriteDWord( $o ) {
	return pack( "V", $o );
}

/*****************************************************************************
 *
 *  @func   DWORD | WriteWord |
  *          Converts a given WORD into its binary representation.
 *
 *  @parm   WORD | o |
  *          Value to be converted
 *
 *  @rvalue binary representation
 *
 *****************************************************************************/
function WriteWord( $o ) {
	return pack( "v", $o );
}

/*****************************************************************************
 *
 *  @func   BYTE | WriteByte |
  *          Converts a given BYTE into its binary representation.
 *
 *  @parm   BYTE | o |
  *          Value to be converted
 *
 *  @rvalue binary representation
 *
 *****************************************************************************/
function WriteByte( $o ) {
	return pack( "C", $o );
}

/*****************************************************************************
 *
 *  @func   BYTE | WriteBits |
  *          Adds 'size' bits from 'data' to the stream until all 'size' bits
 *          have been output.
 *          Currently a maximum of 31 bits is supported only. For adding
 *          32 bits split it up into two 16-bit shunks of data.
 *
 *  @parm   U32 | data |
  *          Data to be appended to the stream
 *
 *  @parm   INT | size |
  *          indicates how many of the 32 bits are significant and
 *          should be output
 *
 *  @rvalue returns the actual byte
 *
 *****************************************************************************/
function WriteBits( $data, $size) {
global $currentByte;
global $bytePos;

	$ret = "";
			
	if ( $data < 0 ) {
		$data *= -1;
		$data = ( 0x7FFFFFFF >> ( 31 - $size ) ) - $data;
	}

	while ( $size != 0 ) {
		if ( $size > $bytePos ) {
			$currentByte |= $data << (30 - $size) >> (30 - $bytePos);
			$ret .= $this->WriteByte( $currentByte );
			$size -= $bytePos;
			$currentByte = 0;
			$bytePos = 8;
		}
		else if ( $size <= $bytePos ) {
			$currentByte |= $data << (30 - $size) >> (30 - $bytePos);
			$bytePos -= $size;
			$size = 0;
			if ( !$bytePos ) {
				$ret .= $this->WriteByte( $currentByte );
				$currentByte = 0;
				$bytePos = 8;
			}
		}
	}
	return $ret;
}

/*****************************************************************************
 *
 *  @func   BYTE | FlushBits |
  *          Kick out the current partially filled byte to the stream.
 *          If there is a byte currently being built for addition to
 *          the stream, then the end of that byte is filled with zeroes
 *          and the byte is added to the stream.
 *
 *  @rvalue returns the actual byte
 *
 *****************************************************************************/
function FlushBits() {

global $currentByte,$bytePos;
	$ret = "";

	if ( $bytePos != 8 ) {
		$ret = $this->WriteByte( $currentByte );
		$currentByte = 0;
		$bytePos = 8;
	}

	return $ret;
}

/*****************************************************************************
 *
 *  @func   BYTE | WriteRect |
  *          Writes a rectangle to the stream.
 *
 *  @parm   int | nBits |
  *          Number of Bits needed to store the rect
 *
 *  @parm   int | xmin |
 *          X-value for the upper left corner of rectangle
 *
 *  @parm   int | xmax |
 *          X-value for the lower right corner of rectangle
 *
 *  @parm   int | ymin |
 *          Y-value for the upper left corner of rectangle
 *
 *  @parm   int | ymax |
 *          Y-value for the lower right corner of rectangle
 *
 *  @rvalue returns the actual byte
 *
 *****************************************************************************/
function WriteRect( $nBits, $xmin, $xmax, $ymin, $ymax ) {

global $currentByte;
global $bytePos;
			
	$ret = "";
	$ret .= $this->WriteBits( $nBits, 5 );
	$ret .= $this->WriteBits( $xmin, $nBits );
	$ret .= $this->WriteBits( $xmax, $nBits );
	$ret .= $this->WriteBits( $ymin, $nBits);
	$ret .= $this->WriteBits( $ymax, $nBits);
	$ret .= $this->FlushBits();
	return $ret;
}

/*****************************************************************************
 *
 *  @func   BYTE | WriteMatrix |
 *          Writes a matrix to the stream.
 *
 *  @parm   boolean | hasScale |
 *          Flag determining whether this matrix has scale values
 *
 *  @parm   int | scaleX |
 *          X-scale for the matrix
 *
 *  @parm   int | scaleY |
 *          Y-scale for the matrix
 *
 *  @parm   boolean | hasRotate |
 *          Flag determining whether this matrix has rotation values
 *
 *  @parm   int | rotateSkew0 |
*          Horizontal skew for the matrix or degree of rotation if
 *          equal to rotateSkew1
 *
 *  @parm   int | rotateSkew1 |
 *          Vertical skew for the matrix or degree of rotation if
 *          equal to rotateSkew0
 *
 *  @parm   int | translateX |
 *          Horizontal translation for the matrix
 *
 *  @parm   int | translateY |
 *          Vertical translation for the matrix
 *
 *  @rvalue returns the actual byte
 *
 *****************************************************************************/
function WriteMatrix( $hasScale, $scaleX, $scaleY,
                      $hasRotate, $rotateSkew0, $rotateSkew1,
                      $translateX, $translateY ) 
	{
		$nScaleBits = 22;
		$nRotateBits = 0;
		$nTranslateBits = 0;

global $currentByte;
global $bytePos;

		$ret = "";

		$ret .= $this->WriteBits( $hasScale, 1 );
		if ( $hasScale ) {
			$ret .= $this->WriteBits( $nScaleBits, 5);
			$ret .= $this->WriteBits( $scaleX, $nScaleBits );
			$ret .= $this->WriteBits( $scaleY, $nScaleBits );
		}
		$ret .= $this->WriteBits( $hasRotate, 1 );
		if ( $hasRotate ) {
			$ret .= $this->WriteBits( $nRotateBits, 5);
			$ret .= $this->WriteBits( $rotateSkew0, $nRotateBits);
			$ret .= $this->WriteBits( $rotateSkew1, $nRotateBits );
		}
		$ret .= $this->WriteBits( $nTranslateBits, 5);
		$ret .= $this->WriteBits( $translateX, $nTranslateBits);
		$ret .= $this->WriteBits( $translateY, $nTranslateBits);
		$ret .= $this->FlushBits();
		return $ret;
}

/*****************************************************************************
 *
 *  @func   BYTE | WriteLine |
 *
 *          Writes a line to the stream.
 *
 *  @parm   boolean | hv|
 *
 *          Flag determining whether this line is
 *          0 = horizontal or 1 = vertical
 *
 *  @parm   int | delta |
 *
 *          Length of line
 *
 *  @rvalue returns the actual byte
 *
 *****************************************************************************/
function WriteLine( $hv, $delta, $width, $height ) {

	atkdebug("::Writeline: whbc"."::".$width."::".$height);
	
	$ret = "";
	$ret .= $this->WriteBits( 1, 1 );                // edge-record
	$ret .= $this->WriteBits( 1, 1);                // straight
	$ret .= $this->WriteBits( $this->MinBits( $width, $height) - 1, 4);    // nBits
	$ret .= $this->WriteBits( 0, 1);                // Vert/Horz
	$ret .= $this->WriteBits( $hv, 1);              // vertical
	$ret .= $this->WriteBits( $delta, $this->MinBits( $width, $height)+1 ); // DeltaX
	return $ret;
}

/*****************************************************************************
 *
 *  @func   BYTE | MinBits|
 *
 *          Calculates the mininum of bits required to store the
 *          global variables width and height.
 *
 *  @rvalue returns the mininum of bits required
 *
 *****************************************************************************/
function MinBits($width, $height) {
	
	$max = max( $width * SCoord1, $height * SCoord1 );
	
	atkdebug(":: SCoord1 in MinBits:: ".SCoord1);
	atkdebug(":: MinBits wh:: ".$width." :: ".$height);
	if ( !$max )
		return 0;
	$x = 1;
	for( $i = 1; $i < 32; $i++ ) {
		$x <<= 1;
		if ( $x > $max )
			break;
	}
	return $i;
  }

	
	
  }
?>
