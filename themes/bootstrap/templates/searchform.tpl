{if $saved_criteria.load_criteria}
    <div class="load_criteria">
        {$saved_criteria.label_load_criteria}:
        {$saved_criteria.load_criteria}
        {if $saved_criteria.forget_criteria}
            <a href="{$saved_criteria.forget_criteria}" title="{$saved_criteria.label_forget_criteria}"><img
                        class="recordlist" border="0"
                        src="{atkthemeicon name='delete' type='recordlist'}"/></a>
        {/if}
    </div>
{/if}

<div class="form-horizontal">
    <div class="row form-group">
        <div class="col-sm-2 control-label fieldlabel">
            {$searchmode_title}
        </div>
        <div class="col-sm-10 field">
            {$searchmode_and} &nbsp;&nbsp; {$searchmode_or}
        </div>
    </div>

    <hr/>

    {foreach from=$fields item=field}
        <div class="row form-group">
            {if $field.line!=""}
                <div class="col-md-8 field">{$field.line}</div>
            {else}
                <div class="col-sm-2 control-label fieldlabel">{if $field.label!=""}{$field.label}: {/if}</div>
                <div class="col-sm-6 field">{$field.full}</div>
            {/if}
            <div class="col-sm-4 field">{$field.searchmode}</div>
        </div>
    {/foreach}

    {if $saved_criteria.toggle_save_criteria }
        <hr/>
    {/if}
</div>

{if $saved_criteria.toggle_save_criteria }
    <div class="save_criteria">
        {$saved_criteria.toggle_save_criteria} {$saved_criteria.label_save_criteria} {$saved_criteria.save_criteria}
    </div>
{/if}