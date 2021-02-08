{atkconfig var="dispatcher" smartyvar="dispatcher"}
<section class="content-header">

    <div class="container-fluid">
        <div class="row mb-2">
            <div class="col-sm-6">
                <h1>{$title}</h1>
            </div>
            <div class="col-sm-6">
                {stacktrace}
                {if count($stacktrace) > 0}
                    <ol class="breadcrumb float-sm-right">
                        <li class="breadcrumb-item"><a href="{$dispatcher}"><span class="fas fa-home"></a></li>
                        {foreach $stacktrace as $item}
                            {if !$item@last}
                                <li class="active breadcrumb-item">
                                    <a href="{$item.url}" data-toggle="tooltip"
                                       data-placement="bottom"
                                       title="{$item.descriptor}">{$item.title}</a>
                                </li>
                            {else}
                                <li class="breadcrumb-item">{$item.title}</li>
                            {/if}
                        {/foreach}
                    </ol>
                    <script type="text/javascript">
                        {literal}
                        // use tooltip only if breadcrumb is visible
                        jQuery(function () {
                            jQuery('.breadcrumb li a[data-toggle="tooltip"]').tooltip()
                        });
                        {/literal}
                    </script>
                {/if}

            </div>
        </div>
    </div>

</section>
