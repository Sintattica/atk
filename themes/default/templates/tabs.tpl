<table border="0" cellpadding="0" cellspacing="0" width="98%" align="center" valign="top">
  <tr>
    <td width="100%">
      <table border="1" cellpadding="2" cellspacing="0">
        <tr>                              
          {section name=i loop=$tabs}
          <td valign="middle" align="left" nowrap>
          {if $tabs[i].selected}
            <b>&nbsp;{$tabs[i].title}&nbsp;</b>
          {else}
            &nbsp;<a href="{$tabs[i].link}" style="color:black;text-decoration:none">{$tabs[i].title}</a>&nbsp;
          {/if}
          </td>          
          {/section}
        </tr>
      </table>
      <table border="1" cellspacing="0" cellpadding="5" width="100%">
        <tr>
          <td>
            {$content}
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>