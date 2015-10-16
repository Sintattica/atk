<ul class="pagination">
{foreach from=$links item='link' key='i'}
    {if isset($link.current) && $link.current}
        <li class="active"><a href="#">{$link.title}</a></li>
    {else}
        <li><a href="javascript:void(0)" onclick="{$link.call|escape}" title="{$link.title|escape}">{$link.title|escape}</a></li>
    {/if}

{/foreach}
</ul>
