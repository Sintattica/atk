<div id="tabbed_{$panename}" class="tabOuterDiv">
<table border="0" cellpadding="0" cellspacing="0">
  <tr>
  {foreach from=$tabs key=tab item=value}
  <td valign="bottom">
  
  <div class="tabInnerDiv">

  <div id="panetab_{$tab}" style="position: absolute;">

    <table border="0" cellspacing="0" cellpadding="0">
      <tr onclick="showpanetab('{$tab}','{$panename}','{$defaulttab}')">
        <td height="22" valign="middle" align="center" nowrap class="tabOn">
        {$value.title}
        </td>
      </tr>
    </table>

  </div>

  <table border="0" cellspacing="0" cellpadding="0" style="cursor: pointer;">
    <tr onclick="showpanetab('{$tab}','{$panename}','{$defaulttab}')">
      <td height="22" valign="middle" align="center" nowrap class="tabOff">
        {$value.title}
      </td>
    </tr>
   </table>

   </div>

   </td>

   {/foreach}

 </tr>
</table>
</div>

<div id="tabbedpane_{$panename}">
  {$content}
</div>