{$formstart}
<table border="0" cellspacing="0" cellpadding="2" width="100%">
  {if $header!=""}
  <tr>
    <td valign="top" align="center">{$header}<br><br></td>
  </tr>
  {/if}
  {if $index!=""}
  <tr>
    <td valign="top" align="center">{$index}<br><br></td>
  </tr>
  {/if}
  {if $navbar!=""}
  <tr>
    <td valign="top" align="center">{$navbar}<br></td>
  </tr>
  {/if}
  <tr>
    <td valign="top" align="center">{$list}<br></td>
  </tr>
  {if $navbar!=""}
  <tr>
    <td valign="top" align="center">{$navbar}<br></td>
  </tr>
  {/if}
  {if $footer!=""}
  <tr>
    <td valign="top" align="center">{$footer}<br><br></td>
  </tr>
  {/if}
</table>
{$formend}