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

  useattrib("atkattribute");

  /**
   * Draws one colored <td>gif</td> box with javascript parameters in the desired height,width and color
   * @access private
   */
  function drawTD($field,$color,$colHeight,$colWidth,$extern=0)
  {
    $row  ="<td width='".$colWidth."' bgcolor='$color'>";
    if ($extern !=0)
    {
      $row .='<A href ="javascript:remotePicker';
    }
     else
    {
      $row .='<A href ="javascript:picker';
    }
    $row .="('".$field."','".$color."')";
    $row .='" title="'.atktext("color", "atk").': '.$color.'"><IMG SRC="'.atkconfig('atkroot').'atk/images/dummy.gif" border=0 width='.$colWidth.' height='.$colHeight.' alt="-X-"></a></td>';
    $row .= "\n";

    return $row;
  }

  /**
   * Returns a string with the entire colorPicker matrix
   * @access private
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

    $matrix_red   = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix_green = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix_blue  = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';
    $matrix_grey  = '<table width = "100%" border="0" cellspacing="1" cellpadding="0"><tr>';

    // Primary color shades and grayscales
    for ($i=0;$i<$nrColors;$i++)
    {
      $matrix_red   .= drawTD($field, "#".$webColors[$i]."0000", $colHeight, $colWidth, $extern);
      $matrix_green .= drawTD($field, "#00".$webColors[$i]."00", $colHeight, $colWidth, $extern);
      $matrix_blue  .= drawTD($field, "#0000".$webColors[$i], $colHeight, $colWidth, $extern);
      $matrix_grey  .= drawTD($field, "#".$webColors[$i].$webColors[$i].$webColors[$i], $colHeight, $colWidth, $extern);
    }

    $matrix_red   .= '</tr></table>';
    $matrix_green .= '</tr></table>';
    $matrix_blue  .= '</tr></table>';
    $matrix_grey  .= '</tr></table>';

    // Check optional user colors
    if (trim($userColors) != "")
    {
      $tmpColors = explode("|", $userColors);

      $matrix_user = '<table border="0" cellspacing="1" cellpadding="0"><tr>';
      for ($i=0;$i<=count($tmpColors)-1;$i++)
      {
        $dummy = $tmpColors[$i];
        if (substr($dummy,0,1) != "#")
        {
          $dummy = "#".$dummy;
        }
        $matrix_user .= drawTD($field, $dummy, $colHeight, $colWidth, $extern);
      }
      $matrix_user .= '</tr></table>';
    }
    else
    {
      $matrix_user = '';
    }

    return array($matrix_rb, $matrix_rg, $matrix_gb, $matrix_red, $matrix_green, $matrix_blue, $matrix_grey, $matrix_user);
  }

  /**
   * Function to check if a color is correct (default is hex colors)
   * Example:
   * check_color("#zff, "hex"")
   * check_color("rgb(204,255,0)", "rgb")
   * @access private
   * @return boolean valid color or not
   */
  function check_color()
  {
    $numargs = func_num_args();
    if($numargs < 1) { atkerror("Error in function check_color(): No arguments passed to function."); return false; }
    $colors = func_get_arg(0);
    if($numargs == 2) { $type = func_get_arg(1); }else{ $type = "hex"; }
    if($numargs > 2) { atkerror("Error in function check_color(): Too many arguments passed to function."); return false; }

    // color types -
    //          hex - hexidecimal value in form of #XXX, XXX, #XXXXXX, or XXXXXX
    //          rgb - rgb value in form of rgb(X,Y,Z);
    if($type == "hex")
    {
      $colors = strtolower(str_replace("#", "", $colors));
      // \d is all decimal digits (0-9)
      $t = preg_match_all("([\da-f])", $colors, $matches);
      $length = strlen($colors);

      // Have to match the preg_match_all with the length to prevent incorrect results
      // else 3 matches in a 6 character code would make something like 123x98 valid
      if(($t == 3 and $length == 3) or ($t == 6 and $length == 6)){ return true; } else { return false; }
    }
    elseif($type == "rgb")
    {
      $error = 0;
      // check the start and the end of the color code to make sure it's a valid form
      if((substr($colors, 0, 4)) != "rgb(" or substr($colors, -1) != ")"){ $error++; }
      else
      {
        $len = strlen($colors);
        // the full length - 5 characters to make up for the "rgb(" and the ")" - 5 characters total
        $lenone = $len - 5;

        // $colors is now only the values and the commas (",") that seperate them
        // rgb(51,102,153) is now 51,102,153 - we need this to explode the data in the next step
        $colors = substr($colors, 4, $lenone);

        // explode the color substring - should be in X,Y,Z format now.
        // $extras takes any characters more than the X,Y,Z format.
        list($r, $g, $b, $extras) = explode(",",$colors);

        // Check the rgb values that were just exploded.
        if($r < 0 or $r > 255 or $g < 0 or $g > 255 or $b < 0 or $b > 255) { $error++; }

        // Make sure that none of the values are blank  - prevents rgb(12) or rgb(12,12) from being valid
        if($r == "" or $g == "" or $b == ""){ $error++; }

        // \D in the character class means any non-decimal character except newline
        $rt = preg_match_all("[\D]", $r, $matches);
        $gt = preg_match_all("[\D]", $g, $matches);
        $bt = preg_match_all("[\D]", $b, $matches);

        // If we matched any characters above the values are not valid.
        // Also is there is anything in $extras - the whole color code is an invalid form!
        if($rt > 0 or $gt > 0 or $bt > 0 or $extras != ""){ $error++; }

        // If there are any errors then the color is not valid, return false, otherwise
        // we're okay and we retun true
        if($error > 0) { return false; } else { return true; }
      }
    }
    else
    {
      atkerror("Error in function check_color(): Invalid argument for color type.");
      return false;
    }
  }

  /**
   * The atkColorPickerAttribute class represents an attribute of an atkNode.
   * An atkColorPickerAttribute shows a box with 10 user defined colors and 90 pre-defined colors
   * from wich the user can select a color or enter the hexcode in a textfield
   *
   * flags: AF_POPUP  opens a popup screen with colorpicker instead of drawing under the input field
   * @todo There are many global functions in the class file
   *       class.atkcolorpickerattribute.inc. These should be moved to private
   *       class methods.
   * @author Rene Bakx <rene@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkColorPickerAttribute extends atkAttribute
  {
    var $m_userColors;
    var $m_currentColor;

    /**
     * Constructor
     *
     * <b>Example:</b>
     * $this->add(new atkColorPickerAttribute("naam",Array,AF_OBLIGATORY|AF_POPUP));
     * @param $name Name of the attribute
     * @param $array   Array with max. 12 user defined colors
     * @param $flags Flags for the attribute
     */

    function atkColorPickerAttribute($name, $userColors="", $flags = 0)
    {
      // Call baseclass constructor. We set 10 as default size. An html
      // colorvalue (#ffffaa) is 7 chars, but with 10 chars, we can also
      // save named colors like 'white' etc.
      $this->atkAttribute($name, $flags, 10);

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
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/colorpicker.js");

      $colHeight  = "11";
      $colWidth   = "11";
      $formRef    = $this->fieldName();

      if ($this->m_currentColor == "")
      {
         $this->m_currentColor = atkArrayNvl($record, $this->fieldName(), "");
      }

      $colorField = '<input type="hidden" name="'.$fieldprefix.$this->formName().'" value="'.$this->m_currentColor.'" size="7" maxlength="7">';

      if ($this->m_userColors !="")
      {
        $temp    = $this->m_userColors;
        $userCol = implode("|",$temp);
      }
      else
      {
        $userCol = "";
      }

      if (trim($this->m_currentColor) == "")
      {
        $selectImg = atkconfig("atkroot")."atk/images/select_color_off.gif";
        $alt       = atkText("no_color_selected", "atk");
      }
      else
      {
        $selectImg = atkconfig("atkroot")."atk/images/select_color_on.gif";
        $alt       = $this->m_currentColor;
      }

      // check if $this->m_currentColor starts with an # (to avoid parted url)
      if (substr($this->m_currentColor,0,1) == "#")
      {
        $urlColor = substr($this->m_currentColor,1);
      }
      else
      {
        $urlColor = $this->m_currentColor;
      }

      $url    = atkPopup('atk/popups/colorpicker.inc','field='.$fieldprefix.$this->fieldName().'&activecolor='.$urlColor.'&usercol='.$userCol,'colorPicker',315,600,'no','no');
      $text   = '<a href="javascript:void(0);" onclick="'.$url.'; return false;"><img name="img_'.$fieldprefix.$this->fieldName().'" src="'.$selectImg.'" border="0" alt="'.$alt.'"></a>'.$colorField;
      $result = '<table cellpadding="0" cellspacing="0" border="0" width="35" height="21" align="left"><tr><td id="example_'.$fieldprefix.$this->fieldName().'" name="example_'.$fieldprefix.$this->fieldName().'" style="padding:0px;" valign="top" align="left" bgcolor="'.$this->m_currentColor.'">'.$text.'</td></tr></table>';

      return $result;
    }

    function validate(&$record, $mode)
    {
      $color = $record[$this->fieldName()];
      if(!check_color($color))
        triggerError($record, $this->fieldName(), 'error_invalid_color');
    }

    function dbFieldType()
    {
      return "string";
    }
  }

?>