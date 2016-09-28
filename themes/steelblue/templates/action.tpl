{atkmessages}
{if isset($helplink)}
    <div id="action-helplink" style="border: 0px solid red">
        {$helplink}<br/>
    </div>
{/if}
{if count($atkmessages)}
    <div class="atkmessages">
        {foreach from=$atkmessages item=message}
            <div class="atkmessages_{$message.type}">{$message.message}</div>
        {/foreach}
    </div>
{/if}
{$header}
{$formstart}
<div id="action-content" style="border: 0px solid green;">
    {$content}
</div>
<br/>
<div id="action-buttons">
    <div class="action-buttons-buttons">
        {foreach from=$buttons item=button}
            {$button}
        {/foreach}
    </div>
   {if $buttons}
    <div class="spinner"><img src="{atkthemeimg id='spinner.gif'}" alt="Busy"/></div>
    {/if}
</div>

{$formend}
