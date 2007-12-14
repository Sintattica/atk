{atkmessages}
<div id="action-helplink" style="border: 0px solid red">
{if isset($helplink)}{$helplink}<br />{/if}
</div>
{if count($atkmessages)}
<div class="atkmessages">
  {foreach from=$atkmessages item=message}
    <div class="atkmessages_{$message.type}">{$message.message}</div>
  {/foreach}
</div>
<br />
{/if}
{$header}
{$formstart}
<div id="action-content" style="border: 0px solid green;">
{$content}
</div>
<br>
<div id="action-buttons">
      {foreach from=$buttons item=button}
        &nbsp;{$button}&nbsp;
      {/foreach}
</div>
{$formend}
