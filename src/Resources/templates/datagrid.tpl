{if !empty($index) || !empty($editcontrol)}
    <div class="row no-gutters datagrid-editcontrol">
        {if !empty($editcontrol)}{$editcontrol}{/if} {if !empty($index)}{$index}{/if}
    </div>
{elseif $displayTopInfo && (!empty($paginator) || !empty($limit) || !empty($summary))}
    <div class="row no-gutters top mb-2">
        <div class="col-12 col-sm-6 text-center text-sm-left">
            {if !empty($editcontrol)}
                <div>{$editcontrol}</div>{/if}

            {if !empty($summary)}
                <div class="d-inline-block mr-3">{$summary}</div>
            {/if}

            {if !empty($limit)}
                <div class="d-inline-block">{$limit}</div>
            {/if}
        </div>
        <div class="col-12 col-sm-6 text-center text-sm-right mt-3 mt-sm-0">
            {if !empty($paginator)}
                <div class="d-inline-block">{$paginator}</div>{/if}
        </div>
    </div>

{/if}
{if !empty($list)}
    <div class="row mt-1 no-gutters datagrid-list">
        {$list}
    </div>
{/if}
{if !empty($norecordsfound)}
    <div class="row mt-1 no-gutters datagrid-norecordsfound">
        <i>{$norecordsfound}</i>
    </div>
{/if}
{if $displayBottomInfo && (!empty($paginator) || !empty($limit) || !empty($summary))}
    <div class="row mt-1 no-gutters bottom">
        <div class="col-12 col-sm-6 text-center text-sm-left">
            {if !empty($summary)}
                <div class="d-inline-block mr-3">{$summary}</div>
            {/if}

            {if !empty($limit)}
                <div class="d-inline-block">{$limit}</div>
            {/if}
        </div>
        <div class="col-12 col-sm-6 text-center text-sm-right mt-3 mt-sm-0">
            {if !empty($paginator)}
                <div class="d-inline-block">{$paginator}</div>{/if}
        </div>
    </div>
{/if}
