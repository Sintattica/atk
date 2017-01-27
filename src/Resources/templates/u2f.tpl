{atkconfig var="login_logo" smartyvar="login_logo"}

<script language="JavaScript">
    setTimeout(function() {
        var req = {$requests};
        u2f.sign(req, function(data) {
            var form = document.getElementById('u2f_form');
            var u2f_response = document.getElementById('u2f_response');
            u2f_response.value = JSON.stringify(data);
            form.submit();
        });
    }, 1000);
</script>

<div class="container">
    <div class="form-signin">
        <div class="panel panel-default">
            <div class="panel-heading">
                <h3 class="panel-title">{atktext id="u2f_form_title"}</h3>
            </div>
            <div class="panel-body">
                <img src="{$login_logo}" class="center-block img-responsive login-logo">

                {if isset($error)}
                    <div class="alert alert-danger"><p>{$error}</p></div>{/if}

                <div>
                    {atktext id="u2f_form_description"}
                </div>

                <form action="{$formurl}" method="post" role="form" id="u2f_form">
                    <input type="hidden" name="auth_user" id="auth_user" value="{$auth_user}"/>
                    <input type="hidden" name="u2f_response" id="u2f_response"/>
                    <input type="hidden" name="auth_rememberme" value="{$auth_rememberme}">
                </form>

            </div>
        </div>
    </div>
</div>
