<div class="controls form-inline dglimit">
    {if $label}<label>{$label}:</label>{/if}
    <select data-no-search onchange="{$call|escape}" class="form-control form-control-sm" style="width: 70px;">
        {foreach from=$options item=option}
            <option value="{$option.value|escape}"{if $option.current} selected="selected"{/if}>{$option.title|escape}</option>
        {/foreach}
    </select>

    <script>
        jQuery(document).ready(function () {
            ATK.Tools.enableSelect2ForSelect('select[data-no-search]');
        });
    </script>
</div>
