<li class="nav-item {$active}">
    <a href="#" class="nav-link">
        {include file='menu/sidebar/icon.tpl'}
        <p>
            {$title}
            <i class="fas fa-angle-left right"></i>
        </p>
    </a>
    <ul class="nav nav-treeview">{$submenu|unescape:'html'}</ul>
</li>
