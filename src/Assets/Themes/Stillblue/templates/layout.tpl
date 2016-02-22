<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
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
