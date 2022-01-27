<li class="dropdown-submenu dropdown-hover">
    <a id="dropdownSubMenu2" href="#" role="button"
       data-toggle="dropdown" aria-haspopup="true" aria-expanded="false"
       class="dropdown-item dropdown-toggle {$classes}"
    >
        {if $icon}
            {include file='menu/sidebar/icon.tpl'}
        {/if}

        {if !$hide_name}
            <span>{$title}</span>
        {/if}

    </a>
    <ul aria-labelledby="dropdownSubMenu2" class="dropdown-menu border-0 shadow">{$submenu}</ul>
</li>
