<li class="nav-item">
    {if $link}
        <a class="{$classes}" href="{$link}" {$attributes}>{$title}</a>
    {else}
        <div class="{$classes}" {$attributes}>{$title}</div>
    {/if}
</li>
