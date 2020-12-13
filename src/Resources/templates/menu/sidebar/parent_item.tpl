<li class="nav-item {$active}">
    <a href="#" class="nav-link">
        <i class="{$icon} nav-icon"></i>
        <p>
            {$title}
            <i class="fas fa-angle-left right"></i>
        </p>
    </a>
    <ul class="nav nav-treeview">{$submenu|unescape:'html'}</ul>
</li>
