<script language="JavaScript" type="text/javascript">
var tabs = new Array();
{section name=i loop=$tabs}tabs[tabs.length] = "{$tabs[i].tab}"; {/section}
</script>

<div class="atktabs">
  {section name=i loop=$tabs}
    <span onclick="showTab('{$tabs[i].tab}')" id="tab_{$tabs[i].tab}" class="atktab">
    {$tabs[i].title}
    </span>
  {/section}
</div>
<div classs="atktabbed_content">
  {$content}
</div>