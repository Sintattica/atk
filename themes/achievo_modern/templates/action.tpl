{$formstart}
<div id="action-helplink" style="border: 0px solid red">
{if $helplink!=""}{$helplink}{/if}
</div>
  {atkmessages}
  {if count($atkmessages)}
        <div class="atkmessages">
          {foreach from=$atkmessages item=message}
            &nbsp;{$message}<br>
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
