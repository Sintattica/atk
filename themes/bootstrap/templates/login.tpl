{atkconfig var="theme_login_logo" smartyvar="login_logo"}
{atkconfig var="theme_panel_class" smartyvar="panel_class"}
{if !$login_logo}{atkconfig var="theme_logo" smartyvar="login_logo"}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg id='login_logo.jpg'}{/capture}{/if}
{if !$login_logo}{capture assign="login_logo"}{atkthemeimg id='logo.jpg'}{/capture}{/if}

<div class="container">
    <div class="panel panel-default {$panel_class}">
        <div class="panel-heading">
            <h3 class="panel-title">{atktext id='login_form'}</h3>
        </div>
        <div class="panel-body">
            <div class="row">
                <div class="col-sm-6">
                    {if isset($auth_max_loginattempts_exceeded)}
                        {$auth_max_loginattempts_exceeded}
                    {else}
                        {if isset($auth_mismatch)}<div class="alert alert-danger">{$auth_mismatch}</div>{/if}
                        {if isset($auth_account_locked)}<div class="alert alert-danger">{$auth_account_locked}</div>{/if}
                        <form action="{$formurl}" method="post" role="form">
                            {$atksessionformvars}
                            <div class="form-group">
                                <label for="auth_user">{atktext id='username'}</label>
                                {$userfield}
                            </div>
                            <div class="form-group">
                                <label for="auth_pw">{atktext id='password'}</label>
                                <input class="form-control" type="password" id="auth_pw" name="auth_pw" value="">
                            </div>
                            <button type="submit" name="login" class="btn btn-primary center-block" value="{atktext id='login'}">{atktext id='login'}</button>
                            {if $auth_enablepasswordmailer}<input name="login" class="btn btn-default" type="submit" value="{atktext id='password_forgotten'}">{/if}
                        </form>
                    {/if}
                </div>
                <div class="col-sm-6">
                    <img src="{$login_logo}" alt="Logo" class="center-block">
                </div>
            </div>
        </div>
    </div>
</div>