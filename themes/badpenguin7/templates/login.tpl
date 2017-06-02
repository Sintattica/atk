{atkconfig var="theme_login_logo" smartyvar="login_logo"}
{if !$login_logo}{atkconfig var="theme_logo" smartyvar="login_logo"}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg id="login_logo.jpg"}{/capture}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg id="logo.jpg"}{/capture}{/if}
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


<div class="LoginHeader"><img src="{atkthemeimg id="contentheaderright.jpg"}" align="right"></div>
<div id='loginform'>
<form action="{$formurl}" method="post">
<div id='logologin'><img src="{$login_logo}" alt="Logo">
    {if $auth_enablepasswordmailer}<p align="right"><input name="login" class="button" type="button" value="{atktext id="password_forgotten"}">{/if}
</div>

  <div id="loginform-title">{atktext id="login_form"}</div>
  <div id="loginform-content">
  {if isset($auth_max_loginattempts_exceeded)}
    {$auth_max_loginattempts_exceeded}
  {else}
    {$atksessionformvars}
    {if isset($auth_mismatch)}<span class="error">{$auth_mismatch}</span><br>{/if}
    {if isset($auth_account_locked)}<span class="error">{$auth_account_locked}</span><br>{/if}
    <table cellpadding="0" cellspacing="0" border="0"><tr>
    <td class="loginformLabel">{atktext id="username"}:</td><td class="loginformField">{$userfield}</td>
    </tr><tr>
    <td class="loginformLabel">{atktext id="password"}:</td><td class="loginformField"><input class="loginform" type="password" size="15" name="auth_pw" value=""></td>
    </tr><tr>
    <td class="loginformLabel"></td><td>
    <input name="login" class="button atkdefaultbutton" type="submit" value="{atktext id="login"}">
    </td>
    </tr>
    
    
    </table>
  {/if}
  </div>
  
</form>
</div>
<div class="LoginFooter"><img src="{atkthemeimg id="contentfooterright.jpg"}" align="right"></div>
