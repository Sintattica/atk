<div id="top">
  <img src="{atkthemeimg logo.jpg}" alt="Logo Achievo" />
  <div id="topLinks">
    <span id="top-center">{foreach  from=$centerpiece_links item=link}{$link}&nbsp;&nbsp;|&nbsp;&nbsp;{/foreach}</span>
    <span id="top-logout"><a href="index.php?atklogout=1" target="{$logouttarget}">{atktext logout}</a></span>
    {if $searchpiece}
    &nbsp;&nbsp;|&nbsp;&nbsp;{atktext search}&nbsp;
    <span id="top-search">{$searchpiece}</span>
    {/if}
  </div>
  <div id="lastviewedBox">
  {crmlastviewed}
  </div>
  <div id="loginBox">
  {atktext logged_in_as}: {$user} {if $username}[{$username}]{/if}
  </div>
</div>
