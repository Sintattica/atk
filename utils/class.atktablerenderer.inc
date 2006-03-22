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
  define("TBL_CENTER", 3);

  /**
   * Table renderer class. Makes rendering of html tables easier.
   *
   * The current implementation cannot handle merged cells (rowspan/colspan).
   * If you need this, you should render the table manually.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   *
   */
  class atkTableRenderer
  {
    var $m_cellalignment = array();
    var $m_rowalignment = array();
    var $m_colalignment = array();
    var $m_defaultalignment = TBL_LEFT;

    /**
     * Default constructor
     */
    function atkTableRenderer()
    {
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
     * @param int    $flags  One or more bitwise flags that influence the way
     *                       the table is rendered.
     *                       Valid flags:
     *                       - TBL_HEADER: the first row will be rendered as
     *                                     a table header.
     *                       - TBL_FOOTER: the last row will be rendered as a
     *                                     table footer.
     *                       - TBL_ALTERNATE: The rows of the table should
     *                                        alternate in color.
     *                       - TBL_DATA: Alias for TBL_HEADER|TBL_ALTERNATE.
     * @param string $style  The style to render the table in (without .css
     *                       extension).
     * @return String The rendered table.
     */
    function render($data, $flags=0, $style="")
    {
      if ($style!="")
      {
        $this->registerStyle($style);
      }

      // Analyze. If not all rows have the same number of columns, we should pad the end.
      $max = $this->_getMaxCols($data);

      // Render
      $output.= $this->_start($style);

      for ($row=0, $_row=count($data); $row<$_row; $row++)
      {
        $output.='<tr class="row'.(hasFlag($flags, TBL_ALTERNATE)?(($row%2)==0?1:2):1).'">';

        if (($row==0&&hasFlag($flags, TBL_HEADER)) || ($row==$_row-1&&hasFlag($flags, TBL_FOOTER)))
        {
          $pre = "<th"; $post = "</th>";
        }
        else
        {
          $pre = "<td"; $post = "</td>";
        }

        for ($col=0, $_col=count($data[$row]); $col<$_col; $col++)
        {
          $align = $this->_alignmentStr($this->getAlignment($row, $col));
          $output.=$pre." ".$align.">".$data[$row][$col].$post;
        }

        // Check if we must pad
        $align = $this->_alignmentStr($this->getAlignment($row, $col));
        if ($col==$max-2)
        {
          // Must add only one
          $output.=$pre." ".$align.">&nbsp;".$post;
        }
        else if ($col<$max-2)
        {
          // Must add more
          $output.=$pre." ".$align.' colspan="'.($max-$col-1).'">&nbsp;'.$post;
        }

        $output.="</tr>\n";
      }

      $output.= $this->_end($style);

      return $output;
    }

    /**
     * @access private
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

    function registerStyle($style)
    {
      $theme = &atkinstance("atk.ui.atktheme");
      $page = &atkPage::getInstance();
      $page->register_style($theme->stylePath($style.".css"));
    }

    /**
     * @access private
     */
    function _start($style)
    {
      return "<table".($style!=""?' class="'.$style.'"':'').">";
    }

    /**
     * @access private
     */
    function _end($style)
    {
      return "</table>";
    }

    /**
     * @access private
     */
    function _alignmentStr($alignment)
    {
      switch ($alignment)
      {
        case TBL_LEFT: return 'align="left"';
        case TBL_RIGHT: return 'align="right"';
        case TBL_CENTER: return 'align="center"';
        default: return '';
      }
    }

    function getAlignment($row, $col)
    {
      if (array_key_exists($row, $this->m_cellalignment) && array_key_exists($col, $this->m_cellalignment[$row]) && $this->m_cellalignment[$row][$col]!="") return $this->m_cellalignment[$row][$col];
      if (array_key_exists($col, $this->m_colalignment) && $this->m_colalignment[$col]!="") return $this->m_colalignment[$col];
      if (array_key_exists($row, $this->m_rowalignment) && $this->m_rowalignment[$row]!="") return $this->m_rowalignment[$row];
      return $this->m_defaultalignment;
    }
  }

?>