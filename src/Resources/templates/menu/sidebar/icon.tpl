{if $icon_type=='image'}
    <img src="{$icon}" width="100%" height="auto" class="rounded-circle {$icon_classes}"  alt="Icon Item"/>
{elseif $icon_type=='chars'}
    <div class="rounded-circle {$icon_classes}">{$icon}</div>
{else}
    <i class="{$icon} nav-icon {$icon_classes}"></i>
{/if}
