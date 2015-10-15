<div id="{$paneName}" class="tabContainer tabbedPane">
    <table border="0" cellpadding="0" cellspacing="0" align="left" valign="top">
        <tr>
            <td width="100%" align="left">
                <br/>
                <nav border="0" cellpadding="0" cellspacing="0" class="tabsTabs tabContainer navbar navbar-default"
                     role="navigation">
                    <ul class="nav navbar-nav">
                        {foreach from=$tabs key=tabName item=tab}
                            <li class="{$tabName} tabbedPaneTab {if $tab.selected}active activetab{else}passivetab{/if}"
                                valign="middle" align="left" nowrap="nowrap">
                                <a href="javascript:void(0)"
                                   onclick="ATK.TabbedPane.showTab('{$paneName}', '{$tabName}');
                                           return false;">{$tab.title}</a>
                            </li>
                        {/foreach}
                    </ul>
                </nav>
                <table border="0" cellspacing="0" cellpadding="5" width="100%" class="tabsContent navbar-default">
                    <tr>
                        <td>
                            {$content}
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
    </table>
</div>