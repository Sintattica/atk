{$formstart}
<table border="0" cellspacing="0" cellpadding="2" width="100%">
  {if $helplink!=""}<tr><td align="right" class="helplink">{$helplink}</td></tr>{/if}
  <tr>
    <td align="center" valign="top">
      <br>
      <div class="atkmessages">
        {atkmessages}
        {foreach from=$atkmessages item=message}
          &nbsp;{$message}<br>
        {/foreach}
      </div>
    </td>
  </tr>
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
