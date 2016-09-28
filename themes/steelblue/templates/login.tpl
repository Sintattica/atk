{atkconfig var="theme_login_logo" smartyvar="login_logo"}
{if !$login_logo}{atkconfig var="theme_logo" smartyvar="login_logo"}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg id='login_logo.jpg'}{/capture}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg id='logo.jpg'}{/capture}{/if}
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


<div class="LoginHeader"><img src="{atkthemeimg id='contentheaderright.jpg'}" align="right"></div>
<div id='loginform'>
    <div id='logologin'><img src="{$login_logo}" alt="Logo"></div>
    <form action="{$formurl}" method="post">
        <div id="loginform-title">{atktext id='login_form'}</div>
        <div id="loginform-content">
            {if isset($auth_max_loginattempts_exceeded)}
                {$auth_max_loginattempts_exceeded}
            {else}
                {$atksessionformvars}
                <table cellpadding="0" cellspacing="0" border="0">
                    <tr>
                        <td class="loginformLabel">{atktext id='username'}:</td>
                        <td class="loginformField">{$userfield}</td>
                    </tr>
                    <tr>
                        <td class="loginformLabel">{atktext id='password'}:</td>
                        <td class="loginformField"><input class="loginform" type="password" size="15" name="auth_pw" value=""></td>
                    </tr>
                    {if isset($auth_enable_rememberme)}
                    <tr>
                        <td class="loginformLabel">&nbsp;</td>
                        <td class="loginformField">
                            <label for="auth_rememberme">
                                <input type="checkbox" id="auth_rememberme" name="auth_rememberme" value="1" {if isset($auth_rememberme)}checked{/if}>
                                {atktext id="auth_rememberme"}
                            </label>
                        </td>
                    </tr>
                    {/if}
                    <tr>
                        <td class="loginformLabel"></td>
                        <td>
                            <input name="login" class="button atkdefaultbutton" type="submit" value="{atktext id='login'}">
                            {if $auth_enablepasswordmailer}<input name="login" class="button" type="submit" value="{atktext id='password_forgotten'}">{/if}
                        </td>
                    </tr>
                </table>
                {if isset($auth_mismatch)}<br><span class="error">{$auth_mismatch}</span><br>{/if}
                {if isset($auth_account_locked)}<b/><span class="error">{$auth_account_locked}</span><br>{/if}
            {/if}
        </div>
    </form>
</div>
<div class="LoginFooter"><img src="{atkthemeimg id='contentfooterright.jpg'}" align="right"></div>
