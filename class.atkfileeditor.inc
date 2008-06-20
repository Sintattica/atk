<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * @internal used attribute
   */
  useattrib("atktextattribute");

  /**
   * File editing node.
   *
   * This is a special derivative atkNode that does not have database
   * interaction, but that can be used to edit files in a directory on the
   * server.
   *
   * Note: This class does not support postAdd, postUpdate and postDelete
   * hooks. Other overrides may or me not be supported, but this has not been
   * tested.
   *
   * Derived classes need not add attributes. The only thing to specify in
   * derived classes is the baseclass constructor, which can be configured
   * with parameters. (See constuctor documentation)
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   *
   */
  class atkFileEditor extends atkNode
  {
    var $m_dir;
    var $m_basedir;
    var $m_filefilter;
    
    /**
     * Constructor
     *
     * This function is called when a new atkFileEditor is instantiated.
     *
     * @param String $name The name of the node.
     * @param String $dir The directory that the fileeditor lists. If you
     *                    want to enable addition of new files, make sure
     *                    that the webserver has write access to this dir.
     *                    Only regular files (not subdirs or special files)
     *                    are listed.
     * @param String $filter A regular expression that is used to filter which
     *                       files will be shown. Example: '.txt$' lists only
     *                       txt files in the directory.
     *                       Note: This parameter is also used to validate the
     *                       filename of newly created files. This means, you
     *                       cannot create a new file called test.css if your
     *                       filter param is set to '.tpl$';
     *                       Note 2: Watch out when using $ in your regular
     *                       expression; PHP parses this, so use single quotes
     *                       or escape the dollarsign with \.
     * @param int $flags The node flags. See atkNode for a list of possible
     *                   flags.
     */
    function atkFileEditor($name, $dir="", $filter="", $flags=0)
    {
      $this->atkNode($name, $flags|NF_ADD_LINK);
      $this->m_dir = $dir;
      $this->m_basedir = $dir;
      if ($dir=="") $this->m_dir = "./";

      // dir must have a trailing slash.
      if (substr($this->m_dir,-1)!="/") $this->m_dir.="/";

      $this->m_filefilter = $filter;

      $this->add(new atkAttribute("filename",AF_PRIMARY|AF_SEARCHABLE));
      $this->add(new atkTextAttribute("filecontent",30,AF_HIDE_LIST));

      $this->setOrder("dummy.filename");
      $this->setTable("dummy");
    }

   /**
    * This function is used to change the node directory
    *
    * @param String $dir The name of the dir to change to
    */
    function setDir($dir)
    {
      $this->m_dir = $dir;
    }

   /**
    * This function reads the contents of the directory
    *
    * @param String $selector The selected item in the directory list
    * @return array The array containing the directory and file names
    *               from the currently selected directory
    */
    function countDb($selector)
    {
      $d = dir($this->m_dir);
      $arr = array();
      while (false !== ($entry = $d->read()))
      {
        $this->addFileEntry($entry,$arr);
      }
      $d->close();
      return count($arr);
    }

   /**
    * This function adds a file or directory to the list in the node
    *
    * @param String $entry The directory that is added to the list
    * @param array $arr The array containing the result of the
    *                   added item
    */
    function addFileEntry($entry,&$arr)
    {
      // First we check if we are about to search and set a few things
      // if we are
      $searching=false;

      /* there may be search criteria, which we also filter */
      $searchArray = @$this->m_postvars['atksearch'];
      if (isset($searchArray["filename"]) && $searchArray["filename"]!="")
      {
        $searching = true;
      }

      // list only regular files or directories
      if (is_file($this->m_dir.$entry) && ($this->m_filefilter=="" || ereg($this->m_filefilter,$entry)))
      {
        if($searching===true && (ereg($searchArray["filename"],$entry)))
        {
          $arr[] = $entry;
        }
        elseif($searching==false)
        {
          $arr[] = $entry;
        }
      }
      elseif(is_dir($this->m_dir.$entry))
      {
        if (!($entry == '.' || $entry == 'CVS' || $entry == '.svn'))
        {
          if(!($this->stripDir($this->m_basedir) == $this->stripDir($this->m_dir) && $entry == '..'))
          {
            $arr[] = $entry;
          }
        }
      }
    }

   /**
    * This function sets the actions of the items in the list
    *
    * @param String $record Identifier for the record
    * @param  array $actions Result array containing the options
    * @param  unknow $mraactions 
    */
    function recordActions($record, &$actions, &$mraactions)
    {
      $this->m_dir = $this->stripDir($this->m_dir);
      if (is_dir($this->m_dir."/".$record["filename"]))
      {        
        $actions['view'] = dispatch_url($this->atkNodeType(), "dirchange", array('atkselector' => $this->m_dir.$record["filename"]));
        unset($actions["edit"]);
        unset($actions["delete"]);
        return;
      }
      
      // Remove edit/delete actions when a file is not writeable.
      if(!is_writeable($this->m_dir."/".$record["filename"]))
      {
        unset($actions["edit"]);
        unset($actions["delete"]);
      }
    }

   /**
    * This function loops through the items in a directory and
    * and calls functions to print the results
    *
    * @param String $selector Identifier for the selected item
    * @param String $orderby The list of items is ordered by the item type
    *                        mentioned in this variable
    * @param unknown $limit
    * @return unknown
    */
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
          $arr=array();
          while (false !== ($entry = $d->read()))
          {
            $this->addFileEntry($entry,$arr);
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

   /**
    * This function controls actions on the selected file is allowed
    *
    * @param Array $rec Array that contains the identifier of the record
    * @param unknown_type $mode
    */
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

   /**
    * This function prints the current directory at the top of the list
    *
    * @return String The text containing the directory name
    */
    function adminHeader()
    {
      return "<p><b>".$this->text('current_dir').": ".substr_replace($this->m_dir,'',0,strlen($this->m_basedir))."</b></p>";
    }

   /**
    * This function overrides the addDb function to add 
    * a file to the selected directory
    *
    * @param Array $record Array that contains the name of the new file
    * @return Boolean The result of the file addition
    */
    function addDb($record)
    {
      $sessmngr = atkGetSessionManager();
      $this->m_dir = $this->stripDir($sessmngr->stackVar('dirname'));
      $fp = @fopen($this->m_dir.$record['filename'],"wb");
      if ($fp==NULL)
      {
        atkerror("Unable to open file ".$record['filename']." for writing. (Is directory '".$this->m_dir."' readable by webserver?");
        return false;
      }
      else
      {
        fwrite($fp, $record['filecontent']);
        fclose($fp);
        atkdebug("Wrote ".$record['filename']);
      }
      return true;
    }

   /**
    * This function overrides the updateDb function to update 
    * the contents of a file
    *
    * @param array $record Array that contains the name of the file that
    *                      is updated
    * @return Boolean The result of the file update 
    */
    function updateDb(&$record)
    {
      // The record that must be updated is indicated by 'atkorgkey'
      // (not by atkselector, since the primary key might have
      // changed, so we use the atkorgkey, which is the value before
      // any update happened.)
      $sessmngr = atkGetSessionManager();
      $this->m_dir = $this->stripDir($sessmngr->stackVar('dirname'));
     
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

   /**
    * This function overrides the deleteDb function to delete a file
    * from the selected directory
    *
    * @param String $selector The identifier of the file that should be deleted
    * @return boolean The result of the file deletion
    */
    function deleteDb($selector)
    {
      $sessmngr = atkGetSessionManager();
      $this->m_dir = $this->stripDir($sessmngr->stackVar('dirname'));
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
    
   /**
    * This function implements the functionality to move up 
    * and down directories
    *
    */
    function action_dirchange()
    { 
      $selectedDir = $this->stripDir($this->m_postvars['atkselector']);
      $sessmngr = atkGetSessionManager();
      ($sessmngr->stackVar('dirname', $selectedDir));
              
      $this->m_dir = $selectedDir;
      $this->callHandler('admin');
    }
   
   /**
    * This function strips a given directory to a valid relative path
    *
    * @param String $dirname Path of the dir to change to
    * @return String Stripped directory path
    */
    function stripDir($dirname)
    {
      // normalizes the given string to a relative dir that should always start with the base directory
      if (strpos(realpath($dirname), realpath($this->m_basedir))===0)
      {
        $resultdir = rtrim(str_replace(realpath($this->m_basedir), $this->m_basedir, realpath($dirname)),'/').'/';
        if ($resultdir == '' || !is_dir($resultdir))
        {
          $resultdir = rtrim($this->m_basedir,'/').'/';
        }
      }
      else
      {
        $resultdir = rtrim($this->m_basedir,'/').'/';
      }
      return $resultdir;
    }
  }

?>