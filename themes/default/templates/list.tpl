{$formstart}
<table border="0" cellspacing="0" cellpadding="2" width="100%">
  {if $header!=""}
  <tr>
    <td valign="top" align="left">{$header}<br><br></td>
  </tr>
  {/if}
  {if $index!=""}
  <tr>
    <td valign="top" align="left">{$index}<br><br></td>
  </tr>
  {/if}
  {if $navbar!=""}
  <tr>
    <td valign="top" align="left">{$navbar}<br></td>
  </tr>
  {/if}
  <tr>
    <td valign="top" align="left">{$list}<br></td>
  </tr>
  {if $navbar!=""}
  <tr>
    <td valign="top" align="left">{$navbar}<br></td>
  </tr>
  {/if}
  {if $footer!=""}
  <tr>
    <td valign="top" align="left">{$footer}<br><br></td>
  </tr>
  {/if}
</table>
{$formend}