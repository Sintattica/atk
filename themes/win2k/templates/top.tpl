<table border="0" cellpadding="6" cellspacing="0" bgcolor="#FFFFFF" width="100%" align="center" valign="top">
  <tr>
    <td>
      <img src="{$themedir}images/logo.gif" alt='' align=left>
    </td>
    <td align="center" class="block" >
      <table width="100%" bgcolor="#FFFFFF">
	<tr>
	  <td align="left" width="20%"><a href="http://localhost/achievo" target="{$logouttarget}">Home</a></td>
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
