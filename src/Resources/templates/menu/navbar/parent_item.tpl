<li class="nav-item dropdown">
    <a id="dropdownSubMenu1" href="#" data-toggle="dropdown"
       aria-haspopup="true" aria-expanded="false"
       class="nav-link dropdown-toggle {$classes}"
    >
        {if $icon}
            {include file='menu/sidebar/icon.tpl'}
        {/if}

        {if !$hide_name}
            <span>{$title}</span>
        {/if}
    </a>
    <ul aria-labelledby="dropdownSubMenu1" class="dropdown-menu border-0 shadow">{$submenu}</ul>
</li>
