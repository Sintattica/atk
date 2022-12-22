<li class="nav-item">
    {if $link}
        <a class="{$classes}" href="{$link}" {$attributes} {if $tooltip} data-toggle="tooltip" data-placement="{$tooltip_placement}" title="{$tooltip}" {/if}>
            {if $icon}
                {include file='menu/sidebar/icon.tpl'}
            {/if}

            {if !$hide_name}
                <span>{$title}</span>
            {/if}

            {if $badge_text}
                <span class="badge {if $badge_status}badge-{$badge_status} {/if}navbar-badge">{$badge_text}</span>
            {/if}
        </a>
    {else}
        <div class="{$classes}" {$attributes}>{$title}</div>
    {/if}
</li>
