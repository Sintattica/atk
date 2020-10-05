<div class="container-fluid">
    {atkmessages}
    {if count($atkmessages)}
        <div class="atkmessages">
            {foreach from=$atkmessages item=message}
                <div class="atkmessages_{$message.type}">{$message.message}</div>
            {/foreach}
        </div>
    {/if}
    <div class="actionpageWrapper">
        {stacktrace}
        {if count($stacktrace) > 1}
            <ol class="breadcrumb">
                {foreach $stacktrace as $item}
                    {if !$item@last}
                        <li class="active"><a href="{$item.url}" data-toggle="tooltip" data-placement="bottom" title="{$item.descriptor|escape}">{$item.title}</a></li>
                    {else}
                        <li>{$item.title}</li>
                    {/if}
                {/foreach}
            </ol>

            <script type="text/javascript">
                {literal}
                    // use tooltip only if breadcrumb is visible
                    jQuery(function () { jQuery('.breadcrumb li a[data-toggle="tooltip"]').tooltip()});
                {/literal}
            </script>
        {/if}
        {foreach from=$blocks item=block}
            {$block}
        {/foreach}
    </div>
</div>
