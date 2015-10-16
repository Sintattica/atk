<div class="form-horizontal">
    {if $saved_criteria.load_criteria}
        <div class="row form-group load_criteria">
            <div class="col-sm-2 control-label fieldlabel">{$saved_criteria.label_load_criteria}</div>
            <div class="col-sm-10 form-inline">
                {$saved_criteria.load_criteria}
                {if $saved_criteria.forget_criteria}
                    <a href="{$saved_criteria.forget_criteria}" title="{$saved_criteria.label_forget_criteria}"
                       class="btn btn-default">
                        <span class="glyphicon glyphicon-trash"></span>
                    </a>
                {/if}
            </div>
        </div>
    {/if}

    <div class="row form-group">
        <div class="col-sm-2 control-label fieldlabel">{$searchmode_title}</div>
        <div class="col-sm-10">
            <div class="radio">
                <label>{$searchmode_and}</label>
            </div>
            <div class="radio">
                <label>{$searchmode_or}</label>
            </div>
        </div>
    </div>

    <hr/>

    {foreach from=$fields item=field}
        <div class="row form-group">
            {if $field.line!=""}
                <div class="col-md-8 field">{$field.line}</div>
            {else}
                <div class="col-sm-2 control-label fieldlabel">{if $field.label!=""}{$field.label}{/if}</div>
                <div class="col-sm-6 field">{$field.full}</div>
            {/if}
            <div class="col-sm-4 field">{$field.searchmode}</div>
        </div>
    {/foreach}

    {if $saved_criteria.toggle_save_criteria }
        <hr/>
        <div class="row form-group save_criteria">
            <div class="col-sm-2">
                <div class="checkbox control-label">
                    <label>{$saved_criteria.toggle_save_criteria} <span class="fieldlabel">{$saved_criteria.text_save_criteria}</span></label>
                </div>
            </div>
            <div class="col-sm-10 field">{$saved_criteria.save_criteria}</div>
        </div>
    {/if}
</div>

