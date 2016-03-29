<div class="form-horizontal">
    {foreach from=$fields item=field}
        <div class="form-group">
            {if $field.label!=""}{$field.label}{/if}
            {$field.full}
        </div>
    {/foreach}
</div>

