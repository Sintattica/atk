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
  /** Global function, used in both this class and the popup version! */

  /**
    * Draws one colored <td>gif</td> box with javascript parameters in the desired height,width and color
   */
  function drawTD($field,$color,$colHeight,$colWidth,$extern=0)
   {
    $row .="<td width='".$colWidth."' bgcolor='$color'>";
    if ($extern !=0)
    {
      $row .='<A href ="javascript:remotePicker';
    }
     else
    {
      $row .='<A href ="javascript:picker';
    }
    $row .="('".$field."','".$color."')";
    $row .='" title="color: '.$color.'"><IMG SRC="atk/images/dummy.gif" border=0 width='.$colWidth.' height='.$colHeight.' ></a></td>';
    $row .= "\n";
    return $row;
   }

  /**
    * Returns a string with the entire colorPicker matrix
   */
  function colorMatrix($colHeight,$colWidth,$field,$extern=0,$userColors)
  {
    $webColors = array ("44","55","66","77","88","99","AA","BB","CC","DD","EE","FF"); // values to mix, websave 12 variants
    $stdColors  = array("5B919D","cc33cc","cc9933","6699cc","cc6699","669933","ff9900","cccc99","663399","5dabdd","cafe00","ffcc33"); // standard favorite colors
    $nrColors    = count($webColors);
    $matrix .= '<table width="'.($nrColors*$colW).'" border="0" cellspacing="0" cellpadding="0"><tr height="5"><td></td></tr><tr><td>';
    $matrix .= "\n";
    $matrix .= '<table width ="100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
    // red & blue
     for ($i=0;$i<$nrColors;$i++)
      {
        for ($j=0;$j<$nrColors;$j++)
          {
           $color = "#".$webColors[$i].$webColors[$j]."00";
           $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
           }
        $matrix .="</tr><tr>";
      }
    $matrix .='</tr></table></td><td>';
    $matrix .= "\n";
    $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
   // red & green 
     for ($i=0;$i<$nrColors;$i++)
      {
        for ($j=0;$j<$nrColors;$j++)
          {
           $color = "#".$webColors[$i]."00".$webColors[$j];
           $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
           }
        $matrix .="</tr><tr>";
      }
    $matrix .='</tr></table></td><td>';
    $matrix .= "\n";
    $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
   // green & blue
     for ($i=0;$i<$nrColors;$i++)
      {
        for ($j=0;$j<$nrColors;$j++)
          {
           $color = "#00".$webColors[$i].$webColors[$j];
           $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
           }
        $matrix .="</tr><tr>";
      }
    $matrix .='</tr></table></td><td></tr><tr><td>';
    $matrix .= "\n";
    $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
   // red only
     for ($i=0;$i<$nrColors;$i++)
      {
       $color = "#".$webColors[$i]."0000";
       $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
      }
    $matrix .='</tr></table></td><td>';
   // green only
    $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
     for ($i=0;$i<$nrColors;$i++)
      {
       $color = "#00".$webColors[$i]."00";
           $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
      } 
    $matrix .='</tr></table></td><td>';
    $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
  // blue only
     for ($i=0;$i<$nrColors;$i++)
      {
       $color = "#0000".$webColors[$i];
       $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
      } 
    $matrix .='</tr></table></td><td></tr><tr><td>';
    $matrix .= "\n";
    $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
   // grayscales
     for ($i=0;$i<$nrColors;$i++)
      {
       $color = "#".$webColors[$i].$webColors[$i].$webColors[$i];
       $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
      }
     $matrix .='</tr></table></td><td>';
     $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix .="\n";
   // predefined colors
     for ($i=0;$i<12;$i++)
      {
       $color = "#".$stdColors[$i];
       $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
      }
     $matrix .='</tr></table></td><td>'; 
     $matrix .= '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
  // user defined colors?
    $userColors = explode("|",$userColors);
    $nrCol = count($userColors);
    if (($nrCol >1) || $userColors[0] !="")
    {
      if ($nrCol > 12) $nrCol = 12; // max of 12 userdefined colors!
      for ($i=0;$i<$nrCol;$i++)
       {
         $color = "#".$userColors[$i];
         $matrix .= drawTD($field,$color,$colHeight,$colWidth,$extern);
        }
      }
    // colspan blank cells
    if (($colW-$nrCol) !=0)
      {
       $matrix .="<td colspan ='".$nrColors."'>";
       $matrix .='<IMG SRC="atk/images/dummy.gif" border=0 width='.$colWidth.' height='.$colHeight.'></td>';
      }
    $matrix .='</tr></table>';
    $matrix .= "\n";
    $matrix .="</td></tr></table>\n";
    return $matrix;
 }


  class atkColorPickerAttribute extends atkAttribute
  {
    var $m_userColors;
 
    /**
     * Constructor
     *
     * <b>Example:</b>
     * $this->addAttribute(new atkColorPickerAttribute("naam",Array,AF_OBLIGATORY|AF_POPUP));
     * @param $name Name of the attribute
     * @param $array   Array with max. 12 user defined colors 
     * @param $flags Flags for the attribute
     */

    function atkColorPickerAttribute($name ,$userColors="", $flags = 0)
    {
      $this->atkAttribute($name, $flags);
      $GLOBALS['g_layout']->register_script("atk/javascript/newwindow.js");      
      $GLOBALS['g_layout']->register_script("atk/javascript/colorpicker.js");            
      $this->m_userColors = $userColors;
     
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
             $colHeight = "11";
             $colWidth  = "11";
             $formRef   = $this->fieldName();
  
             if ($this->m_userColors !="")
             {      
             $temp = $this->m_userColors;
             $userCol = implode("|",$temp);
             }
      
        
             $result = '<input type="text" name="'.$fieldprefix.$this->formName().
                          '" value="'.htmlspecialchars($record[$this->fieldName()]).'"'.
                            ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                            ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      if ($this->hasFlag(AF_POPUP))
      {
        $url = atkPopup('colorpicker.php','form='.$this->formName().'&usercol='.$userCol,'colorPicker',190,400,'no','no');
        $text ='&nbsp;<a href ="'.$url.'">'.text("colorpicker_selectcolor").'</a>'; 
        $result .=$text;      
      }
      else
      {
      $result .= colorMatrix($colHeight,$colWidth,$formRef,0,$userCol);
      }
      return $result;
    }
   }
  
?>
