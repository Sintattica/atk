<div id="atklogin">
  <h1>{$title}</h1>
  {if $auth_max_loginattempts_exceeded}
    <span class="atkloginerror" id="atkloginattemptsexceeded">{$auth_max_loginattempts_exceeded}</span>
  {else}
    {if $auth_account_locked}
      <span class="atkloginerror">{$auth_account_locked}</span>
    {elseif $auth_mismatch}
      <span class="atkloginerror">{$auth_mismatch}</span>
    {/if}
    <form action="{$formurl}" method="post">
    {$atksessionformvars}

    <div id="atkuserid"><span class="atklabel">{atktext username}: </span>{$userfield}</div>
    <div id="atkpassword"><span class="atklabel">{atktext password}: </span>{$passwordfield}</div>
    <span class="atkbuttons">
      {$submitbutton} {if $forgotpasswordbutton!=""}{$forgotpasswordbutton}{/if}
    </span>
    </form>
  {/if}
</div>