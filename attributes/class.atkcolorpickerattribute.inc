<?php

  /**  
   * The atkColorPickerAttribute class represents an attribute of an atkNode.
   * An atkColorPickerAttribute shows a box with 10 user defined colors and 90 pre-defined colors
   * from wich the user can select a color or enter the hexcode in a textfield
   * 
   * flags: AF_POPUP  opens a popup screen with colorpicker instead of drawing under the input field
   * 
   * @author Rene Bakx <rene@ibuildings.nl>
   * @version $Revision$
   *
   * $Id$
   */  

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
    $row .='" title="'.text('color').': '.$color.'"><IMG SRC="atk/images/dummy.gif" border=0 width='.$colWidth.' height='.$colHeight.' ></a></td>';
    $row .= "\n";

    return $row;
  }

  /**
    * Returns a string with the entire colorPicker matrix
   */
  function colorMatrix($colHeight,$colWidth,$field,$extern=0,$userColors)
  {
    $webColors = array ("00", "11", "22", "33", "44","55","66","77","88","99","AA","BB","CC","DD","EE","FF");
    $stdColors = array("000000","A4358E","C79476","6699cc","cc6699","669933","ff9900","cccc99","663399","5dabdd","cafe00","ffcc33"); // standard favorite colors
    $nrColors  = count($webColors);

   // Mix RED / BLUE (16x16 = 256 items)
    $matrix_rb  = '<table width ="100%" border="0" cellspacing="1" cellpadding="0">';
    $matrix_rb .= '<tr>';
    for ($i=0;$i<$nrColors;$i++)
    {
      for ($j=0;$j<$nrColors;$j++)
      {
        $color      = "#".$webColors[$i].$webColors[$j]."00";
        $matrix_rb .= drawTD($field, $color, $colHeight, $colWidth, $extern);
      }
      $matrix_rb .= '</tr><tr>';
    }
    $matrix_rb .= '</tr></table>';

   // Mix RED / GREEN (16x16 = 256 items)
    $matrix_rg  = '<table width ="100%" border="0" cellspacing="1" cellpadding="0">';
    $matrix_rg .= '<tr>';
    for ($i=0;$i<$nrColors;$i++)
    {
      for ($j=0;$j<$nrColors;$j++)
      {
        $color      = "#".$webColors[$i]."00".$webColors[$j];
        $matrix_rg .= drawTD($field, $color, $colHeight, $colWidth, $extern);
      }
      $matrix_rg .= '</tr><tr>';
    }
    $matrix_rg .= '</tr></table>';

   // Mix GREEN / BLUE (16x16 = 256 items)
    $matrix_gb  = '<table width ="100%" border="0" cellspacing="1" cellpadding="0">';
    $matrix_gb .= '<tr>';
    for ($i=0;$i<$nrColors;$i++)
    {
      for ($j=0;$j<$nrColors;$j++)
      {
        $color      = "#00".$webColors[$i].$webColors[$j];
        $matrix_gb .= drawTD($field, $color, $colHeight, $colWidth, $extern);
      }
      $matrix_gb .= '</tr><tr>';
    }
    $matrix_gb .= '</tr></table>';

   // Primary color shades and grayscales
    for ($i=0;$i<$nrColors;$i++)
    {
      $matrix_red   .= drawTD($field, "#".$webColors[$i]."0000", $colHeight, $colWidth, $extern);
      $matrix_green .= drawTD($field, "#00".$webColors[$i]."00", $colHeight, $colWidth, $extern);
      $matrix_blue  .= drawTD($field, "#0000".$webColors[$i], $colHeight, $colWidth, $extern);
      $matrix_grey  .= drawTD($field, "#".$webColors[$i].$webColors[$i].$webColors[$i], $colHeight, $colWidth, $extern);
    }
 
    $matrix_red   = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>'.$matrix_red.'</tr></table>';
    $matrix_green = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>'.$matrix_green.'</tr></table>';
    $matrix_blue  = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>'.$matrix_blue.'</tr></table>';
    $matrix_grey  = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>'.$matrix_grey.'</tr></table>';

    // Check optional user colors
    if (trim($userColors) != "")
    {
      $tmpColors = explode("|", $userColors);

      for ($i=0;$i<=count($tmpColors)-1;$i++)  
      {  
        $dummy = $tmpColors[$i];
        if (substr($dummy,0,1) != "#")
        {
          $dummy = "#".$dummy;
        }
        $matrix_user .= drawTD($field, $dummy, $colHeight, $colWidth, $extern);
      }
      $matrix_user = '<table border="0" cellspacing="1" cellpadding="0"><tr>'.$matrix_user.'</tr></table>';
    }

    return array($matrix_rb, $matrix_rg, $matrix_gb, $matrix_red, $matrix_green, $matrix_blue, $matrix_grey, $matrix_user);
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
      global $config_atkroot;
      
      // Call baseclass constructor. We set 10 as default size. An html 
      // colorvalue (#ffffaa) is 7 chars, but with 10 chars, we can also
      // save named colors like 'white' etc.
      $this->atkAttribute($name, $flags, 10);     

      $GLOBALS['g_layout']->register_script($config_atkroot."atk/javascript/newwindow.js");      
      $GLOBALS['g_layout']->register_script($config_atkroot."atk/javascript/colorpicker.js");            
      $this->m_userColors = $userColors;
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
  

    function edit($record="", $fieldprefix="")
    {
      global $config_atkroot;

      $colHeight  = "11";
      $colWidth   = "11";
      $formRef    = $this->fieldName();

      if ($this->currentColor == "")
      {
         $this->currentColor = $record[$fieldprefix.$this->formName()];	
      }

      $colorField = '<input type="hidden" name="'.$fieldprefix.$this->formName().'" value="'.$this->currentColor.'" size="7" maxlength="7">';
      
      if ($this->m_userColors !="")
      {
        $temp    = $this->m_userColors;
        $userCol = implode("|",$temp);
      }

      if (trim($this->currentColor) == "")
      {
        $selectImg = $config_atkroot."atk/images/select_color_off.gif";
        $alt       = text("no_color_selected");
      }
      else
      {
        $selectImg = $config_atkroot."atk/images/select_color_on.gif";
        $alt       = $this->currentColor;
      }

      // check if $this->currentColor starts with an # (to avoid parted url)
      if (substr($this->currentColor,0,1) == "#")
      {
        $urlColor = substr($this->currentColor,1);
      }
      else
      {
        $urlColor = $this->currentColor;
      }

      $url     = atkPopup('atk/popups/colorpicker.inc','field='.$fieldprefix.$this->fieldName().'&activecolor='.$urlColor.'&usercol='.$userCol,'colorPicker',315,600,'no','no');
      $text    = '<a href ="'.$url.'"><img name="img_'.$fieldprefix.$this->fieldName().'" src="'.$selectImg.'" border="0" alt="'.$alt.'"></a>'.$colorField; 
      $result  = "<table><tr><td valign='top' align='left'>";  
      $result .= " <table cellpadding='0' cellspacing='0' border='0' width='35' height='21'><tr><td id='example_".$fieldprefix.$this->fieldName()."' name='example_".$fieldprefix.$this->fieldName()."' valign='top' align='left' bgcolor='".$this->currentColor."'>".$text."</td></tr></table>";
      $result .= "</td><tr></table>";      

      return $result;
    }
    

    function dbFieldType()
    { 
      return "string";
    }
    
  } 
  
?>