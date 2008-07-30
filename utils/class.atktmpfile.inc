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
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Temporary file handler.
   * 
   * This class can be used to create, read and remove temporary files.
   * The files are stored in ATK's temporary directory.
   * An ideal application of this class is writing small php include files
   * with cached data.
   * 
   * Note: superseded for caching by atkCache.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */
  class atkTmpFile
  {
  	/**
  	 * Filename as given to constructor.
  	 * Don't use this! Public because of bwc.
  	 *
  	 * @var string
  	 */
    public $m_filename;
    
    /**
     * Internal file pointer.
     * Don't use this! Public because of bwc.
     *
     * @var resource
     */
    public $m_fp;
    
    /**
     * Mode the file is opened in.
     * Don't use this! Public because of bwc.
     *
     * @var string
     */
    public $m_mode;

    /**
     * Create a new temporary file handler.
     *
     * @param string $filename
     * @return atkTmpFile
     */
    public function atkTmpFile($filename)
    {
      $this->m_filename = $filename;
    }
    
    /**
     * Create a new temporary file handler.
     * 
     * Factory method, allowing a shorter syntax.
     *
     * @param string $filename
     * @return atkTmpFile
     */
    public function create($filename)
    {
    	return new atkTmpFile($filename);
    }
    
    ////////////////// COMPLETE FILE ACTIONS (READ/WRITE/REMOVE) ///////////////////////

    /**
     * Returns the contents of the file in an array, split by newline (see PHPs 'file' function).
     * Returns false if the file does not exist.
     *
     * @return mixed
     */
    public function read()
    {
      if ($this->exists())
      {
        return file($this->getPath());
      }
      return false;
    }
    
    /**
     * Returns the contents of the file in a string.
     * Returns false if the file does not exist.
     *
     * @return string
     */
    public function readFile()
    {
      if ($this->exists())
      {
        if (function_exists("file_get_contents"))
        {
          return file_get_contents($this->getPath());
        }
        else return implode(null, $this->read());
      }
      return false;
    }
    
    /**
     * Send the file contents directly to the browser.
     *
     * @return bool Wether the action succeeded
     */
    public function fpassthru()
    {
      if ($this->open("r"))
      {
        fpassthru($this->m_fp);
        $this->close();
        return true;
      }
      return false;
    }

    /**
     * Write data to the file (creates the file if it does not exist and override any existing content).
     *
     * @param string $data Data to write to the file
     * @return bool Wether writing succeeded
     */
    public function writeFile($data)
    {
      if ($this->open("w"))
      {
        $this->write($data);
        $this->close();
        return true;
      }
      return false;
    }
    
    /**
     * Exports a PHP variable to a file, makes the file a PHP file.
     *
     * @param string $varname Name of the variable
     * @param string $data    Variable data
     * @return bool Wether the action succeeded
     */
    public function writeAsPhp($varname, $data)
    {
      $res = "<?php\n";
      $res.= "\$".$varname." = ".var_export($data, true);
      $res.= "\n?>";

      return $this->writeFile($res);
    }    
    
    /**
     * Append data to a file.
     *
     * @param string $data Data to append
     * @return bool Wether appending succeeded
     */
    public function appendToFile($data)
    {
    	if ($this->open('a'))
    	{
    		$this->write($data);
    		$this->close();
    		return true;
    	}
    	return false;
    }
    
    /**
     * Removes a file.
     * 
     * @return bool Wether removing succeeded
     */
    public function remove()
    {
      $this->close();
      return unlink($this->getPath());
    }
    
    ////////////////// GETTING FILE INFO ///////////////////////
    
    /**
     * Wether or not the file exists.
     *
     * @return bool Exists?
     */
    public function exists()
    {
      return file_exists($this->getPath());
    }
    
    /**
     * Returns the time the file was last changed, or FALSE in case of an error.
     * The time is returned as a Unix timestamp.
     *
     * @return int Timestamp last changed
     */
    public function filecTime()
    {
      if($this->exists()) return filectime($this->getPath());
      return false;
    }

    /**
     * Returns the file age in seconds
     *
     * @return int Seconds of file age.
     */
    public function fileAge()
    {
      $filectime = $this->filectime();
      if($filectime!=false) return (time()-$filectime);
      return false;
    }

    /**
     * Get the complete path of the file
     * 
     * Example: 
     * <code>$file->getPath(); => ./atktmp/tempdir/tempfile.inc</code>
     *
     * @return string Path for the file
     */
    public function getPath()
    {
      return atkconfig("atktempdir").$this->m_filename;
    }
    
    ////////////////// SIMPLE FILE OPERATIONS ///////////////////////

    /**
     * Open the file with a specific mode (see PHPs fopen).
     * Close it first if it's already open with a different mode.
     * And create the directory structure if we are writing to the file.
     *
     * @param string $mode Mode to open the file with
     * @return bool Wether opening succeeded
     */
    public function open($mode)
    {
      if ($this->m_mode!="" && $this->m_mode!=$mode)
      {
        // file is already open in different mode, close first
        $this->close();
      }
      if (is_null($this->m_fp))
      {
        if ($mode != 'r' && $mode != 'r+')
        {
          $this->createDirectoryStructure();  
        }
        
        $this->m_fp = fopen($this->getPath(), $mode);
        $this->m_mode = $mode;
      }
      return !is_null($this->m_fp);
    }

    /**
     * Write data to the current (open) file.
     *
     * @param string $data Data to write to the file
     * @return mixed Number of bytes written or false for error
     */
    public function write($data)
    {
      return fwrite($this->m_fp, $data);
    }

    /**
     * Close the current (open) file
     *
     * @return bool Wether we could close the file
     */
    public function close()
    {
      if (!is_null($this->m_fp))
      {
        fclose($this->m_fp);
        $this->m_mode="";
        $this->m_fp = null;
        return true;
      }
      return false;
    }
    
    ////////////////// MISC ///////////////////////
    
    /**
     * Recursively creates the directory structure for this file.
     * 
     * @return bool Wether we succeeded
     */
    public function createDirectoryStructure()
    {
      useattrib('atkfileattribute');
      return atkFileAttribute::mkdir(dirname($this->getPath()));
    }
  }

  ?>
