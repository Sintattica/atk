<div id="atktop">
<h1>{$title}</h1>
<span id="atktopleft">
  <span id="atkloggedinas">{$logintext}: </span>
  <span id="atkloggedinuser">{$user}</span>
  <a id="atklogoutlink" href="{$logoutlink}" target="{$logouttarget}">{$logouttext}</a>
</span>
{if $centerpiece!=""}
  <span id="atktopcenter">{$centerpiece}</span>
{/if}
{if $searchpiece!=""}
  <span id="atktopsearch">{$searchpiece}</span>
{/if}
</div>