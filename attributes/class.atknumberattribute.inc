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
   
  /**
   * The atkNumberAttribute can be used for numeric values.
   *   
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkNumberAttribute extends atkAttribute
  {
    var $m_decimals=0; // The number of decimals of the number. 
  
    /**
     * Constructor
     * @param $name  Name of the attribute
     * @param $flags Flags for this attribute
     * @param $size  The size(s) for this attribute (default 10)
     * @param $decimals The number of decimals to use. 
     *               
     */
    function atkNumberAttribute($name, $flags=0, $size=10, $decimals=0)
    {
      $this->atkAttribute($name,$flags|AF_NO_QUOTES, $size); // base class constructor
      $this->m_decimals = $decimals;
    }
    
    function display($record, $mode="")
    {      
      return atkNumberAttribute::translateSeparator(sprintf("%.".$this->m_decimals."f", $record[$this->fieldName()]));
    }
    
    /**
     * Replace decimal separator (from the language file "decimal_separator") 
     * with the standard separator ('.') so, for instance,  99,95 would become 
     * 99.95 when language is set to 'nl'.
     * @static
     * @param String $number The number that needs to be converted
     * @return String The converted number 
     */
    function standardiseSeparator($number)
    {      
      return str_replace(atktext("decimal_separator"),".",$number);     
    }
    
    /**
     * Replace standard decimal separator ('.') with the one from the language 
     * file so, for instance, 99.95 would be converted to 99,95 when language
     * is set to 'nl'.
     * @static
     * @param String $number The number that needs to be converted
     * @return String The converted number 
     */
    function translateSeparator($number)
    {
      return str_replace(".", atktext("decimal_separator"), $number);      
    }

    /**
     * Validates if value is numeric
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */    
    function validate(&$record, $mode)
    {            
      if (!is_numeric($record[$this->fieldName()])&&$record[$this->fieldName()]!="") 
         triggerError($record, $this->fieldName(), 'error_notnumeric');
    }
    
    /**
     * Convert values from an HTML form posting to an internal value for 
     * this attribute. 
     *
     * If the user entered a number in his native language, he may have used
     * a different decimal separator, which we first convert to the '.'
     * standard separator (ATK uses the regular dot notation internally)
     *
     * @param array $postvars The array with html posted values ($_POST, for 
     *                        example) that holds this attribute's value.
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
      return atkNumberAttribute::standardiseSeparator($postvars[$this->fieldName()]);
    }
    
    
    function value2db($rec)
    {
      if($rec[$this->fieldName()]==""&&!$this->hasFlag(AF_OBLIGATORY))
        return "NULL";

      if($this->m_decimals>0)
        return round((float) $rec[$this->fieldName()], $this->m_decimals);
      else
        return (int) $rec[$this->fieldName()];
    }

    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("between", "exact","greaterthan","greaterthanequal","lessthan","lessthanequal");
    }
  
    function dbFieldType()
    {
      return ($this->m_decimals>0?"decimal":"number");
    }
    
    function dbFieldSize()
    {
      return $this->m_maxsize.($this->m_decimals>0?",".$this->m_decimals:"");
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with values
     * @param $fieldprefix The attribute must use this to prefix its form elements (used for
     *                     embedded forms)
     * @return Piece of htmlcode
     */
    function edit($record="", $fieldprefix="")
    {      
      $maxsize = $this->m_maxsize;
      if ($this->m_decimals>0)
      {
        $maxsize += ($this->m_decimals+1); // make room for the number of decimals
      }
      
      $value = atkNumberAttribute::translateSeparator(sprintf("%.".$this->m_decimals."f", $record[$this->fieldName()]));
      
      $id = $fieldprefix.$this->fieldName();      
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);  
      $result = '<input type="text" id="'.$id.'" name="'.$id.
                '" value="'.$value.'"'.
                ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                ($maxsize > 0 ? ' maxlength="'.$maxsize.'"' : '').'>';

      return $result;
    }
    
   /**
     * Returns a piece of html code that can be used to search for an
     * attribute's value.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function search($record="", $extended=false, $fieldprefix="")
    {
      $value = $record[$this->fieldName()];
      if (!$extended) 
      {
        if (is_array($value))
        {
          if ($value["from"]!=""||$value["to"]!="")
          {
             // Two values were entered in the extended search screen.
             $record[$this->fieldName()] = $value["from"]."/".$value["to"];
          }
          else
          {
            $record[$this->fieldName()] = "";
          }
        }
        return parent::search($record, $extended);
      }
      else
      {
        $id = "atksearch".$fieldprefix."[".$this->formName()."][from]";
        $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);  
        $result= '<input type="text" id="'.$id.'" name="'.$id.'" value="'.$value.'"'.
                  ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                  ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';
                  
        $id = "atksearch".$fieldprefix."[".$this->formName()."][to]";
        $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);  
        $result.= " (".text("until").' <input type="text" id="'.$id.'" name="'.$id.'" value="'.$value.'"'.
                  ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                  ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>)';                  
        return $result;
      }
    }
    
   /**
     * Creates an search condition for a given search value
     * @param $table the table name
     * @param $value the search value
     * @return a piece of the SQL where clause
     */
    function searchCondition(&$query, $table, $value, $searchmode)
    {  
      $query->addSearchCondition($this->getSearchCondition($query, $table, $value, $searchmode));
    }
    
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
    	if (!is_array($value))
      {        
        $slash = strpos($value, "/"); // Support for 'from/to' searches.
        $str = $value;
        if ($slash!==false)
        {                    
          $value = array();
          $value["from"]=substr($str, 0, $slash);
          $value["to"]=substr($str,$slash+1);         
          $searchmode="between";          
        }
        else
        {
          $value = array();
          $value["from"] = $str; // quicksearch doesn't use an array.
        }
      }
      if ($searchmode!="between") 
      {        
        // Use default handler, and use only the first field.
        return parent::getSearchCondition($query, $table, $value["from"], $searchmode);
      }
      else
      {       
        $fieldname = $table.".".$this->fieldName();
        if ($value["from"]!="" && $value["to"]!="")
        {
          if ($value["from"]>$value["to"])
          {
            // User entered fields in wrong order. Let's fix that.
            $tmp = $value["from"]; $value["from"] = $value["to"]; $value["to"] = $tmp;
          }
          $searchcondition = $query->betweenCondition($fieldname,escapeSQL($value["from"]), escapeSQL($value["to"])); 
        }
        else if ($value["from"]!="" && $value["to"]=="")
        {
          $searchcondition = $query->greaterthanequalCondition($fieldname, $value["from"]);
        }
        else if ($value["from"]=="" && $value["to"]!="")
        {
          $searchcondition = $query->lessthanequalCondition($fieldname, $value["to"]);
        }
      }
      return $searchcondition;
    }
  }
?>
