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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Some defines
   */
  define("TBL_HEADER", 1);    // Should the first row of the table be rendered as a header?
  define("TBL_FOOTER", 2);    // Should the last row of the table be rendered as a footer?
  define("TBL_ALTERNATE", 4); // Should each row alternate in color? If you want to use this
                              // flag, you should provide a style.
  define("TBL_DATA", TBL_HEADER|TBL_ALTERNATE); // Alias for easy rendering of data tables.

  define("TBL_LEFT", 1);
  define("TBL_RIGHT", 2);
  define("TBL_CENTER", 4);
  define("TBL_TOP",8);
  define("TBL_BOTTOM",16);
  define("TBL_MIDDLE",32);


  /**
   * Table renderer class. Makes rendering of html tables easier.
   *
   * The current implementation cannot handle merged cells (rowspan/colspan).
   * If you need this, you should render the table manually.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Edgar van Ispelen <edgar@ibuildings.nl>
   * @package atk
   * @subpackage utils
   *
   */
  class atkTableRenderer
  {
    var $m_cellalignment = array();
    var $m_rowalignment = array();
    var $m_colalignment = array();
    var $m_span = array();
    var $m_defaultalignment = TBL_LEFT;
    var $m_cellclass = array();
    var $m_rowclass = array();
    var $m_colclass = array();
    var $m_defaultclass = "";
    var $m_maxWidth = 0;
    var $m_flags = 0;
    var $m_style = '';

    /**
     * Constructs a new table renderer
     *
     * @param int $flags flags to set (allowed: TBL_HEADER, TBL_FOOTER,
     * TBL_ALTERNATE and TBL_DATA)
     * @param string $style style class to set for the table
     * @param string $module module to register style with
     */
    function atkTableRenderer($flags = 0, $style='', $module='')
    {
      $this->m_flags = $flags;
      
      if ($style !== '')
      {
        $this->setTableStyle($style, $module);
      }
    }
    
    /**
     * Sets a flag on this object. Allowed: TBL_HEADER, TBL_FOOTER,
     * TBL_ALTERNATE and TBL_DATA
     *
     * @param int $flag
     */
    function setFlag($flag)
    {
      $this->m_flags |= $flag;
    }

    /**
     * Set the default alignment for all cells in the table.
     *
     * @param int $alignment The desired alignment (TBL_LEFT, TBL_RIGHT or
     *                       TBL_CENTER).
     */
    function setDefaultAlignment($alignment)
    {
      $this->m_defaultalignment = $alignment;
    }

    /**
     * Set the alignment for a particular cell.
     *
     * @param int $row The row to set.
     * @param int $col The column to set.
     * @param int $alignment The desired alignment (TBL_LEFT, TBL_RIGHT or
     *                       TBL_CENTER).
     */
    function setAlignment($row, $col, $alignment)
    {
      $this->m_cellalignment[$row][$col] = $alignment;
    }

    /**
     * Set the alignment for an entire row.
     *
     * @param int $row The row to set.
     * @param int $alignment The desired alignment (TBL_LEFT, TBL_RIGHT or
     *                       TBL_CENTER).
     */
    function setRowAlignment($row, $alignment)
    {
      $this->m_rowalignment[$row] = $alignment;
    }

    /**
     * Set the alignment for an entire column.
     *
     * @param int $col The column to set.
     * @param int $alignment The desired alignment (TBL_LEFT, TBL_RIGHT or
     *                       TBL_CENTER).
     */
    function setColAlignment($col, $alignment)
    {
      $this->m_colalignment[$col] = $alignment;
    }

    /**
     * Set the default class for all cells in the table.
     *
     * @param string $class Classname to be used by default for table cells
     */
    function setDefaultClass($class)
    {
      $this->m_defaultclass = $class;
    }

    /**
     * Set the class for a particular cell.
     *
     * @param int $row The row to set.
     * @param int $col The column to set.
     * @param string $class Classname to be used by default for table cells
     */
    function setClass($row, $col, $class)
    {
      $this->m_cellclass[$row][$col] = $class;
    }

    /**
     * Set the class for an entire row.
     *
     * @param int $row The row to set.
     * @param string $class Classname to be used by default for table cells
     */
    function setRowClass($row, $class)
    {
      $this->m_rowclass[$row] = $class;
    }

    /**
     * Set the class for an entire column.
     *
     * @param int $col The column to set.
     * @param string $class Classname to be used by default for table cells
     */
    function setColClass($col, $class)
    {
      $this->m_colclass[$col] = $class;
    }

    /**
     * Sets spanning on a specifc column. 
     *
     * @param int $row row number
     * @param int $col row column
     * @param int $span width
     * @param string $type
     */
    function _setSpan($row,$col,$span,$type="col")
    {
      if(array_key_exists($row, $this->m_span) && is_array($this->m_span[$row][$col]))
      {
        $this->m_span[$row][$col] = array_merge(array($type."span"=>$span),$this->m_span[$row][$col]);
      }
      else
      {
        $this->m_span[$row][$col] = array($type."span"=>$span);
      }
    }

    /**
     * Set the colspan for a cel
     *
     * @param int $row The row to set
     * @param int $col The col to set
     * @param int $span The span width
     */
    function setColSpan($row,$col,$span)
    {
      $this->_setSpan($row,$col,$span,"col");
    }

    /**
     * Set the rowspan for a cel
     *
     * @param int $row The row to set
     * @param int $col The col to set
     * @param int $span The span width
     */
    function setRowSpan($row,$col,$span)
    {
      $this->_setSpan($row,$col,$span,"row");
    }
    
    /**
     * Registers a style with the page.
     *
     * @param string $style styleclass name
     * @param string $module module to look for stylesheets
     */
    function registerStyle($style,$module="")
    {
      $theme = &atkinstance("atk.ui.atktheme");
      $page = &atkPage::getInstance();
      $page->register_style($theme->stylePath($style.".css",$module));
    }
    
    /**
     * Gets the maximum number of columns in a row
     *
     * @param aray $data table data
     * @return int max number of rows
     */
    function _getMaxCols($data)
    {
      $max = 0;
      for($i=0, $_i=count($data); $i<$_i; $i++)
      {
        $max = max(count($data[$i]), $max);
      }
      return $max;
    }
    
    /**
     * Render a table.
     *
     * <b>Example:</b>
     * <code>
     *  $tbl = &atknew("atk.utils.atktablerenderer");
     *  $tbl->render($data, TBL_HEADER|TBL_ALTERNATE, "recordlist");
     * </code>
     *
     * @param array  $data   A multidimensional array containing the rows and
     *                       columns with data. The first dimension represents
     *                       the rows. The second dimension represents the
     *                       cols.
     *                       If rows to not contain a complete amount of cols
     *                       (compared to the other rows), the row is
     *                       automatically filled upon the right by a spacer
     *                       cell.
     * @param int    $flags  (obsolete). Set flags in constructor or setFlag()
     *                       One or more bitwise flags that influence the way
     *                       the table is rendered.
     *                       Valid flags:
     *                       - TBL_HEADER: the first row will be rendered as
     *                                     a table header.
     *                       - TBL_FOOTER: the last row will be rendered as a
     *                                     table footer.
     *                       - TBL_ALTERNATE: The rows of the table should
     *                                        alternate in color.
     *                       - TBL_DATA: Alias for TBL_HEADER|TBL_ALTERNATE.
     * @param string $style  (obsolete) Use the constructor parameter or setTableStyle()
     *                       The style to render the table in (without .css
     *                       extension).
     * @param string $module (obsolete) Use the constructor parameter or setTableStyle()
     *                       The module where is style is located.
     *
     * @return String The rendered table.
     */
    function render($data, $flags=0, $style="",$module="")
    {
      $this->setFlag($flags);
      if ($style !== "")
      {
        $this->setTableStyle($style, $module);
      }
      $output = $this->_renderTable($data);
      
      return $output;
    }
    
    /**
     * Sets the table style class and registers it in the result page 
     *
     * @param string $class styleclass name
     * @param string $module module to look for stylesheets
     */
    function setTableStyle($class, $module="")
    {
      $this->m_style = $class;
      $this->registerStyle($class, $module);
    }
    
    /**
     * Gets the table style attribute component
     *
     * @return string
     */
    function _getTableStyleStr()
    {
      $class = '';
      if (!empty($this->m_style))
      {
        $class = "class=\"{$this->m_style}\"";
      }
      
      return $class;
    }
    
    /**
     * Renders the table. 
     *
     * @param array $data 
     * @return html of the rendered table
     */
    function _renderTable($data)
    {     
      $this->m_maxWidth = $this->_getMaxCols($data);
      $class = $this->_getTableStyleStr();
      
      $output = "\n<table {$class}>";
      
      // should render a header?
      if (hasFlag($this->m_flags, TBL_HEADER))
      {
        $header = $data[0];
        $rowOffset = 1;
        $output .= $this->_renderHeader($header, 0);
      }
      
      // should render a footer
      if (hasFlag($this->m_flags, TBL_FOOTER))
      {
        $footer_row = count($data) - 1;
        $footer = array_pop($data);
        
        $output .= $this->_renderBody($data, $rowOffset);
        $output .= $this->_renderFooter($footer, $footer_row);
      }
      else
      {
        $output .= $this->_renderBody($data);
      }
      
      $output .= "</table>\n";
      
      $this->m_maxWidth = 0; // reset
      
      return $output;
    }
    
    /**
     * Renders the value inside one cell.
     * Supports the cell having id/value.
     * This method only extracts the value.
     *
     * @param array|string $cell
     * @return unknown
     */
    function _renderValue($cell)
    {
      $value = "";
      if(is_array($cell))
      {
        // Use the value field, if available
        if(isset($cell['value']))
          $value = $cell['value'];
      }
      else
      {
        $value = (string)$cell;
        if($value == '')
        {
          $value = '&nbsp;';
        }
      }
        
      return $value;
    }
    
    /**
     * Renders one cell in the table.
     *
     * @param array|string $data value of the cell
     * @param int $row row number of current cell
     * @param int $col column number of current cell
     * @param string $type type of tag to use. mostly 'td'
     * @return string rendered cell
     */
    function _renderCell($data, $row, $col, $type)
    {
      $span     = $this->getSpan($row, $col);
      $spanstr  = $this->_spanStr($span);
      $alignstr = $this->_alignmentStr($this->getAlignment($row, $col));
      $idstr    = $this->_getIdStr($data);
      $class    = $this->_classStr($this->getClass($row, $col));
      
      $output  = "\n<{$type} {$idstr} {$class} {$spanstr} {$alignstr}>";
      $output .= $this->_renderValue($data);
      $output .= "</{$type}>";
      return $output;
    }
    
    /**
     * Renders one header row.
     *
     * @param array $data row data
     * @param int $row rownumber of the header, mostly 0
     * @return string rendered header row
     */
    function _renderHeader($data, $row)
    {
      $output = '<thead>';
      $output .= $this->_renderRow($data, $row, 'th');
      $output .= '</thead>';
      return $output;
    }
    /**
     * Renders one footer row.
     *
     * @param array $data row data
     * @param int $row rownumber of the footer
     * @return string rendered footer row
     */
    function _renderFooter($data, $row)
    {
      $output = '<tfoot>';
      $output .= $this->_renderRow($data, $row, 'th');
      $output .= '</tfoot>';
      return $output;
    }
    
    /**
     * Gets the styleclass for a rownumber
     * Supports alternating rowclasses if TBL_ALTERNATE is set
     * 
     * @param string|int $row row number or rowclassname
     * @return string html class string like 'class=row1'
     */
    function _getRowClassStr($row)
    {
      if (is_string($row))
      {
        return 'class="'.$row.'"';
      }
      else if (hasFlag($this->m_flags, TBL_ALTERNATE) && ($row % 2) !== 0)
      {
        return 'class="row2"';
      }
      else
      {
       return 'class="row1"';
      }
    }

    /**
     * Renders one row in the table.
     *
     * @param array $data rowdata array
     * @param int $row rownumber
     * @param string $columntype tagname to use to render columns inside this row
     * @return string html of the row
     */
    function _renderRow($data, $row, $columntype='td')
    {      
      $class = $this->_getRowClassStr($row);
      
      $output = "\n<tr {$class}>";
      
      // render all cells inside
      $colCount = count($data);
      for ($col=0; $col<$colCount; $col++)
      {
        $output .= $this->_renderCell($data[$col], $row, $col, $columntype);
      }
     
      // fill up remaining space
      $filling = $this->m_maxWidth - $colCount;
      if ($filling > 0)
      {
        $output .= $this->_renderFillingCell($filling, $columntype);
      }
       
      $output .= "\n</tr>";
      
      return $output;
    }
    
    /**
     * Renders a filling cell used to display a correct two-dimensional table.
     *
     * @param int $colspan columns to span
     * @param string $type tagtype to use
     * @return string html filling cell
     */
    function _renderFillingCell($colspan, $type)
    {
      return "<{$type} colspan=\"{$colspan}\">&nbsp;</{$type}>";
    }

    /**
     * Renders the body of the table. The body contains all rows except the
     * header and footer rows. Surrounds the body with tbody tags.
     *
     * @param array $data all body rows
     * @param int $rowOffset how many header rows are drawn before the body
     * @return string html table body
     */
    function _renderBody($data, $rowOffset = 0)
    {
      $output = '<tbody>';
      
      $rowCount = count($data);
      for ($row=$rowOffset; $row<$rowCount; $row++)
      {
        $rowData = $data[$row];
        $output .= $this->_renderRow($rowData, $row);
      }
      
      $output .= '</tbody>';
      return $output;
    }

    
    /**
     * Gets the cell id string if available.
     *
     * @param array $data cell data
     * @return string html id string like 'id="1"';
     */
    function _getIdStr($data)
    {
      $id = '';
      if(is_array($data) && isset($data['id']))
      {
        $id = "id=\"{$data['id']}\"";
      }

      return $id;
    }
    
    /**
     * Gets the alignmentstring for an alignment flag
     *
     * @param int $alignment alignmentflag TBL_[LEFT, RIGHT, CENTER, BOTTOM or MIDDLE]
     * @return string html alignment for a cell or row
     */
    function _alignmentStr($alignment)
    {
      $ret='';
      if(hasFlag($alignment,TBL_LEFT)) $ret.='align="left" ';
      elseif(hasFLag($alignment,TBL_RIGHT)) $ret.='align="right" ';
      elseif(hasFlag($alignment,TBL_CENTER)) $ret.='align="center" ';

      if(hasFlag($alignment,TBL_TOP)) $ret.='valign="top" ';
      elseif(hasFLag($alignment,TBL_BOTTOM)) $ret.='valign="bottom" ';
      elseif(hasFlag($alignment,TBL_MIDDLE)) $ret.='valign="middle" ';

      return $ret;
    }
    
    /**
     * Gets the span string for a cell
     *
     * @param array $span array containing spanning info
     * @return string html spanning string
     */
    function _spanStr($span)
    {
      if ($span["rowspan"]=="" && $span["colspan"]=="")  return '';

      if($span["rowspan"]!="" && $span["colspan"]!="")
      {
        return 'rowspan="'.$span["rowspan"].'" colspan="'.$span["colspan"].'"';
      }
      elseif($span["rowspan"]!="")
      {
        return 'rowspan="'.$span["rowspan"].'"';
      }
      elseif($span["colspan"]!="")
      {
        return 'colspan="'.$span["colspan"].'"';
      }
    }
    
    /**
     * Constructs the html class attribute string using a classname
     *
     * @access private
     * @param String $class Classname
     * @return String HTML Class attribute string
     */
    function _classStr($class)
    {
      if (empty($class))
        return "";
      else
        return 'class="'.$class.'"';
    }

    /**
     * Gets the alignment for a specific cell
     *
     * @param int $row row nuber
     * @param int $col column number
     * @return string aligment
     */
    function getAlignment($row, $col)
    {
      if (array_key_exists($row, $this->m_cellalignment) && array_key_exists($col, $this->m_cellalignment[$row]) && $this->m_cellalignment[$row][$col]!="") return $this->m_cellalignment[$row][$col];
      if (array_key_exists($col, $this->m_colalignment) && $this->m_colalignment[$col]!="") return $this->m_colalignment[$col];
      if (array_key_exists($row, $this->m_rowalignment) && $this->m_rowalignment[$row]!="") return $this->m_rowalignment[$row];
      return $this->m_defaultalignment;
    }

    /**
     * Determines the class to be set for a specific cell
     *
     * @param Integer $row Row number of the cell
     * @param Integer $col Column number of the cell
     * @return String Class string of the cell
     */
    function getClass($row, $col)
    {
      if (array_key_exists($row, $this->m_cellclass) && array_key_exists($col, $this->m_cellclass[$row]) && $this->m_cellclass[$row][$col]!="") return $this->m_cellclass[$row][$col];
      if (array_key_exists($col, $this->m_colclass) && $this->m_colclass[$col]!="") return $this->m_colclass[$col];
      if (array_key_exists($row, $this->m_rowclass) && $this->m_rowclass[$row]!="") return $this->m_rowclass[$row];
      return $this->m_defaultclass;
    }

    /**
     * Gets the spanning for a specific cell
     *
     * @param int $row row number of the cell
     * @param int $col column number of the cell
     * @return bool|string spanning or false if nothing found for the cell
     */
    function getSpan($row,$col)
    {
      if(array_key_exists($row,$this->m_span) &&
         array_key_exists($col,$this->m_span[$row]) &&
         is_array($this->m_span[$row][$col])) return $this->m_span[$row][$col];
      return false;
    }
  }

?>
