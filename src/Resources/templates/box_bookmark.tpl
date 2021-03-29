<section class="content overflow-hidden">
    <div class="card">
        <div class="card-header">
            <h3 class="card-title pt-1">{$title}</h3>
            {if $link_edit}
                <a class="float-right btn btn-sm btn-primary" href='{$link_edit}' style="border-radius: 50%; padding: .2rem .36rem; font-size: 0.7rem !important;"><i class="fas fa-pencil-alt"></i></a>
            {/if}
        </div>
        <div class="card-body">
            {$content}
        </div>

    </div>
</section>
