<div class="navbar navbar-default navbar-fixed-top" role="navigation">
    <div class="container-fluid">
        <div class="navbar-header">
            <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target=".navbar-collapse">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            <a class="navbar-brand" href="./">{$app_title}</a>
        </div>
        <div class="navbar-collapse collapse">
            {$menu}
            <ul class="nav navbar-nav navbar-right">
                {if $user}
                    <li id="top-logout">
                    <a href="index.php?atklogout=1" target="{$logouttarget}">{atktext logout} {$user}</a>
                    </li>{/if}
                {if $searchpiece}
                    <li id="top-search">{$searchpiece}</li>
                {/if}
            </ul>
        </div>
        <!--/.nav-collapse -->
    </div>
</div>