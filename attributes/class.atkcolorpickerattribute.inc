<?php
                                                             
  /**  
   * The atkColorPickerAttribute class represents an attribute of an atkNode.
   * An atkColorPickerAttribute shows a box with 10 user defined colors and 90 pre-defined colors
   * from wich the user can select a color or enter the hexcode in a textfield
   * 
   * 
   * flags: AF_POPUP  opens a popup screen with colorpicker instead of drawing under the input field
   * <br>
   * @author Rene Bakx (rene@ibuildings.nl)
   *
   */
  class atkColorPickerAttribute
  {
    /**
     * The name of the attribute
     */
    var $m_name;
    var $colHeight;
    var $colWidth;
    var $userColors;
    var $m_flags = 0;

    /**
     * The owner of the attribute (set by atknode)
     */
    var $m_owner = "";
    
    /**
     * Owner Instance
     */
    var $m_ownerInstance="";

    /**
     * The size, searchsize and maxsize of the attribute's field.
     */
    var $m_size = 0;
    var $m_searchsize = 0;
    var $m_maxsize = 0;

    /**
     * All meta data for the table this attribute is "part off"
     */
    var $m_tableMeta = "";

    /**
     * Constructor
     *
     * <b>Example:</b>
     *        $this->addAttribute(new atkAttribute("naam",AF_OBLIGATORY));
     * @param $name Name of the attribute
     * @param $flags Flags for the attribute
     */

    function atkColorPickerAttribute($name ,$userColors="", $flags = 0)
    {
      $GLOBALS['g_layout']->register_script("atk/javascript/newwindow.js");      
      $GLOBALS['g_layout']->register_script("atk/javascript/colorpicker.js");      
      $this->m_name = $name;
      $this->m_flags = (double)$flags;
      $this->userColors = $userColors;
      }

    /**
     * Checks if flag exist
     * @param $flag the flag you want to check
     * @return True or False
     */
    function hasFlag($flag)
    {
      return (($this->m_flags & $flag) == $flag);
    }

// makes ONE colored box
function drawTD($color)

{

 $row .="<td bgcolor='$color'>";
 $row .='<A class="picker" href ="javascript:picker';
 $row .="('".$this->formRef."','".$color."')";
 $row .='"><IMG SRC="atk\images\dummy.gif" border=0 width='.$this->colWidth.' height='.$this->colHeight.' ></a></td>';
 $row .= "\n";
return $row;
}

//builds  the entire colorPicker Matrix table definition
function colorMatrix()
{
 $webColors = array ("66","77","88","99","AA","BB","CC","DD","EE","FF"); // values to mix, can be extended with more combo's
 $matrix .= '<BR><BR><table width="100%" border="1" cellspacing="0" cellpadding="0"><tr><td>';
 $matrix .= "\n";
 $matrix .= '<table width = "100%" border="0" cellspacing="2" cellpadding="0"><tr>';
 $matrix .="\n";
 for ($i=0;$i<count($webColors);$i++)
 // red
 { 
 $color = "#".$webColors[$i]."0000";
 $matrix  .= $this->drawTD($color); 
 }
  // red -> green
 $j=count($webColors)-1;
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#".$webColors[$j].$webColors[$i]."00";
 $j--;
 $matrix  .= $this->drawTD($color); 
 } 

 $matrix .="</tr><tr>\n";
 // green
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#00".$webColors[$i]."00";
 $matrix  .= $this->drawTD($color); 
 }
   // green -> blue
 $j=count($webColors)-1;
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#00".$webColors[$j].$webColors[$i];
 $j--;
 $matrix  .= $this->drawTD($color); 
 } 
 $matrix .="</tr><tr>\n";

 // blue
 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#0000".$webColors[$i];
  $matrix  .= $this->drawTD($color); 
 }

 // blue -> red
 $j=count($webColors)-1;
 for ($i=0;$i<count($webColors);$i++)
 { 
 $color = "#".$webColors[$i]."00".$webColors[$j];
 $j--;
 $matrix  .= $this->drawTD($color); 
 } 

 $matrix .="</tr><tr>\n";
 // red & blue

 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#".$webColors[$i]."00".$webColors[$i];
  $matrix  .= $this->drawTD($color); 
 }
 
 // green & blue

 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#00".$webColors[$i].$webColors[$i];
  $matrix  .= $this->drawTD($color); 
 }
 $matrix .="</tr><tr>\n";

// red & green

 for ($i=0;$i<count($webColors);$i++)
 { 
  $color = "#".$webColors[$i].$webColors[$i]."00";
  $matrix  .= $this->drawTD($color); 
 }
 if (count($this->userColors) == 0 || $this->userColors[0] =="")
  {
 // predefined user colors
   $color = "#000000";
   $matrix  .= $this->drawTD($color);
   $color = "#333333";
   $matrix  .= $this->drawTD($color);
   $color = "#666666";
   $matrix  .= $this->drawTD($color);
   $color = "#999999";
   $matrix  .= $this->drawTD($color);
   $color = "#CCCCCC";
   $matrix  .= $this->drawTD($color);
   $color = "#FFFFFF";
   $matrix  .= $this->drawTD($color);
   $color = "#FF0000";
   $matrix  .= $this->drawTD($color);
   $color = "#00FF00";
   $matrix  .= $this->drawTD($color);
   $color = "#0000FF";
   $matrix  .= $this->drawTD($color);
   $color = "#0CAFE0";
   $matrix  .= $this->drawTD($color);
 }

