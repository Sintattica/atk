<li class="nav-item dropdown">
    <a href="#" data-toggle="dropdown"
       aria-haspopup="true" aria-expanded="false"
       class="parent item nav-link dropdown-toggle {$classes}"
    >
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
    <ul aria-labelledby="dropdownSubMenu1" class="dropdown-menu border-0 shadow">{$submenu}</ul>
</li>
