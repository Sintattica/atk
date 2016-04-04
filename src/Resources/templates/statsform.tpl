<div class="stats-form form-inline">
    {foreach from=$fields item=field}
        <div class="form-group {$field.class}" id="{$field.rowid}">
            <div class="control-label"><label>{if $field.label!=""}{$field.label}{/if}</label></div>
            <div id="{$field.id}">{$field.full}</div>
        </div>
    {/foreach}
</div>