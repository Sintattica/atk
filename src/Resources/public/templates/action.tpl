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
<div style="border:0px solid red;">{$header}</div>
{$formstart}
<div> <!-- div added to enable nested forms -->
    <div id="action-content" style="border: 0px solid green;">
        {$content}
    </div>
    <div id="action-buttons" style="border: 0px solid blue;">
        <div class="action-buttons-buttons">
            {foreach from=$buttons item=button}
                {$button}
            {/foreach}
        </div>
        <div class="spinner"><i class="fa fa-cog fa-spin fa-2x"></i></div>
    </div>
</div>
{$formend}
