<script language="JavaScript" type="text/javascript">
var tabs = new Array();
{section name=i loop=$tabs}
  tabs[tabs.length] = "{$tabs[i].tab}";
{/section}
</script>

<table border="0" cellpadding="0" cellspacing="0" valign="top">
  <tr>
    <td width="100%" align="left">
      <br />
	    <table border="0" cellpadding="0" cellspacing="0" class="tabsTabs mainTabs">
        <tr>                              
          {section name=i loop=$tabs}
            <td id="tab_{$tabs[i].tab}" valign="middle" align="left" nowrap="nowrap" class="{if $tabs[i].selected}activetab{else}passivetab{/if}">	
              <a href="javascript:void(0)" onclick="showTab('{$tabs[i].tab}')">{$tabs[i].title}</a>
            </td>          
            <td>&nbsp;</td>
		      {/section}
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