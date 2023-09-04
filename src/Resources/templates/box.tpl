<section class="content overflow-hidden">

    <div class="card card-outline card-secondary">
        <div class="card-header d-flex justify-content-between">

            {* left section of header *}
            <div class="card-header-left">
                <h3 class="card-title">{$title}</h3>
            </div>

            {* right section of header *}
            <div class='card-header-right ml-auto'>
                {if $link_edit} {* TODO: refactor editLink *}
                    <a class="btn btn-sm btn-primary" href='{$link_edit}'
                       style="border-radius: 50%; padding: .2rem .36rem; font-size: 0.7rem !important;"
                       data-toggle="tooltip" data-placement="top" title="Edit">
                        <i class="fa-solid fa-pencil"></i>
                    </a>
                {/if}

                {if $nodeHelp}
                    <a href="" class="ml-2" data-toggle="modal" data-target="#helpModal">
                        <i class="fa-regular fa-circle-question" data-toggle="tooltip" data-placement="top"
                           title="Help"></i>
                    </a>
                {/if}

                {if $bookmarkLink}
                    {$bookmarkLink}
                {/if}
            </div>

        </div>

        <div class="card-body">
            {if $legend}
                <div class="card-body-legend mb-2">
                    {$legend}
                </div>
            {/if}

            {if $filterInputs}
                <div class="card-body-filter-inputs mb-2">
                    {$filterInputs}
                </div>
            {/if}

            {if $filterButtons}
                <div class="card-body-filter-buttons mb-2">
                    {$filterButtons}
                </div>
            {/if}

            {if $boxActions}
                <div class="card-body-box-actions mb-1">
                    {$boxActions}
                </div>
            {/if}

            {$content}
        </div>
    </div>
</section>

<!-- Help Modal -->
<div class="modal fade" id="helpModal" tabindex="-1" role="dialog" aria-labelledby="helpModalLabel"
     aria-hidden="true">
    <div class="modal-dialog" role="document">
        <div class="modal-content">
            <div class="modal-header">
                <h5 class="modal-title" id="helpModalLabel">Help</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                    <span aria-hidden="true">&times;</span>
                </button>
            </div>
            <div class="modal-body">
                {$nodeHelp}
            </div>
        </div>
    </div>
</div>
