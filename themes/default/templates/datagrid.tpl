<table border="0" cellspacing="0" cellpadding="2">
{if !empty($top)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$top}
  </td>
</tr>
{/if}
{if !empty($index) || !empty($editcontrol)}
  <tr>
    <td align="left" valign="top">
      {if !empty($editcontrol)}{$editcontrol}{/if} {if !empty($index)}{$index}{/if}
    </td>
  </tr>
{elseif !empty($paginator) || !empty($limit)}
  <tr>
    <td align="left" valign="middle">
      {if !empty($editcontrol)}{$editcontrol}{/if} {if !empty($paginator)}{$paginator}{/if}
    </td>
    <td align="right" valign="middle">
      {if !empty($limit)}{$limit}{/if}
    </td>
  </tr>
{/if}
{if !empty($list)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$list}
  </td>
</tr>
{/if}
{if !empty($norecordsfound)}
  <tr>
    <td align="left" valign="top">
      <i>{$norecordsfound}</i>
    </td>
  </tr>
{/if}
{if !empty($paginator) || !empty($summary)}
  <tr>
    <td align="left" valign="middle">
      {if !empty($paginator)}{$paginator}{/if}
    </td>
    <td align="right" valign="middle">
      {if !empty($summary)}{$summary}{/if}
    </td>
  </tr>
{/if}
{if !empty($bottom)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$bottom}
  </td>
</tr>
{/if}
</table>