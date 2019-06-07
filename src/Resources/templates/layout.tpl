<!DOCTYPE html>
<html>
{if isset($head)}
    <head>
        <title>{$title|escape}</title>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
        <meta name="atkversion" content="{$atkversion}" />
        {$head}
    </head>
{/if}

{if isset($body)}
    <body{if $extrabodyprops} {$extrabodyprops}{/if}>{$body}
        <div id="hiddenvars" style="display: none">{if isset($hiddenvars)}{$hiddenvars}{/if}</div>
    </body>
{/if}

</html>
