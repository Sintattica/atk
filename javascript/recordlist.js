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

function rl_do(rlId, rownum, action)
{
  if (rl_a[rlId][rownum][action])
  {
    if (!rl_a[rlId]['embed'])
    {
      document.location.href = rl_a[rlId][rownum][action]+'&'+rl_a[rlId]['base'];
    }
    else
    {
      atkSubmit(rl_a[rlId][rownum][action]+'&'+rl_a[rlId]['base']);
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

