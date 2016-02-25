{atkconfig var="login_logo" smartyvar="login_logo"}

<div class="container">
    <div class="panel panel-default">
        <div class="panel-heading">
            <h3 class="panel-title">{atktext login_form}</h3>
        </div>
        <div class="panel-body">
            <div class="row">
                <div class="col-sm-6">
                    <img src="{$login_logo}" class="center-block img-responsive">
                </div>
                <div class="col-sm-6">
                    {if isset($auth_max_loginattempts_exceeded)}
                        {$auth_max_loginattempts_exceeded}
                    {else}
                        {if isset($auth_mismatch)}<div class="alert alert-danger">{$auth_mismatch}</div>{/if}
                        {if isset($auth_account_locked)}<div class="alert alert-danger">{$auth_account_locked}</div>{/if}
                        <form action="{$formurl}" method="post" role="form" class="login-form">
                            {$atksessionformvars}
                            <div class="form-group">
                                <label for="auth_user">{atktext username}</label>
                                {$userfield}
                            </div>
                            <div class="form-group">
                                <label for="auth_pw">{atktext password}</label>
                                <input class="form-control" size="20" type="password" id="auth_pw" name="auth_pw" value="">
                            </div>
                            <button type="submit" name="login" class="btn btn-primary center-block" value="{atktext login}">{atktext login}</button>
                            {if $auth_enablepasswordmailer}<input name="login" class="btn btn-default" type="submit" value="{atktext password_forgotten}">{/if}
                        </form>
                    {/if}
                </div>
            </div>
        </div>
    </div>
</div>