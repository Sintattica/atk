  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage javascript
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
function highlightrow(row, color)
{
  if (typeof(row.style) != 'undefined') 
  {
    row.oldcolor = row.style.backgroundColor;    
    row.style.backgroundColor = color;
  }
}

function resetrow(row)
{  
  row.style.backgroundColor = row.oldcolor;
}

function selectrow(row, rlId, rownum)
{
  table = document.getElementById(rlId);  
  if (table.listener.setRow(rownum, row.oldcolor))
  {
    row.oldcolor = row.style.backgroundColor;    
  }
}

function rl_do(rlId, rownum, action, confirmtext)
{
  extra="";
  if (confirmtext)
  {
    confirmed = confirm(confirmtext);
    if (confirmed) extra = "&confirm=1";
  }
  if (rl_a[rlId][rownum][action] && (!confirmtext || confirmed))
  {
    if (!rl_a[rlId]['embed'])
    {
      document.location.href = rl_a[rlId][rownum][action]+'&'+rl_a[rlId]['base']+extra;
    }
    else
    {
      atkSubmit(rl_a[rlId][rownum][action]+'&'+rl_a[rlId]['base']+extra);
    }
  }
}

function rl_next(rlId)
{
  if (rl_a[rlId]['next'])
  {
    document.location.href = rl_a[rlId]['next'];
  }
  return false
}

function rl_previous(rlId)
{
  if (rl_a[rlId]['previous'])
  {
    document.location.href = rl_a[rlId]['previous'];
    return true;
  }
  return false;
}

rl_a = new Array();

