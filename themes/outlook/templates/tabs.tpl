<table border="0" cellpadding="0" cellspacing="0" bgcolor="#606060" width="98%" align="center" valign="top">
  <tr>
    <td width="100%" bgcolor="#EEEEE0">
      <table border="0" cellpadding="0" cellspacing="0">
        <tr>
          {section name=i loop=$tabs}
          
          <td valign="bottom">
            <table border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td><img src="{$themedir}images/tab_left{if $tabs[i].selected}_s{/if}.gif"></td>
                <td height="22" valign="middle" background="{$themedir}images/tab_back{if $tabs[i].selected}_s{/if}.gif" align="center" nowrap>
                  <span class="{if $tabs[i].selected}tab_selected{else}tab{/if}">
                  {if $tabs[i].selected}
                    {$tabs[i].title}
                  {else}
                    <a href="{$tabs[i].link}" style="color:#ffffff;text-decoration:none">{$tabs[i].title}</a>
                  {/if}
                    </span>
                </td>
                <td><img src="{$themedir}images/tab_right{if $tabs[i].selected}_s{/if}.gif"></td>
              </tr>
            </table>
          </td>
         
          {/section}
          <td valign="bottom" width="100%">
            <table cellspacing="0" cellpadding="0" border="0" width="100%">
              <tr><td bgcolor="#00309c" height="2"><img src="{$themedir}images/blank.gif" height="2" width="2"></td></tr>
            </table>
          </td>
        </tr>
      </table>
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td bgcolor="#00309c" width="2"><img src="{$themedir}images/blank.gif" width="2"></td>
          <td bgcolor="#EEEEE0" align="left" class="block">
            <table border="0" cellspacing="5" cellpadding="5">
              <tr>
                <td>
                  {$content}
                </td>
              </tr>
            </table>
          </td>
          <td bgcolor="#00309c" width="2"><img src="{$themedir}images/blank.gif" width="2"></td>
        </tr>
        <tr>
          <td bgcolor="#00309c" colspan="3" valign="top"><img src="{$themedir}images/blank.gif" height="2" width="1"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>