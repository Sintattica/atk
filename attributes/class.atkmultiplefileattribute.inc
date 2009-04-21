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
   * @version $Revision: 6289 $
   * $Id$
   */
  useattrib("atkFileAttribute");
   
  /**
   * This is an extend of the famous atkfileattribute :). Now its possible 
   * to insert one or more files in one database field
   *
   * @author Martin Roest <martin@ibuildings.nl>
   * @package atk
   * @subpackage attributes
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
     * @param string $name Name of the attribute
     * @param string|array $dir Can be a string with the Directory with images/files or an array with a Directory and a Display Url
     * @param int $flags Flags for this attribute
     * @param int $size Filename size
     */
    function atkMultipleFileAttribute($name, $dir, $flags=0, $size=0)
    {            
      $this->atkFileAttribute($name,$flags|AF_CASCADE_DELETE, $size); // base class constructor
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
     * Returns an array with files extracted from the content of a databasefield
     * @param string $str content of dbfield
     * @param string $del delimiter
     * @return array of files
     */
    function getFiles($str, $del="")
    {
      if($del == "") $del = $this->m_delimiter; 
      return explode($del, $str);
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param array $record Array with fields
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
        return atktext("no_valid_directory");
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
     * @param array $rec Array with Fields
     * @return Nothing or Fieldname or Original filename
     */
    function value2db($rec)
    {
      $select = $_REQUEST["select_".$this->fieldName()];      

      if(!$this->isEmpty($_POST))
      {
        $file = $this->fetchValue($_POST);
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
     * @param array $rec Array with fields
     * @return Array with tmpfile, orgfilename,filesize
     */
    function db2value($rec)
    {
      return Array("tmpfile"=>$this->m_dir.$rec[$this->fieldName()],"orgfilename"=>$rec[$this->fieldName()],"filesize"=>"?");
    }

    /**
     * Display values
     * @param array $record Array with fields
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
             
             $page = &atkPage::getInstance();
             $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");
             $r .=  '<a href="'.$this->m_url.$files[$i].'" alt="'.$files[$i].'" onclick="NewWindow(this.href,\'name\',\''.($imagehw[0]+50).'\',\''.($imagehw[1]+50).'\',\'yes\');return false;">'.$files[$i].'</a><br>';
          }
          else
          {
            $r .= "<a href=\"".$this->m_url."$files[$i]\" target=\"_new\">$files[$i]</a><br>";
          }
        }
        else
        {
          if(strlen($files[$i])>0) $r .= $files[$i]."(<font color=\"#ff0000\">".atktext("file_not_exist")."</font><br>)";
        }
      }
      return $r;
    }
    
    /**
     * Return the database field type of the attribute.
     * @return "string" which is the 'generic' type of the database field for 
     *         this attribute.
     */
    function dbFieldType()
    {
      return "string";
    }
  }
?>
