<nav aria-label="table navigation">
    <ul class="pagination">
        {foreach from=$links item='link' key='i'}
            {if isset($link.current) && $link.current}
                <li class="page-item active"><a  class="page-link" href="#">{$link.title}</a></li>
            {else}
                <li class="page-item">
                        <a class="page-link" href="javascript:void(0)" onclick="{$link.call|escape}" title="{$link.title|escape}">
                                {if $link.type === 'next' and $iconize_links}
                                        <span aria-hidden="true" class="fas fa-angle-right" style="font-size: 0.8em;"></span>
                                {elseif $link.type === 'previous' and $iconize_links}
                                                <span aria-hidden="true" class="fas fa-angle-left" style="font-size: 0.8em;"></span>
                                {elseif $link.type === 'last' and $iconize_links}
                                        <span aria-hidden="true" class="fas fa-angle-double-right" style="font-size: 0.8em;"></span>
                                {elseif $link.type === 'first' and $iconize_links}
                                        <span aria-hidden="true" class="fas fa-angle-double-left" style="font-size: 0.8em;"></span>
                                {else}
                                        {$link.title|escape}

                                {/if}
                        </a></li>
            {/if}
        {/foreach}
    </ul>
</nav>
