<?php
  /**
   * The atkFileWriterAttribute is an attribute that reads data from / saves 
   * data to a file, instead of the database.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   * @version $Revision$
   *
   * $Id$      
   *
   */

  useattrib("atkTextAttribute");
  include_once(atkconfig("atkroot")."atk/utils/class.atkstringparser.inc");

  class atkFileWriterAttribute extends atkTextAttribute
  {
    var $m_filename;
  
    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $filename The name of the file to read/write. 
     *                  Advanced use: This may be a template containing 
     *                  fields from your class, for example: 
     *                  "somedir/textfile_[id].txt". If a record has id '9', 
     *                  this will read/write a file named 
     *                  somedir/textfile_9.txt. Watch out when using fields
     *                  that can change; the attribute won't remove the old
     *                  files.
     * @param $flags Flags for this attribute
     */
    function atkFileWriterAttribute($name, $filename, $flags=0)
    {
      $this->m_filename = $filename;
      $this->atkTextAttribute($name,30,$flags); // base class constructor
    }

    function value2db($record)
    {
      // Note1 : regardless of mode (update or add), we always overwrite the 
      // file with the current contents.
      // Note 2: even if contents is "", we write the file, since the file 
      // might exist and contain old information.
      
      $contents = $record[$this->fieldName()];            
      
      $parser = new atkStringParser($this->m_filename);
 
      if (!$parser->isComplete($record))
      {
        // record does not contain all data. Let's lazy load.
        atkdebug("[atkfilewriter] Lazy loading rest of record to complete filename.");
        $recs = $this->m_ownerInstance->selectDb($record["atkprimkey"]);
        $record = $recs[0];
        //atk_var_dump($record, 'this is what we have now');
     }

      $filename = $parser->parse($record);      

      $fp = @fopen($filename, "w");
      
      if ($fp==false)
      {
        atkerror("[".$this->fieldName()."] couldn't open $filename for writing!");
      }      
      else
      {
        fwrite($fp, $contents);
        fclose($fp);
        atkdebug("[".$this->fieldName()."] succesfully wrote $filename..");
      }      
      return escapeSQL($contents);
    }
    
    function db2value($record)
    {
      // determine filename.
      $filename = stringparse($this->m_filename, $record);
        
      if (!file_exists($filename))
      {
        atkdebug("[".$this->fieldName()."] warning: $filename doesn't exist");
        return $record[$this->fieldName()];
      }
      else
      {
        if ($record[$this->fieldName()]=="")
        {
          // db is empty. if file contains stuff, use that.          
          $contents = implode("",file($filename));
          atkdebug("[".$this->fieldName()."] succesfully read $filename");
          return $contents;
        }
        else
        {
          return $record[$this->fieldName()];
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
      return array();
    }
  
    
  }
  
?>
