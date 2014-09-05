<div{if $field.rowid != ""} id="{$field.rowid}"{/if}{if $field.initial_on_tab!='yes'} style="display: none"{/if}
        class="section-item form-group {$field.tab} {if isset($field.obligatory)}required{/if}">
    {if isset($field.line) && $field.line!=""}
        {$field.line}
    {else}
        {if $field.label!=="AF_NO_LABEL"}
            <label for="{$field.attribute}"
                   class="col-sm-2 control-label {if isset($field.error)}errorlabel{else}fieldlabel{/if}">
                {if $field.label!=""}{$field.label}{/if}
            </label>
        {/if}
        <div class="{if $field.label!=="AF_NO_LABEL"}col-sm-10{else}col-sm-12{/if}">
            {if $field.readonly}<span class="form-control-static">{$field.full}</span>{else}{$field.full}{/if}
           </div>
    {/if}
</div>