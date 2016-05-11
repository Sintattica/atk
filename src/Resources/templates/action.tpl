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
{/if}
{if isset($header)}<div>{$header}</div>{/if}
{$formstart}
<div> <!-- div added to enable nested forms -->
    <div id="action-content">
        {$content}
    </div>
    <div id="action-buttons">
        <div class="action-buttons-buttons">
            {foreach from=$buttons item=button}
                {$button}
            {/foreach}
        </div>
        <div class="spinner"><i class="fa fa-cog fa-spin fa-2x"></i></div>
    </div>
</div>
{$formend}
