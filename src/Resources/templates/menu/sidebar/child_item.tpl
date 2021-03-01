<li class="nav-item {$active}">
    <a href="{$link}" {$attributes} class="nav-link {$classes} {$active} {if !$link} disabled {/if}">
        {include file='menu/sidebar/icon.tpl'}
        <p>{$title}</p>
    </a>
</li>
