<!DOCTYPE html>
<html>
{if isset($head)}
    <head>
        <title>{$title}</title>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
        <meta name="atkversion" content="{$atkversion}"/>
        {$head}
    </head>
{/if}

{if isset($body)}
    <body class="hold-transition {if $extra_classes} {$extra_classes} {/if} "{if $extrabodyprops} {$extrabodyprops}{/if}>
        {if !login_page }<div class="wrapper">{/if}

            {$body}
            <div id="hiddenvars" style="display: none">{if isset($hiddenvars)}{$hiddenvars}{/if}</div>

        {if !login_page }</div>{/if}
    </body>
{/if}

</html>
