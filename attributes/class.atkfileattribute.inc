<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /** flag(s) specific for the atkFileAttribute */
  define("AF_FILE_NO_UPLOAD",           AF_SPECIFIC_1); // unable to upload files
  define("AF_FILE_NO_SELECT",           AF_SPECIFIC_2); // unable to select files
  define("AF_FILE_NO_DELETE",           AF_SPECIFIC_3); // unable to delete files
  define("AF_FILE_NO_AUTOPREVIEW",      AF_SPECIFIC_4); // don't try to detect the file type (shows only filename)
  define("AF_FILE_PHYSICAL_DELETE",     AF_SPECIFIC_5); // removes the file physically
  define("AF_FILE_POPUP",                    AF_POPUP); // show preview in popup instead of inline  

  /**
   * With this you can upload, select and remove files in a given directory.
   *
   * @todo - Code clean up (del variable is dirty)
   *       - Support for storing the file itself in the db instead of on disk.
   *
   * @author Martin Roest <martin@ibuildings.nl>
   * @package atk
   * @subpackage attributes
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
     * @param string $name Name of the attribute
     * @param array $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     * @param int $flags Flags for this attribute
     * @param int $size Filename size
     */
    function atkFileAttribute($name, $dir, $flags=0, $size=0)
    {          
      // We use 255 as default filename size if no size specified.
      if ($size==0) $size = 255;
      
      // Call base class constructor. 
      $this->atkAttribute($name,$flags|AF_CASCADE_DELETE, $size); 
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
    }


    /**
     * returns a string with a / on the end
     * @param string $dir_url String with the url/dir
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
     * @param array $record Record
     * @param string $fieldprefix Field prefix
     * @return string piece of html code with a browsebox
     */
    function edit($record="", $fieldprefix="")
    {   
      $result=$this->display($record);
  
      if(is_dir($this->m_dir))
      {
        $file_arr=array();
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
      
      $id = $fieldprefix.$this->fieldName();

      if ($result!="")
      {
        $result.="<br>";
        $result.='<input type="hidden" name="'.$id.'_orgfilename" value="'.$record[$this->fieldName()]['orgfilename'].'">';
      }
      
      $result.='<input type="hidden" name="'.$id.'_postfileskey" value="'.$id.'">';

      if(!$this->hasFlag(AF_FILE_NO_UPLOAD)) 
      {
        $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);  
        $result .= '<input type="file" id="'.$id.'" name="'.$id.'">';
      }

      if(!$this->hasFlag(AF_FILE_NO_SELECT) AND count($file_arr) > 0)
      {
        $this->registerKeyListener($id.'_select', KB_CTRLCURSOR|KB_LEFTRIGHT);  
        $result .= '<select id="'.$id.'_select" name="'.$id.'[select]">';
        $result .= "<option value=\"\" selected>Selectie";                    // Zorg voor een default met waarde NULL
        while (list ($key, $val) = each ($file_arr))
        {
          (isset($record[$this->fieldName()]['filename']) && $record[$this->fieldName()]['filename']==$val) ? $selected = "selected" : $selected = '';
          if(is_file($this->m_dir.$val)) $result .= "<option value=\"".$val."\" $selected>".$val;
        }
        if(count($file_arr) > 0) $result .= "</select>";
      }
      if(!$this->hasFlag(AF_FILE_NO_DELETE)
         && isset($record[$this->fieldname()]['filename'])
         && $record[$this->fieldname()]['filename']!='')
      {      
       $this->registerKeyListener($id.'_del', KB_CTRLCURSOR|KB_CURSOR);
       $result .= '<br><input id="'.$id.'_del" type="checkbox" name="del['.$fieldprefix.$this->fieldName().']">&nbsp;'.text("remove_current_file");
      }
      return $result;
    }

    /**
     * Convert value to record for database
     * @param array $rec Array with Fields
     * @return string Nothing or Fieldname or Original filename
     */
    function value2db($rec)
    {
      if(isset($_REQUEST["del"])) $del = $_REQUEST["del"];

      if($rec[$this->fieldName()]["tmpfile"]=="" && $rec[$this->fieldName()]["filename"]!=""
         && !$this->hasFlag(AF_FILE_NO_SELECT)
         && (!isset($del[$this->fieldName()]) || $del[$this->fieldName()]!="on"))
      {
        return $rec[$this->fieldName()]["filename"];
      }

      if($this->hasFlag(AF_FILE_NO_DELETE)) unset($del);  // Make sure if flag is set $del unset!

      if(isset($del[$this->fieldName()]) && $del[$this->fieldName()] == "on")
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
        // why copy if the files are the same?
        
        if ($this->m_dir.$filename != $rec[$this->fieldName()]["tmpfile"])
        {
            if ($filename!="")
            {
              if (@copy($rec[$this->fieldName()]["tmpfile"],$this->m_dir.$filename))
              {
                atkdebug("COPY: ".$rec[$this->fieldName()]["tmpfile"]." -> ".$this->m_dir.$filename);
                return $filename;
              }
              else
              {
                atkdebug("tmp file ".$rec[$this->fieldName()]["tmpfile"]." dir ".$this->m_dir.$filename);
                atkerror("File kon niet worden opgeslagen");
                return "";
              }
            }
        }
        return $rec[$this->fieldName()]["orgfilename"];
      }
    }
    
    /** 
     * Convert value to string
     * @param array $rec Array with fields
     * @return array Array with tmpfile, orgfilename,filesize
     */
    function db2value($rec)
    {
      return Array("tmpfile"=>$this->m_dir.$rec[$this->fieldName()],"orgfilename"=>$rec[$this->fieldName()],"filename"=>$rec[$this->fieldName()],"filesize"=>"?");
    }

    /**
     * Get filename out of Array
     * @param array $rec Record
     * @return array Array with tmpfile,filename,filesize,orgfilename
     */
    function fetchValue($rec)
    {    
      if(isset($_REQUEST["del"])) $del=$_REQUEST["del"];

      $postfiles_basename = $rec[$this->fieldName()."_postfileskey"];
     
      $basename = $this->fieldName();      
      if (is_array($_FILES)||($rec[$this->fieldName()]["select"] != "" && !$this->hasFlag(AF_FILE_NO_SELECT)) || ($rec[$this->fieldName()]["filename"]!="")) // php4
      { 
        // if no new file has been uploaded..    

        if (count($_FILES)==0||$_FILES[$postfiles_basename]["tmp_name"]=="none"||$_FILES[$postfiles_basename]["tmp_name"]=="")
        {
           // No file to upload, then check if the select box is filled
          if(isset($rec[$this->fieldName()]["select"]) && $rec[$this->fieldName()]["select"] != ""
             && !$this->hasFlag(AF_FILE_NO_SELECT))
          {
            atkdebug("file selected!");
            (isset($del[$this->fieldName()]) && $del[$this->fieldName()]=="on") ? $filename='' : $filename=$rec[$this->fieldName()]["select"];
            $result = array("tmpfile"=>"",

                          "filename"=>$filename,
                          "filesize"=>0,
                          "orgfilename"=>$filename);
          }  // maybe we atk restored data from session
          elseif(isset($rec[$this->fieldName()]["filename"]) && $rec[$this->fieldName()]["filename"]!="")
          {
            $result = $rec[$this->fieldName()];
          }
          else
          {
            $result = array("tmpfile"=>"",
                          "filename"=>"",
                          "filesize"=>0,
                          "orgfilename"=>(isset($rec[$basename."_orgfilename"]))?$rec[$basename."_orgfilename"]:"");
          }
        }
        else
        {    
          $realname = $this->_filenameMangle($rec, $_FILES[$postfiles_basename]["name"]);
          $result = array("tmpfile"=>$_FILES[$postfiles_basename]["tmp_name"],
                          "filename"=>$realname,
                          "filesize"=>$_FILES[$postfiles_basename]["size"],
                          "orgfilename"=>$realname);
        }
        return $result;
      }      
    }
    
    /**
     * Check if the attribute is empty..
     * @param array $postvars Array with the postvars
     * @return boolean true if no image was uploaded
     */    
    function isEmpty($postvars)
    {      
      if (is_array($_FILES))
      {
        // php4
        return ($_FILES[$this->fieldName()]["tmp_name"]=="none");
      }
      else
      {
        // php3
        return ($postvars[$this->fieldName()]=="none");
      }
    }   

    /**
     * Deletes file from HD
     * @param array $record Array with fields
     * @return boolean False if the delete went wrong
     */
    function delete($record)
    {
      if($this->hasFlag(AF_FILE_PHYSICAL_DELETE)&&($record[$this->fieldname()]["orgfilename"]!=""))
      {
        if(!@unlink($this->m_dir.$record[$this->fieldName()]["orgfilename"]))
        {
          return false;
        }
      }
      return true;
    }

    /**
     * Display values
     * @param array $record Array with fields
     * @return string Filename or Nothing
     */
    function display($record)
    {      
            
      $filename    = $record[$this->fieldName()]["orgfilename"];
      atkdebug($this->fieldname()." - File: $filename");
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
            $page = &atkPage::getInstance();
            $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");
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
     * @param array $record Array with values
     * @return string Piece of htmlcode
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
    
    function dbFieldType()
    {
      return "string";
    }

  }
?>
