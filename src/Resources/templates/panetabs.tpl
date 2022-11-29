<div id="{$paneName}" class="tabbedPane">
    <input type="hidden" name="{$fieldName}" value="">
    <ul class="nav nav-tabs">
        {foreach from=$tabs key=tabName item=tab}
            <li class="{$tabName} tabbedPaneTab nav-item mr-2">
                <a class="nav-link {if $tab.selected}active activetab{else}passivetab{/if}" href="javascript:void(0)"
                   onclick="ATK.TabbedPane.showTab('{$paneName}', '{$tabName}');return false;">{$tab.title}</a>
            </li>
        {/foreach}
    </ul>
    <div class="tabsContent mt-4">{$content}</div>
</div>
