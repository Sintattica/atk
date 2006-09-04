{$formstart}
  {if isset($helplink)}<div id="atkhelp">{$helplink}</div>{/if}
  {atkmessages}
  {if count($atkmessages)}
    <div id="atkmessages">
      {foreach from=$atkmessages item=message}
         <div class="atkmessage">{$message}</div>
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
