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
   * @copyright (c)2007 Ibuildings BV
   * @author Edgar van Ispelen <edgar@ibuildings.nl>
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id $
   */

  /**
   * Imports
   * @access private
   */
  atkimport("atk.utils.atktablerenderer");

  /**
   * Renders table with row based groups. A grouped table is a table with some rows
   * grouped together. They are visually folded and can be expanded by clicking.
   *
   * This feature can be used to organise (long) tables where you only want
   * to see a portion of logically grouped rows.
   *
   * By specifying which rows form a group the renderer can
   * fold the rows so they are invisible. A group header row is displayed so,
   * by clicking on it the visibility of the rows in the group is toggled.
   * Please note that you can mix normal rows with grouped rows. See the data
   * format example below.
   *
   *
   * Use the following data format as the first argument to the render method.
   * Format:
   *
   * <code>
   * $data = array(
   *  array('headercol1', 'headercol2'),  // header cannot be grouped, rendered as normal row
   *  array('a1', 'b1'),                  // normal row
   *  array('a2', 'b2'),                  // normal row
   *  array(
   *    'type' => 'group',      // set this so the renderer knows it's a group
   *    'id'  => 'groupname1',  // name of the group (unique), used as html id attribute
   *    'collapsed' => true,    // should the group be collapsed initially? optional, default is true
   *    'header' => 'My Group', // the header row to display, use array format like a normal row or a string
   *    'rows' => array(        // the rows to be grouped together
   *      array('a3', 'b3'),
   *      array('a4', 'b4'),
   *      array('a5', 'b5'),
   *    )
   *  array('a6', 'b6'),                  // normal row
   *  ),
   *  array('footercol1', 'footercol2'), // cannot be grouped, normal row
   * )
   * </code>
   * @author Edgar van Ispelen <edgar@ibuildings.nl>
   * @package atk
   * @subpackage utils
   */
  class atkTableRendererGrouped extends atkTableRenderer
  {
    /**
     * Used in counting rows
     *
     * @var unknown_type
     * @access private
     */
    var $m_currentRow = 0;

    /**
     * Constructor
     *
     * @param int $flags flags to set on this object (see setFlag)
     * @param string $style stylesheet file (basename) to use like: 'recordlist'.
     * @param string $module module name to register style with
     * @return atkTableRendererGrouped
     * @access public
     */
    function atkTableRendererGrouped($flags = 0, $style = '', $module = '')
    {
      $this->atkTableRenderer($flags, $style, $module);
      $this->setClass('groupheader', 0, 'firstcol');
      // Include tools.js for the toggleDisplay function
      $page = &atkPage::getInstance();
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
    }

    /**
     * Renders the body of the table. The body contains all rows except the
     * header and footer rows. Surrounds the body with tbody tags.
     *
     * @param array $data all body rows
     * @param int $rowOffset how many header rows are drawn before the body
     * @return string html table body
     * @access protected
     */
    function _renderBody($data, $rowOffset = 0)
    {
      $output = '';
      $tbodyOpened = false;
      $max = $this->_getMaxCols($data);

      $entryCount = count($data);
      $this->m_currentRow = $rowOffset;
      for ($entry=$rowOffset; $entry<$entryCount; $entry++)
      {
        $entryData = $data[$entry];

        // render a group or a single row
        if (is_array($entryData) && $this->_isGroup($entryData))
        {
          if ($tbodyOpened)
          {
            $output .= '</tbody>';
            $tbodyOpened = false;
          }
          $output .= $this->_renderRowGroup($entryData, $this->m_currentRow, $max);
        }
        else
        {
          if (!$tbodyOpened)
          {
            $output .= '<tbody>';
            $tbodyOpened = true;
          }
          $output .= $this->_renderRow($entryData, $this->m_currentRow);
          $this->m_currentRow++;
        }
      }

      if ($tbodyOpened)
      {
        $output .= '</tbody>';
      }

      $this->m_currentRow = 0; // reset

      return $output;
    }

    /**
     * Determine if the data is a row group
     *
     * @param array $entry data
     * @return bool true if it is a group false otherwise
     * @access protected
     */
    function _isGroup($entry)
    {
      if (array_key_exists('type', $entry) && $entry['type'] === 'group')
      {
        return true;
      }
      return false;
    }

    /**
     * Renders the header of a group
     *
     * @param array $group group data to show header of
     * @param int $width span of the header cell
     * @return string html of the header
     * @access protected
     */
    function _renderRowGroupHeader($group, $width)
    {
      $onClick = "onClick=\"
        window.toggleDisplay('', document.getElementById('{$group['id']}'));
        if (this.className == 'groupheader')
        this.className = 'groupheader_expanded';
        else
        this.className = 'groupheader';\"";

      $class = 'groupheader';
      if (!$this->_isCollapsedGroup($group))
      {
        $class = 'groupheader_expanded';
      }
      $output  = "<tbody class=\"$class\" {$onClick}>";

      // try to render the header
      if (!isset($group['header']))
      {
        // fallback to first row/first col
        $header = '<tr><th class="groupheader" colspan="'.$width.'">'.
            $this->_renderValue($group['rows'][0][0]).
            '</th></tr>';
      }
      else if (is_array($group['header']))
      {
         // draw as normal row
        $header = $this->_renderRow($group['header'], 'groupheader', 'th');
      }
      else
      {
        // render the value
        $header = "<tr><th class=\"groupheader\" colspan=\"{$width}\">
            {$group['header']}
            </th></tr>";
      }

      $output .= $header;
      $output .= '</tbody>';
      return $output;
    }

    /**
     * Checks if the group should be displayed collapsed or expanded
     *
     * @param array $group group data
     * @return bool true if the group should be displayed collapsed, false otherwise
     */
    function _isCollapsedGroup($group)
    {
      return !(array_key_exists('collapsed', $group) && $group['collapsed'] == false);
    }

    /**
     * Renders a group of rows
     *
     * @param array $group data of the group
     * @param int $rowOffset how many real rows there were before
     * @param int $headerWidth number of columns the header has to span
     * @return string html of the header and group
     * @access protected
     */
    function _renderRowGroup($group, $rowOffset, $headerWidth)
    {
      $style = '';
      if ($this->_isCollapsedGroup($group))
      {
        $style = 'style="display: none"';
      }

      $output  = $this->_renderRowGroupHeader($group, $headerWidth);
      $output .= "<tbody id=\"{$group['id']}\" class=\"grouped\" {$style}>";
      $rowCount = count($group['rows']);

      $row = $rowOffset;
      for ($i=0;$i<$rowCount;$i++)
      {
        $row++;
        $rowData = $group['rows'][$i];

        $output .= $this->_renderRow($rowData, $this->m_currentRow);
        $this->m_currentRow++;
      }

      $output .= '</tbody>';

      return $output;
    }


    /**
     * Gets the maximum number of columns in a row.
     *
     * @param aray $data table data
     * @return int max number of rows
     * @access protected
     */
    function _getMaxCols($data)
    {
      $max = 0;

      for($i=0, $_i=count($data); $i<$_i; $i++)
      {
      	if (is_array($data[$i]) && $this->_isGroup($data[$i]))
        {
          // check the group's rows
          for($j=0, $_j=count($data[$i]['rows']); $j<$_j; $j++)
          {
            $max = max(count($data[$i]['rows'][$j]), $max);
          }
        }
        else
        {
          $max = max(count($data[$i]), $max);
        }
      }
      return $max;
    }

    /**
     * Overloading extension of parent::getAlignment() Adds special rule for
     * groupheaders.
     *
     * @param string|int $row rownumber or name
     * @param int $col column number
     * @return int alignment flag
     * @access protected
     */
    function getAlignment($row, $col)
    {
      if ($row == 'groupheader')
      {
        if ($col == 0)
        {
          return TBL_LEFT;
        }
        else {
          return TBL_RIGHT;
        }
      }
      else
      {
        return parent::getAlignment($row, $col);
      }
    }
  }