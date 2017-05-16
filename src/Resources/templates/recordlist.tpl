{atkconfig var="recordlist_onclick" smartyvar="recordlist_onclick"}
{if isset($formstart)}{$formstart}{/if}

<div class="recordListContainer">
    {if $mra!="" && $mraposition == 'top'}
        <!-- multirecord actions -->
        <div class="multirecordactions multirecordactions-top"><i class="fa fa-long-arrow-down fa-2x" aria-hidden="true"></i>
            {$mra}</div>
    {/if}
    <table id="{$listid}" class="table table-bordered table-condensed recordList" style="width: 1%;">
        <!-- header -->
        <tr>
            {section name=headerloop loop=$header}
                <th {if isset($header[headerloop].htmlattributes)}{$header[headerloop].htmlattributes}{/if}
                        {if $smarty.section.headerloop.index===0}class="recordListThFirst"{else}class="recordListTh"{/if}>
                    {if $header[headerloop].content != ""}{$header[headerloop].content}{else}&nbsp;{/if}
                </th>
            {/section}
        </tr>

        {if count($sort)}
            <!-- sort row -->
            <tr class="recordList-sort-row">
                {$sortstart}
                {foreach from=$sort item=col}
                    <th {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                        {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                    </th>
                {/foreach}
                {$sortend}
            </tr>
        {/if}

        {if count($search)}
            <!-- search row -->
            <tr class="recordList-search-row">
                {$searchstart}
                {foreach from=$search item=col}
                    <th class="recordListSearch" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                        {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                    </th>
                {/foreach}
                {$searchend}
            </tr>
        {/if}

        <!-- records -->
        {$liststart}
        {foreach from=$rows item=row}
            <tr id="{$row.id}" class="{$row.class}"
                {if $row.background!=""}style="background-color:{$row.background}" {/if}
                {if $recordlist_onclick}
                    onmouseover="highlightrow(this, '{$row.highlight}')"
                    onmouseout="resetrow(this)"
                    onclick="selectrow(this, '{$listid}', {$row.rownum})"
                {/if}
                    >
                {section name=colloop loop=$row.cols}
                    <{if $row.type == "subtotal"}th{else}td{/if}
                            class="{if $smarty.section.colloop.index===0}recordListTdFirst{else}recordListTd{/if}{if $row.cols[colloop].type == "data" && $recordlist_onclick} clickable{/if} row-type-{$row.cols[colloop].type}"
                            {if isset($row.cols[colloop].htmlattributes)}{$row.cols[colloop].htmlattributes}{/if}
                            {if $row.cols[colloop].type == "data" && $recordlist_onclick} onclick="rl_try('{$listid}', event, {$row.rownum}, ['select', 'edit', 'view'
                    ], false);"{/if}>
                        {if $row.cols[colloop].content != ""}{$row.cols[colloop].content}{else}&nbsp;{/if}
                    </{if $row.type == "subtotal"}th{else}td{/if}>
                {/section}
            </tr>
        {/foreach}
        {$listend}

        {if count($total)}
            <!-- totals row -->
            <tr class="recordList-totals-row">
                {foreach from=$total item=col}
                    <th {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                        {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                    </th>
                {/foreach}
            </tr>
        {/if}
    </table>

    {if $mra!="" && $mraposition == 'bottom'}
        <!-- multirecord actions -->
        <div class="multirecordactions multirecordactions-bottom"><i class="fa fa-long-arrow-up fa-2x" aria-hidden="true"></i> {$mra}</div>
    {/if}
</div>

{if isset($formend)}{$formend}{/if}
