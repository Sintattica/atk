<style>
    #debugger_wrapper{
        display: none;
    }
</style>

<div class="login-box text-sm" style="margin-top:15%; margin-left: auto; margin-right: auto;">
    <div class="card card-outline card-primary">
        <div class="card-header text-center">
            <h3 class="card-title">{$title|default:"Second Factor Authentication"}</h3>
        </div>
        <div class="card-body">
            <p class="login-box-msg">{$auth_2fa_text|default:"Please enter the second factor authentication code."}</p>

            {if isset($error)}
                <div class="alert alert-danger"><p>{$error}</p></div>
            {/if}

            <form action="{$formurl}" method="post" role="form">
                {$atksessionformvars}

                <input type="hidden" name="auth_user" value="{$auth_user}">
                {if isset($auth_rememberme) && $auth_rememberme}
                    <input type="hidden" name="auth_rememberme" value="1">
                {/if}

                <div class="input-group mb-3">
                    <input class="form-control" type="text" id="auth_2fa_code" name="auth_2fa_code" value="" placeholder="{atktext id="auth_2fa_code"|default:"Authentication Code"}" autofocus>
                    <div class="input-group-append">
                        <div class="input-group-text">
                            <span class="fas fa-key"></span>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-12">
                        <button type="submit" class="btn btn-primary btn-block"
                                value="{atktext id="auth_2fa_submit"|default:"Verify"}">{atktext id="auth_2fa_submit"|default:"Verify"}</button>
                    </div>
                </div>
            </form>
        </div>
    </div>
</div>
