<script language="JavaScript" type="text/javascript">
var tabs = new Array();
{section name=i loop=$tabs}tabs[tabs.length] = "{$tabs[i].tab}"; {/section}

var tabLeftImage = "{$themedir}images/tab_left.png";
var tabRightImage = "{$themedir}images/tab_right.png";
var tabBackgroundImage = "{$themedir}images/tab_back.png";
var tabSelectedLeftImage = "{$themedir}images/tab_left_s.png";
var tabSelectedRightImage = "{$themedir}images/tab_right_s.png";
var tabSelectedBackgroundImage = "{$themedir}images/tab_back_s.png";

var tabColor = "#FFFFFF";
var tabSelectedColor = "#000000";
</script>

<div class="tabOuterDiv">
<table border="0" cellpadding="0" cellspacing="0" id="tabContainer">
  <tr>
  {section name=i loop=$tabs}
  <td valign="bottom">

  <div class="tabInnerDiv">

  <div id="tab_{$tabs[i].tab}" style="position: absolute;">

    <table border="0" cellspacing="0" cellpadding="0">
      <tr onclick="showTab('{$tabs[i].tab}')">
        <td height="22" valign="middle" align="center" nowrap class="tabOn">
          <span style="color: #ff0000;">{$tabs[i].title}</span>
        </td>
      </tr>
    </table>

  </div>

  <table border="0" cellspacing="0" cellpadding="0" style="cursor: pointer;">
    <tr onclick="showTab('{$tabs[i].tab}')">
      <td height="22" valign="middle" align="center" nowrap class="tabOff">
        {$tabs[i].title}
      </td>
    </tr>
   </table>

   </div>

   </td>

   {/section}

 </tr>
</table>
</div>

<div id="tabContent">
  {$content}
</div>