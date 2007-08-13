<?php
atkimport("atk.utils.atktablerenderer");

/**
 * 
 * Renders a grouped table, use the following data format as argument to the
 * render method.
 * 
 * Format:
 * 
 * <code>
 * $data = array(
 *  array('headercol1', 'headercol2'), // cannot be grouped, normal row
 *  array(...), // normal row
 *  array(...), // normal row
 *  array(
 *    'type' => 'group',      // defaults to 'row', so must set this... 
 *    'id'  => 'groupname1', 
 *    'collapsed' => true,    // optional, default is true
 *    'header' => array(...), // the header row to display
 *    'rows' => array(
 *      array(...),
 *      array(...),
 *      array(...),
 *    )
 *  ),
 *  array('footercol1', 'footercol2'), // cannot be grouped, normal row
 * )
 * </code>
 */
class atkTableRendererGrouped extends atkTableRenderer
{  
  var $m_currentRow = 0; // used in counting rows
  
  /**
   * Constructor
   *
   * @param int $flags flags to set on this object (see setFlag)
   * @param string $style stylesheet file (basename) to use like: 'recordlist'.
   * @param string $module module name to register style with
   * @return atkTableRendererGrouped
   */
  function atkTableRendererGrouped($flags = 0, $style = '', $module = '')
  {
    $this->atkTableRenderer($flags, $style, $module);
    $this->setClass('groupheader', 0, 'firstcol');
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
    $output = '';
    $tbodyOpened = false;
    $max = $this->_getMaxCols($data);
    
    $entryCount = count($data);
    $this->m_currentRow = $rowOffset;
    for ($entry=$rowOffset; $entry<$entryCount; $entry++)
    {
      $entryData = $data[$entry];
      
      // render a group or a single row
      if ($this->_isGroup($entryData))
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
     */
    function _getMaxCols($data)
    {
      $max = 0;
      for($i=0, $_i=count($data); $i<$_i; $i++)
      {
        if ($this->_isGroup($data[$i]))
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