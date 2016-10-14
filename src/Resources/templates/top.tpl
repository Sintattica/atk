{atkconfig var="brand_logo" smartyvar="brand_logo"}
{atkconfig var="dispatcher" smartyvar="dispatcher"}
<div class="navbar navbar-default navbar-fixed-top" role="navigation">
    <div class="container-fluid">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target=".navbar-collapse">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            {if empty($brand_logo)}
                <a class="navbar-brand" href="{$dispatcher}">
                    {$app_title}
                </a>
            {else}
                <a class="navbar-brand has-logo" href="{$dispatcher}">
                    <img border="0" src="{$brand_logo}" alt="{$app_title}"/>
                </a>
            {/if}
        </div>
        <div class="navbar-collapse collapse">
            {$menu}
        </div>
    </div>
</div>
