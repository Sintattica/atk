{atkconfig var="theme_logo" smartyvar="theme_logo"}
<div class="navbar navbar-default navbar-fixed-top" role="navigation">
    <div class="container-fluid">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target=".navbar-collapse">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            {if empty($theme_logo)}
                <a class="navbar-brand" href="./">
                    {$app_title}
                </a>
            {else}
                <a class="navbar-brand has-logo" href="./">
                    <img border="0" src="{$theme_logo}" alt="Logo"/>
                </a>
            {/if}
        </div>
        <div class="navbar-collapse collapse">
            {$menu}
            <ul class="nav navbar-nav navbar-right">
                {if $user}
                    <li id="top-logout">
                        <a href="index.php?atklogout=1" target="{$logouttarget}">{$user}&nbsp;&nbsp;<span class="glyphicon glyphicon-log-out"></span></a>
                    </li>{/if}
                {if $searchpiece}
                    <li id="top-search">{$searchpiece}</li>
                {/if}
            </ul>
        </div>
        <!--/.nav-collapse -->
    </div>
</div>