{$header}
{$formstart}
<div id="action-helplink" style="border: 0px solid red">
{if isset($helplink)}{$helplink}{/if}
</div>
{atkmessages}
{if count($atkmessages)}
<div class="atkmessages">
  {foreach from=$atkmessages item=message}
    <div class="atkmessages_{$message.type}">{$message.message}</div>
  {/foreach}
</div>
{/if}
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
