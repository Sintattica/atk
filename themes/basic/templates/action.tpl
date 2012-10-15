{$formstart}
  {if isset($helplink)}<div id="atkhelp">{$helplink}</div>{/if}
  {atkmessages}
  {if count($atkmessages)}
  <div class="atkmessages">
    {foreach from=$atkmessages item=message}
      <div class="atkmessages_{$message.type}">{$message.message}</div>
    {/foreach}
  </div>
  {/if}
  <div id="atkcontent">
    {$content}
  </div>
  <div id="atkbuttons">
    {foreach from=$buttons item=button}
      <span class="atkbutton">{$button}</span>
    {/foreach}
  </div>
{$formend}
