<!DOCTYPE html>
<html>
{if isset($head)}
    <head>
        <title>{$title}</title>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        {$head}
    </head>
{/if}

{if isset($body)}
    <body{if $extrabodyprops} {$extrabodyprops}{/if}>{$body}

    {if $hiddenvars}
        <div id="hiddenvars" style="display: none">{$hiddenvars}</div>
    {/if}
    </body>
{/if}

</html>
