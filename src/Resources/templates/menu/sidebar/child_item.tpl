<li class="nav-item {$nav_classes}">
    <a href="{$link}" {$attributes} class="nav-link {$classes} {$active} {if !$link} disabled {/if}">
        {include file='menu/sidebar/icon.tpl'}
        <p>
            {$title}
            <span class="badge {if $badge_status}badge-{$badge_status} {/if}right">{$badge_text}</span>
        </p>
    </a>
</li>
