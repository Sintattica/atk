{$formstart}
<table border="0" cellspacing="0" cellpadding="2" width="100%">
  <tr>
    <td valign="top" align="center">{$content}<br></td>
  </tr>
  <tr>
    <td align="center" valign="top">
      {foreach from=$buttons item=button}
        &nbsp;{$button}&nbsp;        
      {/foreach}<br><br></td>
  </tr>
</table>
{$formend}