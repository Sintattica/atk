<?php
  /**
   * This is an extend of the famous atkfileattribute :). Now its possible to insert one or more
   * files in one database field
   * <br>
   * <b>Changes:</b>
   *        0.1      - initial import! (martin)    
   *
   * <b>Todo</b>     : deleting files from HD
   *                    
   *
   * @author Martin Roest (martin@ibuildings.nl)
   * @version 0.1 
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:09  ivo
   * Initial revision
   *
   * Revision 1.7  2001/02/22 22:46:18  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.6  2001/02/07 21:27:28  martin
   * fixed tiny dinske in display value
   *
   * Revision 1.5  2001/02/07 21:22:12  martin
   * *** empty log message ***
   *    
   */
  class atkMultipleFileAttribute extends atkFileAttribute
  {
    /** 
     * private vars  
     */
    var $m_delimiter = ";";

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     * @param $flags Flags for this attribute
     */
    function atkMultipleFileAttribute($name, $dir, $flags=0)
    {
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
      $GLOBALS['g_layout']->register_script("atk/javascript/class.atkfileattribute.js.php");
    }

    /**
     * Returns an array with files extracted from the content of a databasefield
     * @param $str content of dbfield
     * @param $del delimiter
     * @return array of files
     */
    function getFiles($str, $del="")
    {
      if($del == "") $del = $this->m_delimiter; 
      return explode($del, $str);
    }

    /**
     * This is done for PHP3 and PHP4 version conflicts
     * @param $needle (see php manual)
     * @param $haystack (see php manual)
     * @return in case of a match returns true
     */
    function in_array($needle,$haystack) 
    { 
      for($i=0;$i<count($haystack) && $haystack[$i] !=$needle;$i++); 
      return ($i!=count($haystack)); 
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @return piece of html code with a browsebox
     */
    function edit($record="")
    {
      if(is_dir($this->m_dir))
      {
        $d = dir($this->m_dir);
        while($item = $d->read())
        {
          if(is_file($this->m_dir.$item)) $file_arr[] = $item;
        }
        $d->close();
      }
      else
      {
        return text("no_valid_directory");
      }

      if(count($file_arr)>0)
      {
        $result .= "<select multiple size=\"3\" name=\"select_".$this->fieldName()."[]\">";
        for($i=0;$i<count($file_arr);$i++)
        {
          $sel = "";
          if(in_array($file_arr[$i], $this->getFiles($record[$this->fieldName()][orgfilename]))) $sel = "selected";
          if(is_file($this->m_dir.$file_arr[$i])) $result .= "<option value=\"".$file_arr[$i]."\" ".$sel.">".$file_arr[$i];
        }
        if(count($file_arr)>0) $result .= "</select>";
      }
      else
      {
        $result = "No files found";
      }
      if(!$this->hasFlag(AF_FILE_NO_UPLOAD)) $result .= ' <input type="file" name="'.$this->fieldName().'">';
      return $result;
    }
    
    /**
     * Convert value to record for database
     * @param $rec Array with Fields
     * @return Nothing or Fieldname or Original filename
     */
    function value2db($rec)
    {
      $select = "select_".$this->fieldName();
      global $$select;
      global $HTTP_POST_VARS;

      if(!$this->isEmpty($HTTP_POST_VARS))
      {
        $file = $this->fetchValue($HTTP_POST_VARS);
        $file[filename] = str_replace(' ','_',$file["filename"]);
        if($file[filename]!="")
        {
          @copy($file["tmpfile"],$this->m_dir.$file[filename]) OR die("<br><br><center><b>Save failed!</b></center><br>");
          $r .= $file[filename].";";
        }
      }
      if(is_array($$select)) $r .= implode($this->m_delimiter, $$select);
      return $r;
    }
    
    /** 
     * Convert value to string
     * @param $rec Array with fields
     * @return Array with tmpfile, orgfilename,filesize
     */
    function db2value($rec)
    {
      return Array("tmpfile"=>$this->m_dir.$rec[$this->fieldName()],"orgfilename"=>$rec[$this->fieldName()],"filesize"=>"?");
    }

    /**
     * Display values
     * @param $record Array with fields
     * @return Filename or Nothing
     */
    function display($record)
    {
      $files = explode($this->m_delimiter, $record[$this->fieldName()][orgfilename]);
      $prev_type = Array("jpg","jpeg","gif","tif","png","bmp","htm","html","txt");  // file types for preview
      $imgtype_prev = Array("jpg","jpeg","gif","png");  // types whitch are supported by GetImageSize

      for($i=0;$i<count($files);$i++)
      {
        if(is_file($this->m_dir.$files[$i]))
        {
          $ext = strtolower(substr($files[$i], strrpos($files[$i],'.')+1,strlen($files[$i])));
          if(in_array($ext, $prev_type))
          {
             if(in_array($ext, $imgtype_prev))
             { 
               $imagehw = GetImageSize($this->m_dir.$files[$i]);
             }
             else
             {
              $imagehw=Array("0"=>"640","1"=>"480");
             }
             $r .=  '<a href="'.$this->m_url.$files[$i].'" alt="'.$files[$i].'" onclick="NewWindow(this.href,\'name\',\''.($imagehw[0]+50).'\',\''.($imagehw[1]+50).'\',\'yes\');return false;">'.$files[$i].'</a><br>';
          }
          else
          {
            $r .= "<a href=\"".$this->m_url."$files[$i]\" target=\"_new\">$files[$i]</a><br>";
          }
        }
        else
        {
          if(strlen($files[$i])>0) $r .= $files[$i]."(<font color=\"#ff0000\">".text("file_not_exist")."</font><br>)";
        }
      }
      return $r;
    }
  }
?>
