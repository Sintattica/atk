{atkconfig var="login_logo" smartyvar="login_logo"}
{atkconfig var="auth_ignorepasswordmatch" smartyvar="auth_ignorepasswordmatch"}

<style>
    .container-fluid{
        margin-top:50px;
    }
</style>
<div class="login-box text-sm" style="margin-top:400px;">
    <!-- /.login-logo -->
    <div class="card card-outline card-primary">
        <div class="card-header text-center">
            <a href="#" class="h1"><img src="{$login_logo}" class="center-block img-responsive login-logo" alt="logo"></a>
        </div>
        <div class="card-body">
            <p class="login-box-msg">{atktext id="login_form_title"}</p>

            {if isset($auth_max_loginattempts_exceeded)}
                <div class="alert alert-danger"><p>{$auth_max_loginattempts_exceeded}</p></div>
            {else}

                {if isset($error)}
                    <div class="alert alert-danger"><p>{$error}</p></div>
                {/if}

                <form action="{$formurl}" method="post">

                    {$atksessionformvars}

                    <div class="input-group mb-3">
                        <input type="text" class="form-control" placeholder="{atktext id="username"}" size="20"
                               id="auth_user" name="auth_user"
                               value="{$defaultname}">
                        <div class="input-group-append">
                            <div class="input-group-text">
                                <span class="fas fa-user"></span>
                            </div>
                        </div>
                    </div>

                    {if !$auth_ignorepasswordmatch}
                        <div class="input-group mb-3">
                            <input type="password" class="form-control" placeholder="{atktext id="password"}"
                                   id="auth_pw"
                                   name="auth_pw" value="">
                            <div class="input-group-append">
                                <div class="input-group-text">
                                    <span class="fas fa-lock"></span>
                                </div>
                            </div>
                        </div>
                    {/if}

                    <div class="row">
                        <div class="col-8 my-auto">

                            {if isset($auth_enable_rememberme)}
                                <div class="icheck-primary"
                                     style="display: flex; flex-direction: row; align-items: center;">
                                    <input type="checkbox" id="auth_rememberme" name="auth_rememberme" value="1"
                                           {if isset($auth_rememberme)}checked{/if}>
                                    <label for="auth_rememberme" style="padding-left:5px; margin-top: 5px;">
                                        {atktext id="auth_rememberme"}
                                    </label>
                                </div>
                            {/if}
                        </div>
                        <!-- /.col -->
                        <div class="col-4 my-auto">
                            <button type="submit" class="btn btn-primary btn-block"
                                    value="{atktext id="login"}">{atktext id="login"}</button>
                        </div>
                        <!-- /.col -->
                    </div>
                </form>
            {/if}
        </div>
        <!-- /.card-body -->
    </div>
    <!-- /.card -->
</div>
