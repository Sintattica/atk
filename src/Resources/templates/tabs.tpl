<script language="JavaScript" type="text/javascript">
    ATK.Tabs.tabs = [
    {section name=i loop=$tabs}
    "{$tabs[i].tab}",
    {/section}
    ];
    ATK.Tabs.tabstateUrl = "{$tabstateUrl}";
</script>


<ul class="nav nav-tabs">
    {section name=i loop=$tabs}
        <li id="tab_{$tabs[i].tab}"
            class="{if $tabs[i].selected}active activetab{else}passivetab{/if}">
            <a href="javascript:void(0)" onclick="ATK.Tabs.showTab('{$tabs[i].tab}')">{$tabs[i].title}</a>
        </li>
    {/section}
</ul>

<div class="tabsContent">{$content}</div>

