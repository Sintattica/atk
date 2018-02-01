<div id="{$panename}_editform" style="width:100%;">
    {if isset($errors) && $errors}
        <div class="error">
            {$errortitle}
            {foreach from=$errors item=error}
                <br>
                {$error.label}: {$error.message} {if isset($error.tablink)} ({atktext id="error_tab"} {$error.tablink}){/if}
            {/foreach}
        </div>
    {/if}

    {foreach from=$fields item=field}
        <div {if $field.rowid != ""} id="{$field.rowid}"{/if}{if !$field.initial_on_tab} style="display: none"{/if} class="row form-group {$field.class}">
            {if $field.label!=="AF_NO_LABEL"}
                <label for="{$field.htmlid}" class="col-sm-3 col-md-2 control-label{if isset($field.error)} errorlabel{/if}">
                    {if $field.label!=""}
                        {$field.label} {if isset($field.obligatory)}{$field.obligatory}{/if}
                    {/if}
                </label>
            {/if}
            <div class="{if $field.label!=="AF_NO_LABEL"}col-sm-9 col-md-10{else}col-md-12{/if}" id="{$field.id}">{$field.full}</div>
        </div>
    {/foreach}
</div>
