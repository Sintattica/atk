<script language="JavaScript" type="text/javascript">
    var tabs = [];
    {section name=i loop=$tabs}
    tabs[tabs.length] = "{$tabs[i].tab}";
    {/section}
</script>


<ul class="nav nav-tabs">
    {section name=i loop=$tabs}
        <li id="tab_{$tabs[i].tab}" class="nav-item mr-2">
            <a class="nav-link {if $tabs[i].selected}active activetab{else}passivetab{/if}" href="javascript:void(0)" onclick="ATK.Tabs.showTab('{$tabs[i].tab}')">{$tabs[i].title}</a>
        </li>
    {/section}
</ul>


<div class="tab-content mt-4">{$content}</div>



