<?php  
  /**
   * File editing node.
   *
   * This is a special derivative atkNode that does not have database 
   * interaction, but that can be used to edit files in a directory on the 
   * server.
   *
   * Note: This class does not support postAdd, postUpdate and postDelete hooks.
   *       Other overrides may or me not be supported, but this has not been
   *       tested.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$
   *
   */
   
  useattrib("atktextattribute");

  class atkFileEditor extends atkNode
  {
    var $m_dir;
    var $m_filefilter; 
    
    /**
     * Constructor
     *
     * This function is called when a new atkFileEditor is instantiated.
     *
     * @param   dir   The directory that the fileeditor lists. If you
     *                want to enable addition of new files, make sure
     *                that the webserver has write access to this dir.
     *                Only regular files (not subdirs or special files)
     *                are listed.
     * @param   filter  A regular expression that is used to filter which
     *                  files will be shown. Example: '.txt$' lists only
     *                  txt files in the directory.
     *                  Note: This parameter is also used to validate the
     *                  filename of newly created files. This means, you 
     *                  cannot create a new file called test.css if your
     *                  filter param is set to '.tpl$';
     *                  Note 2: Watch out when using $ in your regular 
     *                  expression; PHP parses this, so use single quotes
     *                  or escape the dollarsign with \.
     * @param   flags   The node flags. See atkNode for a list of possible 
     *                  flags.
     */
    function atkFileEditor($name, $dir="", $filter="", $flags=0)
    {
      $this->atkNode($name, $flags|NF_ADD_LINK);
      $this->m_dir = $dir;
      if ($dir=="") $this->m_dir = "./";                
      
      // dir must have a trailing slash.
      if (substr($this->m_dir,-1)!="/") $this->m_dir.="/";
      
      $this->m_filefilter = $filter;
      
      $this->add(new atkAttribute("filename",AF_PRIMARY));
      $this->add(new atkTextAttribute("filecontent",30,AF_HIDE_LIST));
      
      $this->setOrder("dummy.filename");
      $this->setTable("dummy");
    }
    
    function setDir($dir)
    {
      $this->m_dir = $dir;
    }
    
    function countDb($selector)
    {
      $d = dir($this->m_dir);
      while (false !== ($entry = $d->read()))
      {
        // list only regular files.
        if (is_file($this->m_dir.$entry) && ($this->m_filefilter=="" || ereg($this->m_filefilter,$entry)))
        {
          $arr[] = $entry;
        }
      }
      $d->close();
      return count($arr);
    }
    
    function selectDb($selector="", $orderby="", $limit="")
    {   
      if ($selector=="")
      {
        // no file selected, generate list..
        
        $start=0;
        $max=-1; // no max            
      
        if (is_array($limit) && count($limit) == 2) 
        {
          $start = $limit["offset"];
          $max = $limit["limit"];        
        }
              
        $d = dir($this->m_dir);
        if($d->handle)
        {
          while (false !== ($entry = $d->read()))
          {
            // list only regular files.
            if (is_file($this->m_dir.$entry) && ($this->m_filefilter=="" || ereg($this->m_filefilter,$entry)))
            {
              
              $arr[] = $entry;
            }
          }
          $d->close();
        

          if (count($arr)>0)
          {
            if ($orderby=="dummy.filename DESC")
            {      
              rsort($arr);
            }
            else
            {
              sort($arr);
            }
          }
          
          $res = array();
    
          for ($i=0;$i<count($arr);$i++)
          {
            if ($i>=$start && ($max==-1||count($res)<$max))
            {
              $res[] = array('filename'=>$arr[$i]);
            }
          }      
        }
        else
        {
          atkdebug("Dir " . $this->m_dir . " could not be read");
        }
      }
      else
      {
        // file selected, read file.
        
        // in the fileeditor, the selector is always dummy.filename=name
        // so we use the value of the decoded pair as a filename.
        $decodedselector = decodeKeyValuePair($selector);      
        $filename = $decodedselector["dummy.filename"];
        $record['filename'] = $filename;
        
        // we must store original filename as primaryKey, for 
        // atknode uses the value in some places.
        $record['atkprimkey'] = $this->primaryKey($record); 
        if (is_file($this->m_dir.$filename))
        {
          $record['filecontent'] = implode("",file($this->m_dir.$filename));
        }
        else
        {
          atkdebug("File $filename not found");
        }
        $res[] = $record;
      }  
      return $res;
    }
    
    function validate(&$rec, $mode)
    {  
      if (!ereg($this->m_filefilter, $rec['filename']))
      {
        triggerError($rec, "filename","filename_invalid");
      }
      else if ($mode=="add" && file_exists($this->m_dir.$rec['filename']))
      {
        triggerError($rec, "filename","file_exists");
      }
    }
    
    function addDb($record)
    {
      $fp = @fopen($this->m_dir.$record['filename'],"wb");
      if ($fp==NULL)
      {
        atkerror("Unable to open file ".$record['filename']." for writing. (Is directory '".$this->m_dir."' readable by webserver?");
      }
      else
      {
        fwrite($fp, $record['filecontent']);
        fclose($fp);
        atkdebug("Wrote ".$record['filename']);
      }
      return true;
    }    
    
    function updateDb(&$record)
    {
      // The record that must be updated is indicated by 'atkorgkey'
      // (not by atkselector, since the primary key might have 
      // changed, so we use the atkorgkey, which is the value before
      // any update happened.)
         
      if ($record['atkprimkey']!="")
      {
        if ($record['atkprimkey']!=$this->primaryKey($record))
        {
        
          $decodedprimkey = decodeKeyValuePair($record['atkprimkey']);      
          $filename = $this->m_dir.$decodedprimkey["dummy.filename"];
          
          unlink($filename);
          atkdebug("Filename changed. Deleted original '$filename'.");        
        }
        $fp = @fopen($this->m_dir.$record['filename'], "wb");
        if ($fp==NULL)
        {
          atkerror("Unable to open file ".$record['filename']." for writing. (Is directory '".$this->m_dir."' readable by webserver?");
        }
        else
        {        
          fwrite($fp, $record['filecontent']);
          fclose($fp);
          atkdebug("Wrote ".$record['filename']);
          $record['atkprimkey'] = $record['filename'];
        }
        return true;
      }
      else
      {
        atkdebug("NOT UPDATING! NO SELECTOR SET!");
        return false;
      } 
    }
    
    function deleteDb($selector)
    {
      $decodedselector = decodeKeyValuePair($selector);      
      $filename = $decodedselector["dummy.filename"];
      if (strpos($filename, "..")===false)
      {
        unlink($this->m_dir.$filename);    
        atkdebug("Deleted ".$this->m_dir.$filename);
      }
      else
      {
        atkerror("Cannot unlink relative files. Possible hack attempt detected!");        
      }
      return true;
    }
  }
    
?>