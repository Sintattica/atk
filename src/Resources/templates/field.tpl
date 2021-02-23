<div{if $field.rowid != ""} id="{$field.rowid}"{/if}{if $field.initial_on_tab!='yes'} style="display: none"{/if}
        class="row section-item form-group {$field.tab} {if isset($field.obligatory)}required{/if} {if isset($field.error)}has-error{/if}">
    {if isset($field.line) && $field.line!=""}
        {$field.line}
    {else}

        {if $field.label!=="AF_NO_LABEL"}
            <label for="{$field.htmlid}" class="col-sm-3 col-md-2 control-label">
                {if $field.label!=""}{$field.label}{/if}
            </label>
        {/if}
        <div class="{if $field.label!=="AF_NO_LABEL"}col-sm-9 col-md-10{else}col-sm-12{/if}" id="{$field.id}">
            {$field.full}
        </div>
    {/if}
</div>
