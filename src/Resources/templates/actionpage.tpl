<div class="wrapper">
    {atkmessages}
    {if count($atkmessages)}
        <div class="atkmessages">
            {foreach from=$atkmessages item=message}
                <div class="atkmessages_{$message.type}">{$message.message}</div>
            {/foreach}
        </div>
    {/if}
    <div class="content-wrapper">
        {include file='menu/breadcrumb.tpl'}

        {foreach from=$blocks item=block}
            {$block}
        {/foreach}
    </div>

    {$footer}
</div>
