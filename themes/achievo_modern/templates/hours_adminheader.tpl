<form name="dayview" method="post" action="dispatch.php" style="position: relative;">
{$sessionform}
<div id="changeView">
  <a href="{$yesterdayurl}">{atktext previousday}</a>&nbsp;
  {if $todayurl && $tomorrowurl}
    <a href="{$todayurl}">{atktext id=today node=houradmin}</a>&nbsp;
    <a href="{$tomorrowurl}">{atktext nextday}</a>
  {/if}
  <a href="{$weekviewurl}">{atktext gotoweekview}</a>
</div>

<div class="currentDate">{$currentdate} {$lockicon}</div>
{$userselect}&nbsp;{$datejumper}&nbsp;<input type="submit" value="{atktext "goto"}">

</form>