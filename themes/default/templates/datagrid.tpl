<table border="0" cellspacing="0" cellpadding="2" width="100%">
{if $index}
  <tr>
    <td align="left" valign="top">
      {$index}
    </td
  </tr>
{elseif $paginator || $limit}
  <tr>
    <td align="left" valign="middle">
      {if $paginator}{$paginator}{/if}
    </td
    <td align="right" valign="middle">
      {if $limit}{$limit}{/if}
    </td>
  </tr>
{/if}
<tr>
  <td align="left" valign="top" colspan="2">
    {$list}
  </td
</tr>
{if $norecordsfound}
  <tr>
    <td align="left" valign="top">
      <i>{$norecordsfound}</i>
    </td>
  </tr>
{/if}
{if $paginator || $summary}
  <tr>
    <td align="left" valign="middle">
      {if $paginator}{$paginator}{/if}
    </td
    <td align="right" valign="middle">
      {if $summary}{$summary}{/if}
    </td>
  </tr>
{/if}
</table>