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
    <br/>
{/if}
{$header}
<div id="action-content" style="border: 0px solid green;">
    {$content}
</div>
{$formstart}
<div id="action-buttons">
    <div class="action-buttons-buttons">
    {foreach from=$buttons item=button}
        &nbsp;{$button}&nbsp;
    {/foreach}
    </div>
    <div class="spinner"><i class="fa fa-cog fa-spin fa-2x"></i></div>
</div>
{$formend}
