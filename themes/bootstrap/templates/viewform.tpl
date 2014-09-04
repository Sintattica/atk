<div class="viewform">
    {foreach from=$fields item=field}
        <div{if $field.rowid != ""} id="{$field.rowid}"{/if}{if !$field.initial_on_tab} style="display: none"{/if}
                class="row section-item {$field.class}">

            {if isset($field.line)}
                <div class="col-md-12 field">{$field.line}</div>
            {else}
                {if $field.label!=="AF_NO_LABEL"}
                    <div class="col-md-2 fieldlabel">{if $field.label!=""}{$field.label}{/if}</div>
                    <div class="col-md-10 field">{$field.full}</div>
                {else}
                    <div class="col-md-12 field">{$field.full}</div>
                {/if}
            {/if}
        </div>
    {/foreach}
</div>
