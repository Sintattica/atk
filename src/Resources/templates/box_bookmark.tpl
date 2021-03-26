<section class="content overflow-hidden">

    <div class="card">
        <div class="card-header">
            <h3 class="card-title">{$title}</h3>
            {if $link_edit}
                <a class="float-right" href='{$link_edit}'><i class="fas fa-pencil-alt"></i></a>
            {/if}
        </div>
        <div class="card-body">
            {$content}
        </div>

    </div>
</section>
