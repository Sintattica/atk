{atkconfig var="theme_login_logo" smartyvar="login_logo"}
{if !$login_logo}{atkconfig var="theme_logo" smartyvar="login_logo"}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg login_logo.jpg}{/capture}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg logo.jpg}{/capture}{/if}
{literal}
<style type="text/css">
body
{
{/literal}
  background: #f3f3f3;
	padding: 50px 24px 24px 24px;
{literal}
}
</style>
{/literal}


<div class="LoginHeader"><img src="{atkthemeimg contentheaderright.jpg}" align="right"></div>
<div id='loginform'>
<div id='logologin'><img src="{$login_logo}" alt="Logo"></div>
<form action="{$formurl}" method="post">
  <div id="loginform-title">{atktext login_form}</div>
  <div id="loginform-content">
  {if isset($auth_max_loginattempts_exceeded)}
    {$auth_max_loginattempts_exceeded}
  {else}
    {$atksessionformvars}
    {if isset($auth_mismatch)}<span class="error">{$auth_mismatch}</span><br>{/if}
    {if isset($auth_account_locked)}<span class="error">{$auth_account_locked}</span><br>{/if}
    <table cellpadding="0" cellspacing="0" border="0"><tr>
    <td class="loginformLabel">{atktext username}:</td><td class="loginformField">{$userfield}</td>
    </tr><tr>
    <td class="loginformLabel">{atktext password}:</td><td class="loginformField"><input class="loginform" type="password" size="15" name="auth_pw" value=""></td>
    </tr><tr>
    <td class="loginformLabel"></td><td>
    <input name="login" class="button atkdefaultbutton" type="submit" value="{atktext login}">
    {if $auth_enablepasswordmailer}<input name="login" class="button" type="submit" value="{atktext password_forgotten}">{/if}
    </td>
    </tr></table>
  {/if}
  </div>
</form>
</div>
<div class="LoginFooter"><img src="{atkthemeimg contentfooterright.jpg}" align="right"></div>
