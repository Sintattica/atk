{atkmessages}
{if isset($helplink)}
    <div id="action-helplink">
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
<div id="action-content">
    {$content}
</div>
{$formstart}
<div id="action-buttons">
    <div class="overlay-wrapper">
        <div class="overlay"><i class="fas fa-3x fa-sync-alt fa-spin"></i>
            <div class="text-bold pt-2">Loading...</div>
        </div>
        <div class="action-buttons-buttons">
            {foreach from=$buttons item=button}
                &nbsp;{$button}&nbsp;
            {/foreach}
        </div>
    </div>
</div>
{$formend}
