<div{if $field.id != ""} id="ar_{$field.id}"{/if}{if !$field.initial_on_tab} style="display: none"{/if}
        class="row section-item form-group {$field.class} {if $field.obligatory}required{/if} {if $field.error}has-error{/if}">
    {if isset($field.line) && $field.line!=""}
        {$field.line}
    {else}

        {if $field.label!=="AF_NO_LABEL"}
            <label for="{$field.id}" class="col-sm-2 control-label">
                {if $field.label!=""}{$field.label}{/if}
            </label>
        {/if}
        <div class="{if $field.label!=="AF_NO_LABEL"}col-sm-10{else}col-sm-12{/if}" id="ac_{$field.id}">
            {$field.html}
        </div>
    {/if}
</div>
