<script language="JavaScript" type="text/javascript">
var tabs = new Array();
{section name=i loop=$tabs}tabs[tabs.length] = "{$tabs[i].tab}"; {/section}

var tabLeftImage = "{$themedir}images/tab_left_inactive.gif";
var tabRightImage = "{$themedir}images/tab_right_inactive.gif";
var tabBackgroundImage = "{$themedir}images/tab_back_inactive.gif";
var tabSelectedLeftImage = "{$themedir}images/tab_left_active.gif";
var tabSelectedRightImage = "{$themedir}images/tab_right_active.gif";
var tabSelectedBackgroundImage = "{$themedir}images/tab_back_active.gif";
var tabOverLeftImage = '{$themedir}images/tab_left_over.gif';
var tabOverBackgroundImage = '{$themedir}images/tab_back_over.gif';
var tabOverRightImage = '{$themedir}images/tab_right_over.gif';

var tabColor = "#FFFFFF";
var tabSelectedColor = "#FFFFFF";

{literal}
function mouseOverImage(el, left, right)
{
  el.style.backgroundImage = 'url(' + tabOverBackgroundImage + ')';
  document.getElementById(left).src = tabOverLeftImage;
  document.getElementById(right).src = tabOverRightImage;
}

function mouseOutImage(el, left, right)
{
  el.style.backgroundImage = 'url(' + tabBackgroundImage + ')';
  document.getElementById(left).src = tabLeftImage;
  document.getElementById(right).src = tabRightImage;
}

{/literal}

</script>

<table border="0" cellpadding="0" cellspacing="0" width="98%" align="center" valign="top">
  <tr>
    <td width="100%" bgcolor="#FFFFFF">
      <table border="0" cellpadding="0" cellspacing="0">
        <tr>
          {section name=i loop=$tabs}

          <td valign="bottom">
          <div style="position: relative;">
            <div id="tab_{$tabs[i].tab}" style="position: absolute;">
            <table border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td><img  src="{$themedir}images/tab_left_active.gif"></td>
                  <td onclick="showTab('{$tabs[i].tab}')" height="19" valign="middle" background="{$themedir}images/tab_back_active.gif" align="center" nowrap>
                    {$tabs[i].title}
                  </td>
                  <td><img src="{$themedir}images/tab_right_active.gif"></td>
                </tr>
              </table>
            </div>
            <table border="0" cellspacing="0" cellpadding="0" style="cursor: pointer;">
              <tr>
                <td><img id="left{$smarty.section.i.index}" src="{$themedir}images/tab_left_inactive.gif"></td>
                <td onclick="showTab('{$tabs[i].tab}')" onmouseover="mouseOverImage(this, 'left{$smarty.section.i.index}', 'right{$smarty.section.i.index}');" onmouseout="mouseOutImage(this, 'left{$smarty.section.i.index}', 'right{$smarty.section.i.index}');" height="19" valign="middle" background="{$themedir}images/tab_back_inactive.gif" align="center" nowrap>
                    {$tabs[i].title}
                </td>
                <td><img id="right{$smarty.section.i.index}" src="{$themedir}images/tab_right_inactive.gif"></td>
              </tr>
            </table>
          </div>
          </td>
          {/section}
          <td valign="bottom" width="100%">
            <table cellspacing="0" cellpadding="0" border="0" width="100%">
              <tr><td bgcolor="#919B9C" height="1"><img src="{$themedir}images/blank.gif" height="1" width="1"></td></tr>
            </table>
          </td>
        </tr>
      </table>
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td bgcolor="#919B9C" width="1"><img src="{$themedir}images/blank.gif" width="1"></td>
          <td bgcolor="#FFFFFF" align="left" class="block">
            <table border="0" cellspacing="5" cellpadding="5" width="100%">
              <tr>
                <td>
                  {$content}
                </td>
              </tr>
            </table>
          </td>
          <td bgcolor="#919B9C" width="1"><img src="{$themedir}images/blank.gif" width="1"></td>
        </tr>
        <tr height="1">
          <td bgcolor="#919B9C" colspan="3" valign="top"><img src="{$themedir}images/blank.gif" height="1" width="1"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>

<script language="JavaScript" type="text/javascript">
showTab();
</script>