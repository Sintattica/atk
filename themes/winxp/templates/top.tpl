<table border="0" cellpadding="6" cellspacing="0" bgcolor="#FFFFFF" width="100%" align="center" valign="top" height="75">
  <tr>
    <td>
      {atkconfig var="theme_logo" smartyvar="theme_logo"}
      {if $theme_logo != ''}<img src="{$theme_logo}" alt='' align=left>{else}Update your configuration to put your logo here{/if}
    </td>
    <td align="center" class="block" >
      <table width="100%" bgcolor="#FFFFFF">
	<tr>
	  <td align="left" width="20%"></td>
	  <td align="center" width="60%">
            <br />{$logintext}:
	    <b>{$user}</b> &nbsp; <a href="{$logoutlink}" target="{$logouttarget}">{$logouttext}</a> &nbsp;
			    {if $centerpiece}{$centerpiece}{/if}</td>
          <td align="right" width="20%" nowrap>{if $searchpiece}{$searchpiece}</span>{/if}</td>
        </tr>
      </table><br/>
    </td>
  </tr>
</table>
