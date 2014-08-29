{if isset($formstart)}{$formstart}{/if}
<div>
    {atkmessages}
    {if count($atkmessages)}
        <div class="row">
            <div class="col-md-12">
                <div class="atkmessages">
                    {foreach from=$atkmessages item=message}
                        <div class="atkmessages_{$message.type}">{$message.message}</div>
                    {/foreach}
                </div>
            </div>
        </div>
    {/if}
    {if (isset($header) && !empty($header))}
        <div class="row">
            <div class="col-md-12">{$header}</div>
        </div>
    {/if}
    {if (isset($index) && !empty($index))}
        <div class="row">
            <div class="col-md-12">{$index}</div>
        </div>
    {/if}
    {if (isset($navbar) && !empty($navbar))}
        <div class="row">
            <div class="col-md-12">{$navbar}</div>
        </div>
    {/if}
    <div class="row">
        <div class="col-md-12">{$list}</div>
    </div>
    {if (isset($navbar) && !empty($navbar))}
        <div class="row">
            <div class="col-md-12">{$navbar}</div>
        </div>
    {/if}
    {if (isset($footer) && !empty($footer))}
        <div class="row">
            <div class="col-md-12">{$footer}</div>
        </div>
    {/if}
</div>
{if isset($formstart)}{$formend}{/if}