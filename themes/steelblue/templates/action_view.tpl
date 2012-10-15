{atkmessages}
{if isset($helplink)}
	<div id="action-helplink" style="border: 0px solid red">
  	{$helplink}<br />
	</div>
{/if}
{if count($atkmessages)}
<div class="atkmessages">
  {foreach from=$atkmessages item=message}
    <div class="atkmessages_{$message.type}">{$message.message}</div>
  {/foreach}
</div>
<br />
{/if}
{$header}
<div id="action-content" style="border: 0px solid green;">
{$content}
</div>
<br>
{$formstart}
<div id="action-buttons">
      {foreach from=$buttons item=button}
        &nbsp;{$button}&nbsp;
      {/foreach}
</div>
{$formend}
