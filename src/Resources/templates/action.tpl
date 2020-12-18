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
{if isset($header)}
    <div>{$header}</div>{/if}
{$formstart}
<div> <!-- div added to enable nested forms -->
    <div id="action-content">
        {$content}
    </div>
    <div id="action-buttons">

        <div class="overlay-wrapper">
            <div class="overlay"><i class="fas fa-3x fa-sync-alt fa-spin"></i>
                <div class="text-bold pt-2">Loading...</div>
            </div>

            <div class="action-buttons-buttons row justify-content-end">
                {foreach from=$buttons item=button}
                    {$button}
                {/foreach}
            </div>
        </div>
    </div>
</div>
{$formend}
