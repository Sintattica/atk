<?php
  /**
   * With this you can upload, select and remove files in a given directory   
   *
   * <b>Todo</b>        : Code clean up (del variable)
   *
   * @author Martin Roest <martin@ibuildings.net>
   * @version $Revision$
   *
   * $Id$
   *     
   */
  class atkFileAttribute extends atkAttribute
  {
    /** 
     * Directory with images  
     */
    var $m_dir = "";
    var $m_url = "";

    /**
     * Name mangle feature. If you set filename tpl, then uploaded files
     * are renamed to what you set in the template. You can use
     * fieldnames between brackets to have the filename determined by
     * the record.
     *
     * This is useful in the following case:
     * Say, you have a class for managing users. Each user has a photo associated
     * with them. Now, if two users would upload 'gandalf.gif', then you would 
     * have a naming conflicht and the picture of one user is overwritten with the
     * one from the other user.
     * If you set m_filenameTpl to "picture_[name]", then the file is renamed before
     * it is stored. If the user's name is 'Ivo Jansch', and he uploads 'gandalf.gif', 
     * then the file that is stored is picture_Ivo_Jansch.gif. This way, you have a
     * unique filename per user.
     */
    var $m_filenameTpl = "";

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     * @param $flags Flags for this attribute
     */
    function atkFileAttribute($name, $dir, $flags=0)
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
      global $del,$atkaction;
      
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
        $result .= "<option value=\"\" selected>Selectie";                    // Zorg voor een default met waarde NULL
        while (list ($key, $val) = each ($file_arr))
        {
          ($record[$this->fieldName()]['filename']==$val) ? $selected = "selected" : $selected = '';
          if(is_file($this->m_dir.$val)) $result .= "<option value=\"".$val.$sel."\" $selected>".$val;
        }
        if(count($file_arr) > 0) $result .= "</select>";
      }
      if(!$this->hasFlag(AF_FILE_NO_DELETE) && $record[$this->fieldname()]['filename']!='')
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
      global $del,$select;            

      if($select[$this->fieldName()] != "" && !$this->hasFlag(AF_FILE_NO_SELECT) && $del[$this->fieldName()]!="on")
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
        return '';
      }
      else
      {
        $filename = $rec[$this->fieldName()]["filename"];
        if ($filename!="")
        {
          if (@copy($rec[$this->fieldName()]["tmpfile"],$this->m_dir.$filename))
          {
            return $filename;
          }
          else
          {
            atkerror("File kon niet worden opgeslagen");
            return "";
          }
        }
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
      global $HTTP_POST_FILES,$select,$del;          
      
      $postfiles_basename = $rec[$this->fieldName()."_postfileskey"];
     
      $basename = $this->fieldName();      
      if (is_array($HTTP_POST_FILES)||($select[$this->fieldName()] != "" && !$this->hasFlag(AF_FILE_NO_SELECT))) // php4
      {   
        // if no new file has been uploaded..    
        if($select[$this->fieldName()] != "" && !$this->hasFlag(AF_FILE_NO_SELECT))
        {
          ($del[$this->fieldName()]=="on") ? $filename='' : $filename=$select[$this->fieldName()];
          $result = array("tmpfile"=>"",
                          "filename"=>$filename,
                          "filesize"=>0,
                          "orgfilename"=>'');
        
        }
        elseif (count($HTTP_POST_FILES)==0||$HTTP_POST_FILES[$postfiles_basename]["tmp_name"]=="none"||$HTTP_POST_FILES[$postfiles_basename]["tmp_name"]=="")
        { 
          $result = array("tmpfile"=>"",
                          "filename"=>"",
                          "filesize"=>0,
                          "orgfilename"=>$rec[$basename."_orgfilename"]);
        }
        else
        {    
          $realname = $this->_filenameMangle($rec, $HTTP_POST_FILES[$postfiles_basename]["name"]);
          $result = array("tmpfile"=>$HTTP_POST_FILES[$postfiles_basename]["tmp_name"],
                          "filename"=>$realname,
                          "filesize"=>$HTTP_POST_FILES[$postfiles_basename]["size"],
                          "orgfilename"=>$realname);
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

    /**
     * Determine the real filename of a file (based on m_filenameTpl).
     */
    function _filenameMangle($rec, $default)
    {    
      if ($this->m_filenameTpl == "")
      {
        $filename = $default;
      }
      else
      {        
        $ext = strtolower(substr($default, strrpos($default,'.')+1));
        $filename = stringparse($this->m_filenameTpl, $rec).".".$ext;
      }
      return str_replace(' ','_',$filename);
    }
    
    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix)
    {
      $field = $record[$this->fieldName()];

      if (is_array($field))
        foreach ($field as $key => $value)
          $result .= '<input type="hidden" name="'.$fieldprefix.$this->formName().'['.$key.']" '.'value="'.$value.'">';

      else
        $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().'" value="'.$field.'">';

      return $result;
    }

  }
?>
