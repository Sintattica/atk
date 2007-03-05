<div id="{$paneName}" class="tabbedPane">
  <table border="0" cellpadding="0" cellspacing="0" align="left" valign="top">
    <tr>
      <td width="100%" align="left">
        <br />
  	    <table border="0" cellpadding="0" cellspacing="0" class="tabsTabs">
          <tr>                              
            {foreach from=$tabs key=tabName item=tab}
              <td class="{$tabName} tabbedPaneTab {if $tab.selected}activetab{else}passivetab{/if}" valign="middle" align="left" nowrap="nowrap">	
                <a href="javascript:void(0)" onclick="ATK.TabbedPane.showTab('{$paneName}', '{$tabName}')">{$tab.title}</a>
              </td>          
              <td>&nbsp;</td>
            {/foreach}
          </tr>
        </table>
        <table border="0" cellspacing="0" cellpadding="5" width="100%" class="tabsContent">
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