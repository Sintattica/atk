<?php
  /**
   * WARNING: this attribute is now obsolete. It has been replaced by the much better atkFileAttribute.
   * It's left here in the source for backward comparibility reasons.
   * @author Ivo (ivo@achievo.com)
   *
   * @version 1.0
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:08  ivo
   * Initial revision
   *
   * Revision 1.2  2001/02/22 22:46:17  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:57  sandy
   * Achievo Tool Kit
   *
   */
  class atkImgAttribute extends atkAttribute
  {
    /**
     * Directory with images
     */
    var $m_dir = "";

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $dir Directory with images
     * @param $flags Flags for this attribute
     */
    function atkImgAttribute($name, $dir, $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_dir = $dir;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with fields
     * @param $size Not used
     * @param $maxsize Not used
     * @return piece of html code with a browsebox
     */
    function edit($record="")
    {
      $result=$this->display($record);

      if ($result!="")
      {
        $result.="<br>";
        $result.='<input type="hidden" name="'.$this->fieldName().'_orgfilename" value="'.$record[$this->fieldName()]['orgfilename'].'">';
      }

      $result .= '<input type="file" name="'.$this->fieldName().'">';
      return $result;
    }
    
    /**
     * VOID implementation.. you can't search imgattribute at the moment.. 
     * and as a matter of fact, you never will.. (use atkfileattribute instead)
     * @param $record Array with fields
     * @param $size Not used
     * @param $maxsize Not used
     * @return piece of html code with a browsebox
     */
    function search($record="")
    {
      return "&nbsp;";
    }    

    /**
     * Convert value to record for database
     * @param $rec Array with Fields
     * @return Nothing or Fieldname or Original filename
     */
    function value2db($rec)
    {
      $filename = $rec[$this->fieldName()]["filename"];
      if ($filename!="")
      {
        if (copy($rec[$this->fieldName()]["tmpfile"],$this->m_dir.$filename))
        {
          return $filename;
        }
      }
      return $rec[$this->fieldName()]["orgfilename"];
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

    function fetchValue($rec)
    {
      $basename = $this->fieldName();

      // Var is set to 'none' by php if no file is uploaded..
      if ($rec[$basename]=="none")
      {
        return Array("tmpfile"=>"","filename"=>"","filesize"=>0,"orgfilename"=>$rec[$basename."_orgfilename"]);
      }

      return Array("tmpfile"=>$rec[$basename],"filename"=>$rec[$basename."_name"],"filesize"=>$rec[$basename."_size"],"orgfilename"=>$rec[$basename."_name"]);
    }

    /**
     * Display values
     * @param $record Array with fields
     * @return Filename or Nothing
     */
    function display($record)
    {
      $img = $record[$this->fieldName()]["orgfilename"];
      if ($img!="")
      {
        return '<img src="'.$this->m_dir.$img.'" border="0">';
      }
      return "";
    }
  }
?>
