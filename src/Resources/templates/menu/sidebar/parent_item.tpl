<li class="nav-item">
    <a href="#" class="nav-link">
        {include file='menu/sidebar/icon.tpl'}
        <p>
            {$title}
            <i class="fas fa-angle-left right"></i>
            <span class="badge {if $badge_status}badge-{$badge_status} {/if}right">{$badge_text}</span>
        </p>
    </a>
    <ul class="nav nav-treeview">{$submenu|unescape:'html'}</ul>
</li>
