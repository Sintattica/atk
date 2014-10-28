{atkconfig var="theme_panel_class" smartyvar="panel_class"}
<div class="panel panel-default {$panel_class}">
    <div class="panel-heading">
        <h3 class="panel-title">{$title}</h3>
        <div style="visibility: hidden" id="atkbusy"><i class="fa fa-cog fa-spin fa-2x"></i></div>
    </div>
    <div class="panel-body">
        {$content}
    </div>
</div>