<?php
  
  class atkTmpFile
  {
    var $m_filename;
    var $m_fp;
    var $m_mode;
    
    function atkTmpFile($filename)
    {
      $this->m_filename = $filename;
    }
    
    function exists()
    {
      return file_exists($this->getPath());
    }
    
    function read()
    {
      if ($this->exists())
      {
        return file($this->getPath());
      }
    }
    
    function writeFile($data)
    {
      if ($this->open("w"))
      {
        $this->write($data);
        $this->close();
        return true;
      }

      return false;
    }          

    function writeAsPhp($varname, $data)
    {
      $res = "<?php\n";
      $res.= "\$".$varname." = ".var_export($data, true);
      $res.= "\n?>";
      
      return $this->writeFile($res);
    }   
    
    function open($mode)
    {
      if ($this->m_mode!="" && $this->m_mode!=$mode)
      {
        // file is already open in different mode, close first
        $this->close();
      }
      if (is_null($this->m_fp))
      {
        $this->m_fp = fopen($this->getPath(), $mode);
        $this->m_mode = $mode;
      }      
      return !is_null($this->m_fp);
    }
    
    function write($data)
    {
      fwrite($this->m_fp, $data);
    }
    
    function close()
    {
      if (!is_null($this->m_fp))
      {
        fclose($this->m_fp);
        $this->m_mode="";
      }
    }
    
    function remove()
    {
      $this->close();
      unlink($this->getPath());
    }
    
    function getPath()
    {
      return atkconfig("atktempdir").$this->m_filename;
    }
  }