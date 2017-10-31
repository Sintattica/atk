{atkconfig var="recordlist_onclick" smartyvar="recordlist_onclick"}
{atkconfig var="recordlist_top_scroller" smartyvar="recordlist_top_scroller"}
{if isset($formstart)}{$formstart}{/if}

{if $recordlist_top_scroller == true}
    <div class="recordListScroller" style="height:30px;line-height:0;margin:0;padding:0;display:none;overflow-x:auto;overflow-y:hidden;">
        <div class="scroller" style="height:30px;line-height:0;margin:0;padding:0;"></div>
    </div>
{/if}

<div class="recordListContainer">
    {if $mra!="" && $mraposition == 'top'}
        <!-- multirecord actions -->
        <div class="multirecordactions multirecordactions-top"><i class="fa fa-long-arrow-down fa-2x" aria-hidden="true"></i>
            {$mra}</div>
    {/if}
    <table id="{$listid}" class="table table-bordered table-condensed recordList">
        <!-- header -->
        <tr class="recordList-header-row">
            {section name=headerloop loop=$header}
                <th {if $smarty.section.headerloop.index===0}class="recordListThFirst"{else}class="recordListTh"{/if}>
                    {if $header[headerloop].content != ""}{$header[headerloop].content}{else}&nbsp;{/if}
                </th>
            {/section}
        </tr>

        {if count($sort)}
            <!-- sort row -->
            <tr class="recordList-sort-row">
                {$sortstart}
                {foreach from=$sort item=col}
                    <th>{if $col.content != ""}{$col.content}{else}&nbsp;{/if}</th>
                {/foreach}
                {$sortend}
            </tr>
        {/if}

        {if count($search)}
            <!-- search row -->
            <tr class="recordList-search-row">
                {$searchstart}
                {foreach from=$search item=col}
                    <th class="recordListSearch">{if $col.content != ""}{$col.content}{else}&nbsp;{/if}</th>
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
                    onmouseover="ATK.RL.highlightRow(this, '{$row.highlight}')"
                    onmouseout="ATK.RL.resetRow(this)"
                    onclick="ATK.RL.selectRow(this, '{$listid}', {$row.rownum})"
                {/if}
                    >
                {section name=colloop loop=$row.cols}
                    <{if $row.type == "subtotal"}th{else}td{/if}
                            class="{if $smarty.section.colloop.index===0}recordListTdFirst{else}recordListTd{/if}{if $row.cols[colloop].type == "data" && $recordlist_onclick} clickable{/if} row-type-{$row.cols[colloop].type}"
                            {if $row.cols[colloop].type == "data" && $recordlist_onclick} onclick="ATK.RL.rl_try('{$listid}', event, {$row.rownum}, ['select', 'edit', 'view'
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
                    <th>{if $col.content != ""}{$col.content}{else}&nbsp;{/if}</th>
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
