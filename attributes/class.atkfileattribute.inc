<?php
  /**
   * With this you can upload, select and remove files in a given directory
   * <br>
   * <b>Changes:</b>
   *       1.1       01-11-2000  Sandy Pleyte (sandy@ibuildings.nl)
   *                -  Added 2 attribute Flags, AF_FILE_NO_AUTOPREVIEW (shows
   *                     only filename) and AF_FILE_POPUP (create's like to popup window)
   *                    Extention of files can be longer then 3 now
   *                    Converts spaces to underscore's in the filename (for Netscape)
   *       1.2       10-11-2000 Ivo Jansch (ivo@ibuildings.nl)
   *                -  atkFileAttribute is now PHP4 compatible.
   *       1.3       04-01-2001 Sandy Pleyte (sandy@ibuildings.nl)
   *                -  $dir Can be a string with the Directory with images/files or 
   *                     an array with a Directory and a Display Url
   *                - Fixed the remove file checkbox, doesn't dissapear anymore when there is no orginal filename.
   *                - Updated the Display function with 2 new array's, one for Preview Popup Types, and
   *                  an Image array with known  image types of php to calculate the window size.
   *               - Text moved to the language file
   *       1.4      09-01-2001 Sandy pleyte (sandy@ibuildings.nl)
   *               - AF_FILE_PHYSICAL_DELETE attribute flags    
   *
   * <b>Todo</b>        : Code clean up ( del variable)
   *
   * @author Martin Roest (martin@ibuildings.net)
   * @version 1.4
   *
   * $Id$
   * $Log$
   * Revision 4.2  2001/05/17 08:06:33  ivo
   * Fixes to atkfileattribute and onetoonerelation.
   *
   * Revision 4.1  2001/05/10 08:31:00  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:08  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/02/22 22:46:17  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.2  2001/01/10 23:24:44  ivo
   * Fixed wrong filename for javascript files (must be .php3 instead of .php).
   *
   * Revision 1.1.1.1  2001/01/10 13:57:57  sandy
   * Achievo Tool Kit
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
     * Constructor
     * @param $name Name of the attribute
     * @param $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     * @param $flags Flags for this attribute
     */
    function atkFileAttribute($name, $dir, $flags=0)
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
  
      $result=$this->display($record);

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

        for($i=0;$i<count($file_arr);$i++)
        {
          if(is_file($this->m_dir.$file_arr[$i])) $result .= "<option value=\"".$file_arr[$i].$sel."\">".$file_arr[$i];
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
     * VOID implementation.. currently you cannot search on file attributes..
     * attribute's value.
     * @param $record Array with fields
     * @param $size Not used
     * @param $maxsize Not used
     * @return piece of html code with a browsebox
     */
    function search($record="")
    {
      return "&nbsp";
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
            return $filename;
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
      global $HTTP_POST_FILES;
       
      $postfiles_basename = $rec[$this->fieldName()."_postfileskey"];
      $basename = $this->fieldName();      

      if (is_array($HTTP_POST_FILES))
      {
        // php4
        if ($HTTP_POST_FILES[$postfiles_basename]["tmp_name"]=="none")
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
            return "<a href=\"".$this->m_url."$filename\" target=\"_new\">$filename</a>";
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
  }
?>