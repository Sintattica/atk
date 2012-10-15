<script language="JavaScript" src="{$jscode}" type="text/javascript"></script>

<div id="menubody">
  {php}echo text("logged_in_as","","atk");{/php}: <b>{$name}</b>
    &nbsp; {if $error}{$error}{/if}
</div>