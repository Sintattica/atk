{literal}
<style type="text/css">
body
{
{/literal}
	padding: 24px;
	background: #fff url({$themedir}images/bodyPattern.gif) repeat left top;
{literal}
}
</style>
{/literal}

<div id='loginform' style="background: #EBEBEB url({$themedir}images/logoGrijs.png) no-repeat 40px 20px;">
<form action="{$formurl}" method="post">
  <div id="loginform-title">{atktext login_form}</div>
  <div id="loginform-content">
  {if isset($auth_max_loginattempts_exceeded)}
    {$auth_max_loginattempts_exceeded}
  {else}
    {$atksessionformvars}
    {if isset($auth_mismatch)}{$auth_mismatch}<br>{/if}
    {if isset($auth_account_locked)}{$auth_account_locked}<br>{/if}
    <table cellpadding="0" cellspacing="0" border="0"><tr>
    <td class="loginformLabel">{atktext username}:</td><td class="loginformField">{$userfield}</td>
    </tr><tr>
    <td class="loginformLabel">{atktext password}:</td><td class="loginformField"><input class="loginform" type="password" size="15" name="auth_pw" value=""></td>
    </tr><tr>
    <td class="loginformLabel"></td><td>
    <input name="login" class="button" type="submit" value="{atktext login}">
    {if $auth_enablepasswordmailer}<input name="login" class="button" type="submit" value="{atktext password_forgotten}">{/if}
    </td>
    </tr></table>
  {/if}
  </div>
</form>
</div>
