<div id="top">
    <img src="{atkthemeimg id='logo.jpg'}" alt="Logo Achievo" />
    <div id="topLinks">
        <span id="top-center">{foreach  from=$centerpiece_links item=link}{$link}&nbsp;&nbsp;|&nbsp;&nbsp;{/foreach}</span>
        <span id="top-logout"><a href="index.php?atklogout=1" target="{$logouttarget}">{atktext id='logout'}</a></span>
            {if $searchpiece}
            &nbsp;&nbsp;|&nbsp;&nbsp;{atktext id='search'}&nbsp;
            <span id="top-search">{$searchpiece}</span>
        {/if}
    </div>
    <div id="loginBox">
        {atktext id='logged_in_as'}: {$user}
    </div>
</div>
