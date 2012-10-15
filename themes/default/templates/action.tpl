{$formstart}
<table border="0" cellspacing="0" cellpadding="2" width="100%">
  {if isset($helplink)}<tr><td align="right" class="helplink">{$helplink}</td></tr>{/if}
  
  {atkmessages}
  {if count($atkmessages)}
    <tr>
      <td align="center" valign="top">
        <br>  
        <div class="atkmessages">
          {foreach from=$atkmessages item=message}
            <div class="atkmessages_{$message.type}">{$message.message}</div>
          {/foreach}
        </div>
        </div>
      </td
    </tr>        
  {/if}  
  
  {if (isset($header) && !empty($header))}
  <tr>
    <td valign="top" align="left">{$header}<br><br></td>
  </tr>
  {/if}
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
