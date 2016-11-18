{atkconfig var="login_logo" smartyvar="login_logo"}
{atkconfig var="auth_ignorepasswordmatch" smartyvar="auth_ignorepasswordmatch"}

<div class="container">
    <div class="form-signin">
        <div class="panel panel-default">
            <div class="panel-heading">
                <h3 class="panel-title">{atktext id="login_form"}</h3>
            </div>
            <div class="panel-body">
                <img src="{$login_logo}" class="center-block img-responsive login-logo">

                {if isset($auth_max_loginattempts_exceeded)}
                    <div class="alert alert-danger"><p>{$auth_max_loginattempts_exceeded}</p></div>
                {else}

                    {if isset($error)}<div class="alert alert-danger"><p>{$error}</p></div>{/if}

                    <form action="{$formurl}" method="post" role="form" class="login-form">
                        {$atksessionformvars}

                        <div class="form-group">
                            <label for="auth_user">{atktext id="username"}</label>
                            <input class="form-control loginform" type="text" size="20" id="auth_user" name="auth_user" value="{$defaultname}" />
                        </div>

                        {if !$auth_ignorepasswordmatch}
                            <div class="form-group">
                                <label for="auth_pw">{atktext id="password"}</label>
                                <input class="form-control" size="20" type="password" id="auth_pw" name="auth_pw" value="">
                            </div>
                        {/if}

                        {if isset($auth_enable_rememberme)}
                            <div class="form-group">
                                <label for="auth_rememberme">
                                    <input type="checkbox" id="auth_rememberme" name="auth_rememberme" value="1" {if isset($auth_rememberme)}checked{/if}>
                                    {atktext id="auth_rememberme"}
                                </label>
                            </div>
                        {/if}
                        <button type="submit" name="login" class="btn btn-primary center-block"
                                value="{atktext id="login"}">{atktext id="login"}</button>
                        {if $auth_enablepasswordmailer}<input name="login" class="btn btn-default" type="submit"
                                                              value="{atktext id="password_forgotten"}">{/if}
                    </form>
                {/if}
            </div>
        </div>
    </div>
</div>
