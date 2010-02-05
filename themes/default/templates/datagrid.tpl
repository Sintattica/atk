<table border="0" cellspacing="0" cellpadding="2">
{if isset($top) && !empty($top)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$top}
  </td>
</tr>
{/if}
{if isset($index) && !empty($index) || isset($editcontrol) && !empty($editcontrol)}
  <tr>
    <td align="left" valign="top">
      {if isset($editcontrol) && !empty($editcontrol)}{$editcontrol}{/if} {if isset($index) && !empty($index)}{$index}{/if}
    </td>
  </tr>
{elseif isset($paginator) && !empty($paginator) || isset($limit) && !empty($limit)}
  <tr>
    <td align="left" valign="middle">
      {if isset($editcontrol) && !empty($editcontrol)}{$editcontrol}{/if} {if isset($paginator) && !empty($paginator)}{$paginator}{/if}
    </td>
    <td align="right" valign="middle">
      {if isset($limit) && !empty($limit)}{$limit}{/if}
    </td>
  </tr>
{/if}
{if isset($list) && !empty($list)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$list}
  </td>
</tr>
{/if}
{if isset($norecordsfound) && !empty($norecordsfound)}
  <tr>
    <td align="left" valign="top">
      <i>{$norecordsfound}</i>
    </td>
  </tr>
{/if}
{if isset($paginator) && !empty($paginator) || isset($summary) && !empty($summary)}
  <tr>
    <td align="left" valign="middle">
      {if isset($paginator) && !empty($paginator)}{$paginator}{/if}
    </td>
    <td align="right" valign="middle">
      {if isset($summary) && !empty($summary)}{$summary}{/if}
    </td>
  </tr>
{/if}
{if isset($bottom) && !empty($bottom)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$bottom}
  </td>
</tr>
{/if}
</table>