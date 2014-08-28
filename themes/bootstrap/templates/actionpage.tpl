<div class="container">
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
                {section name=i loop=$stacktrace}
                    {if %i.index%>=%i.loop%-4}
                        {if %i.last%}
                            <li class="active">{$stacktrace[i].title}</li>
                        {else}
                            <li><a href="{$stacktrace[i].url|atk_htmlentities}">{$stacktrace[i].title}</a></li>
                        {/if}
                    {else}
                        {if %i.index% == 0}...{/if}
                    {/if}
                {/section}
            </div>
        {/if}
        {foreach from=$blocks item=block}
            {$block}
        {/foreach}
    </div>
</div>