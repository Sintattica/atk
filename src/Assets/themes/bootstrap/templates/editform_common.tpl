<div id="editform">
    {if (count($errors)>0)}
        <div class="error">
                {$errortitle}<br/>
                {foreach from=$errors item=error}
                    {$error.label}: {$error.message} {if $error.tablink} ({atktext "error_tab"} {$error.tablink}){/if}
                    <br/>
                {/foreach}
                <br/>
        </div>
    {/if}
    {foreach from=$fieldspart item=part}
        {$part}
    {/foreach}
</div>
