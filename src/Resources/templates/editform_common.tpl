<div id="editform">
    {if (count($errors)>0)}
        <div class="alert alert-danger error">
                {$errortitle}<br/>
                {foreach from=$errors item=error}
                    {$error.label}: {$error.message} {if $error.tablink} ({atktext id="error_tab"} {$error.tablink}){/if}
                    <br/>
                {/foreach}
        </div>
    {/if}
    {foreach from=$fieldspart item=part}
        {$part}
    {/foreach}
</div>
