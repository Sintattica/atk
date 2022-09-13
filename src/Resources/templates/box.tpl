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
                       style="border-radius: 50%; padding: .2rem .36rem; font-size: 0.7rem !important;">
                        <i class="fa-solid fa-pencil"></i>
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