else
{
$nrCol = count($this->userColors);
if ($nrCol > 10) $nrCol = 10;

for ($i=0;$i<$nrCol;$i++)
  {
  $color = "#".$this->userColors[$i];
  $matrix  .= $this->drawTD($color); 
  }
}
 $matrix .='</tr></table>';
 $matrix .= "\n";
 $matrix .="</td></tr></table>\n";

return $matrix;
}

    /**
     * Returns the fieldname
     * @return fieldname
     */
    function fieldName()
    {
      return $this->m_name;
    }

    /**
     * Returns the formname (fixed from atkNode
     * @return formname
     */
    function formName()
    {
      return $this->m_name;
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      return ($postvars[$this->fieldName()]=="");
    }

    /**
     * Add's slashes to the string for the database
     * @param $rec Array with values
     * @return String with slashes
     */
    function value2db($rec)
    {
      return escapeSQL($rec[$this->fieldName()]);
    }

    /**
     * Removes slashes from the string
     * @param $rec Array with values
     * @return String without slashes
     */
    function db2value($rec)
    {
      return stripslashes($rec[$this->fieldName()]);
    }


    /**
     * Fetch values
     * @param $rec Array with values
     * @return String without slashes
     */
    function fetchValue($rec)
    {
      return $rec[$this->fieldName()];
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
             $this->colHeight = "18";
             $this->colWidth  = "18";
             $this->formRef   = $this->fieldName();
             
             if ($this->userColors !="")
             {
             $temp = $this->userColors;
             $userCol = implode("|",$temp);
             }
             $matrix = $this->colorMatrix();
             $url = "javascript:OpenChild('colorpicker.php?form=".$this->formRef = $this->fieldName()."&usercol=".$userCol."')";
             $text ='&nbsp;<a href ="'.$url.'">'.text("colorpicker_label").'</a>'; 
        
             $result = '<input type="text" name="'.$fieldprefix.$this->formName().
                          '" value="'.htmlentities($record[$this->fieldName()]).'"'.
                            ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                            ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      if ($this->hasFlag(AF_POPUP))
      {
        $result .=$text;      
      }
      else
      {
      $result .= $matrix;
      }
      return $result;
    }
    
    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().
                '" value="'.$record[$this->fieldName()].'">';
      return $result;             
    }

    /**
     * Returns a piece of html code that can be used to search for an
     * attribute's value.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function search($record="", $extended=false)
    {
      $result= '<input type="text" name="atksearch['.$this->formName().']" value="'.$record[$this->fieldName()].'"'.
                ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';                      
      return $result;
    }        

    /**
     * Creates an search condition for a given search value
     * @param $table the table name
     * @param $value the search value
     * @return a piece of the SQL where clause
     */
    function searchCondition(&$query, $table, $value, $searchmode)
    {      
      $func = $searchmode."Condition";
      if (method_exists($query,$func))
      {
        $query->addCondition($query->$func($table.".".$this->fieldName(),escapeSQL($value)));
      }
      else
      {
        atkdebug("Database doesn't support searchmode '$searchmode', ignoring condition.");
      }
    }

    /**
     * Returns a displayable string for this value.
     * Since we always display in html, but we don't expext the text
     * to be html, we have to do some conversions.
     * if you don't like this, use the atkHtmlAttribute.
     * @param $record
     * @return String ready for displaying
     * @see atkHtmlAttribute
     */
    function display($record)
    {
      // default behaviour is that we display a value 'as is'.
      return nl2br(htmlentities($record[$this->fieldName()]));
    }

    /**
     * Validate's values
     * @param $value Value to be validate
     * @return Nothing
     */
    function validate($value)
    {
      return "";
    }

    /**
     * Add's fields to query
     * @param $query The SQL query
     * @param $tablename Tablename
     * @param $fieldaliasprefix Field alias prefix
     * @param $level ?
     * @param $mode select/insert/update
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "select")
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      else if ($mode == "add" || $mode == "edit")
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
    }

    /**
     * delete is only of interest for special attributes like relations, or file attributes.
     * @param $record record that has to be delete
     */
    function delete($record)
    {
      // delete is only of interest for special attributes like relations, or file attributes.
    }
    
    /**
     * returns a record which is the sum of two records
     */
    function sum($rec1, $rec2)
    {
      return Array($this->fieldName()=>($rec1[$this->fieldName()]+$rec2[$this->fieldName()]));
    }
    
    /**
     * Fetch the metadata about this attrib from the table metadata
     */    
    function fetchMeta($metadata)
    {
      $this->m_tableMeta = $metadata[$this->m_name];
    }
    
    /**
     * This function is called right after the attribute is added to the node.
     * Attributes may do special initialisation here..
     */
    function init()
    {
    }

  }
  
?>
