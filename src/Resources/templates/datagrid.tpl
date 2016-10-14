{atkconfig var="datagrid_display_top_paginator" smartyvar="displayTopPaginator"}
{atkconfig var="datagrid_display_bottom_paginator" smartyvar="displayBottomPaginator"}


{if !empty($top)}
    <div class="row datagrid-top">
        <div class="col-sm-12">
            {$top}
        </div>
    </div>
{/if}
{if !empty($index) || !empty($editcontrol)}
    <div class="row datagrid-editcontrol">
        <div class="col-sm-12">
            {if !empty($editcontrol)}{$editcontrol}{/if} {if !empty($index)}{$index}{/if}
        </div>
    </div>
{elseif !empty($paginator) || !empty($limit) || !empty($summary)}
    {if $displayTopPaginator}
    <div class="row">
        <div class="col-sm-12 datagrid-paginator-navigation top">
        {if !empty($editcontrol)}<div>{$editcontrol}</div>{/if}
        {if !empty($paginator)}<div>{$paginator}</div>{/if}
        {if !empty($summary)}<div>{$summary}</div>{/if}
        {if !empty($limit)}<div>{$limit}</div>{/if}
        </div>
    </div>
    {/if}
{/if}
{if !empty($list)}
    <div class="row datagrid-list">
        <div class="col-sm-12">
            {$list}
        </div>
    </div>
{/if}
{if !empty($norecordsfound)}
    <div class="row datagrid-norecordsfound">
        <div class="col-sm-12">
            <i>{$norecordsfound}</i>
        </div>
    </div>
{/if}
{if !empty($paginator) || !empty($limit) || !empty($summary)}
    {if $displayBottomPaginator}
    <div class="row">
        <div class="col-sm-12 datagrid-paginator-navigation bottom">
        {if !empty($paginator)}<div>{$paginator}</div>{/if}
        {if !empty($summary)}<div>{$summary}</div>{/if}
        {if !empty($limit)}<div>{$limit}</div>{/if}
        </div>
    </div>
    {/if}
{/if}
{if !empty($bottom)}
    <div class="row datagrid-bottom">
        <div class="col-sm-12">
            {$bottom}
        </div>
    </div>
{/if}