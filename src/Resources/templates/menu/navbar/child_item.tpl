<li class="nav-item">
    {if $link}
        <a class="{$classes}" href="{$link}" {$attributes}>
            {if $icon}
                {include file='menu/sidebar/icon.tpl'}
            {/if}

            {if !$hide_name}
                <span>{$title}</span>
            {/if}
        </a>
    {else}
        <div class="{$classes}" {$attributes}>{$title}</div>
    {/if}
</li>
