<table border="0" cellspacing="0" cellpadding="2">
{if isset($top)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$top}
  </td>
</tr>
{/if}
{if isset($index) || isset($editcontrol)}
  <tr>
    <td align="left" valign="top">
      {if isset($editcontrol)}{$editcontrol}{/if} {if isset($index)}{$index}{/if}
    </td>
  </tr>
{elseif isset($paginator) || isset($limit)}
  <tr>
    <td align="left" valign="middle">
      {if isset($editcontrol)}{$editcontrol}{/if} {if isset($paginator)}{$paginator}{/if}
    </td>
    <td align="right" valign="middle">
      {if isset($limit)}{$limit}{/if}
    </td>
  </tr>
{/if}
{if isset($list)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$list}
  </td>
</tr>
{/if}
{if isset($norecordsfound)}
  <tr>
    <td align="left" valign="top">
      <i>{$norecordsfound}</i>
    </td>
  </tr>
{/if}
{if isset( $paginator) || isset($summary)}
  <tr>
    <td align="left" valign="middle">
      {if isset($paginator)}{$paginator}{/if}
    </td>
    <td align="right" valign="middle">
      {if isset($summary)}{$summary}{/if}
    </td>
  </tr>
{/if}
{if isset($bottom)}
<tr>
  <td align="left" valign="top" colspan="2">
    {$bottom}
  </td>
</tr>
{/if}
</table>