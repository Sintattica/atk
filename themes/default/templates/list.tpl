{if isset($formstart)}{$formstart}{/if}
<table border="0" cellspacing="0" cellpadding="2" width="100%">
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
  {if (isset($index) && !empty($index))}
  <tr>
    <td valign="top" align="left">{$index}<br><br></td>
  </tr>
  {/if}
  {if (isset($navbar) && !empty($navbar))}
  <tr>
    <td valign="top" align="left">{$navbar}<br></td>
  </tr>
  {/if}
  <tr>
    <td valign="top" align="left">{$list}<br></td>
  </tr>
  {if (isset($navbar) && !empty($navbar))}
  <tr>
    <td valign="top" align="left">{$navbar}<br></td>
  </tr>
  {/if}
  {if (isset($footer) && !empty($footer))}
  <tr>
    <td valign="top" align="left">{$footer}<br></td>
  </tr>
  {/if}
</table>
{if isset($formstart)}{$formend}{/if}