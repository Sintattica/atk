{atkscript file="atk/javascript/prototype/prototype.js" prefix=$atkroot}
{atkscript file="atk/javascript/rico/rico.js" prefix=$atkroot}
{atkscript file="javascript/login.js" prefix=$themedir}
{atkloadscript code="loginonload();\n"}
<div id="loginform">
  <form action="{$formurl}" method="post">
    <div id="loginform-title">{atktext login_form}</div>
    <div id="loginform-content">
    {if isset($auth_max_loginattempts_exceeded)}
      {$auth_max_loginattempts_exceeded}
    {else}
      {if isset($auth_mismatch)}{$auth_mismatch}<br />{/if}
      {if isset($auth_account_locked)}{$auth_account_locked}<br />{/if}
    
      <div id="loginform-username">
        <span id="loginform-username-text">{atktext username}:</span>
        <span id="loginform-username-input"><input id="auth_user" type="text" size="15" name="auth_user" value="" /></span>
      </div>
      <div id="loginform-password">
        <span id="loginform-password-text">
        {atktext password}:
        </span>
        <span id="loginform-password-input">
          <input class="loginform" id="password" type="password" size="15" name="auth_pw" value="" />
        </span>
      </div>
      <div id="loginform-submit" style="display: none;">
        <a id="loginform-submit-link" href="javascript:document.forms[0].submit();">
        {atktext login}
        </a>
      </div>
      {if $auth_enablepasswordmailer}
      <div id="loginform-passwordforgotten">
        <input name="login" class="button" type="submit" value="{atktext password_forgotten}" />
      </div>
      {/if}
    {/if}
    </div>
  </form>
</div>