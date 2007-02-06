<form action="dispatch.php"{if $targetframe!=""}target="{$targetframe}"{/if}>
<input type="hidden" name="atknodetype" value="search.search">
<input type="hidden" name="atkaction" value="search">
{$session_form}
<input id='top-search-input' name="searchstring" type="text" size="18" value="{$searchstring}">&nbsp;<img id='searchbutton' src="{atkthemeimg search.png}" onclick="document.forms[0].submit()"/>
</form>
