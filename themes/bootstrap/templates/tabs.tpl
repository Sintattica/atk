<script language="JavaScript" type="text/javascript">
    var tabs = new Array();
    {section name=i loop=$tabs}
    tabs[tabs.length] = "{$tabs[i].tab}";
    {/section}
</script>


<div class="tabContainer">
    <ul class="nav nav-tabs">
        {section name=i loop=$tabs}
            <li id="tab_{$tabs[i].tab}"
                class="{if $tabs[i].selected}active activetab{/if}">
                <a href="javascript:void(0)" onclick="showTab('{$tabs[i].tab}')">{$tabs[i].title}</a>
            </li>
        {/section}
    </ul>
</div>

<table border="0" cellspacing="0" cellpadding="5" width="100%" class="tabsContent">
    <tr>
        <td>
            {$content}
        </td>
    </tr>
</table>

