<script language="JavaScript" type="text/javascript">
var tabs = new Array();
{section name=i loop=$tabs}tabs[tabs.length] = "{$tabs[i].tab}"; {/section}

var tabLeftImage = "{$themedir}images/tab_left.gif";
var tabRightImage = "{$themedir}images/tab_right.gif";
var tabBackgroundImage = "{$themedir}images/tab_back.gif";
var tabSelectedLeftImage = "{$themedir}images/tab_left.gif";
var tabSelectedRightImage = "{$themedir}images/tab_right.gif";
var tabSelectedBackgroundImage = "{$themedir}images/tab_back.gif";

var tabColor = "#FFFFFF";
var tabSelectedColor = "#000000";
</script>

<table border="0" cellpadding="0" cellspacing="0" bgcolor="#606060" width="98%" align="center" valign="top">
  <tr>
    <td width="100%" bgcolor="#efefef">
      <table border="0" cellpadding="0" cellspacing="0">
        <tr>
          {section name=i loop=$tabs}
          <td valign="bottom">
          <div style="position: relative;">
            <div id="tab_{$tabs[i].tab}" style="position: absolute;">
              <table border="0" cellspacing="0" cellpadding="0">
                <tr onclick="showTab('{$tabs[i].tab}')">
                  <td><img src="{$themedir}images/tab_left_s.gif"></td>
                  <td height="22" valign="middle" background="{$themedir}images/tab_back_s.gif" align="center" nowrap>
                    <b>{$tabs[i].title}</b>
                  </td>
                  <td><img src="{$themedir}images/tab_right_s.gif"></td>
                </tr>
              </table>
            </div>
            <table border="0" cellspacing="0" cellpadding="0" style="cursor: pointer;">
              <tr onclick="showTab('{$tabs[i].tab}')">
                <td><img src="{$themedir}images/tab_left.gif"></td>
                <td height="22" valign="middle" background="{$themedir}images/tab_back.gif" align="center" nowrap>
                  <b>{$tabs[i].title}</b>
                </td>
                <td><img src="{$themedir}images/tab_right.gif"></td>
              </tr>
            </table>
          </div>
          </td>
          {/section}
          <td valign="bottom" width="100%">
            <table cellspacing="0" cellpadding="0" border="0" width="100%">
              <tr><td height="2"><img src="{$themedir}images/blank.gif" height="2" width="2"></td></tr>
            </table>
          </td>
        </tr>
      </table>
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td width="1"><img src="{$themedir}images/blank.gif" width="1"></td>
          <td bgcolor="#d2d2d2" align="left" class="block">
            <table border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td>
                  {$content}
                </td>
              </tr>
            </table>
          </td>
          <td width="1"><img src="{$themedir}images/blank.gif" width="1"></td>
        </tr>
        <tr>
          <td colspan="3" valign="top"><img src="{$themedir}images/blank.gif" height="2" width="1"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
