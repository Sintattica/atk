<div{if $field.rowid != ""} id="{$field.rowid}"{/if}{if $field.initial_on_tab!='yes'} style="display: none"{/if}
        class="section-item form-group {$field.tab}">
    {if isset($field.line) && $field.line!=""}
        {$field.line}
    {else}
        {if $field.label!=="AF_NO_LABEL"}
            <label for="{$field.attribute}"
                   class="col-md-2 control-label {if isset($field.error)}errorlabel{else}fieldlabel{/if}">
                {if $field.label!=""}{$field.label}{/if}
                {if isset($field.obligatory)}{$field.obligatory}{/if}
            </label>
        {/if}
        <div class="{if $field.label!=="AF_NO_LABEL"}col-md-10{else}col-md-12{/if}">{$field.full}</div>
    {/if}
</div>