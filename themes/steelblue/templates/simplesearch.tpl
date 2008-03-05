<form action="dispatch.php" {if $targetframe!=""}target="{$targetframe}"{/if}>
<input type="hidden" name="atknodetype" value="search.search">
<input type="hidden" name="atkaction" value="search">
{$session_form}
<table cellpadding="0" cellspacing="0" height="18">
  <tr>
    <td valign="center"><input id='top-search-input' name="searchstring" type="text" size="18" value="{$searchstring}">&nbsp;</td>
    <td valign="center">&nbsp;<a href="#" onclick="document.forms[0].submit()">{atktext search}</a></td>
  </tr>
</table>
</form>
