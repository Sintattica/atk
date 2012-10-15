{if isset($formstart)}{$formstart}{/if}
<table border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left">
        <table id="{$listid}" class="recordlist" cellspacing=0>
            <!-- header -->
            <tr>
              {foreach from=$header item=col}
                <th valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                  {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                </th>
              {/foreach}
            </tr>

            {if count($sort)}
              <!-- search row -->
              <tr>
              {$sortstart}
              {foreach from=$sort item=col}
                  <th valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if} align="right">
                    {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                  </th>
              {/foreach}
              {$sortend}
              </tr>
            {/if}

            {if count($search)}
              <!-- search row -->
              <tr>
              {$searchstart}
              {foreach from=$search item=col}
                  <th valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                    {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                  </th>
              {/foreach}
              {$searchend}
              </tr>
            {/if}

            <!-- records -->
            {$liststart}

            {foreach from=$rows item=row}
              <tr id="{$row.id}" class="row{if $row.rownum % 2 == 0 }1{else}2{/if}" {if $row.background!=""}style="background-color:{$row.background}" {/if}
                   onmouseover="highlightrow(this, '{$row.highlight}')"
                   onmouseout="resetrow(this)"
                   onclick="selectrow(this, '{$listid}', {$row.rownum})">
              {foreach from=$row.cols item=col name=i}
                <{if $row.type == "subtotal"}th{else}td{/if} 
                  {if $smarty.foreach.i.index > 0}
                    onclick="rl_try('{$listid}', event, {$row.rownum}, ['select', 'edit', 'view'], false);"{/if} 
                  valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                  {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                </td>
              {/foreach}
            </tr>
            {/foreach}

            {if count($total)}
            <!-- totals row -->
              <tr>
              {foreach from=$total item=col}
                  <th valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                    {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                  </th>
              {/foreach}
              </tr>
            {/if}

      </table>
    </td>
  </tr>
  {if $mra!=""}
  <!-- multirecord actions -->
   <tr>
     <td align="left">
       <table border="0" cellspacing="0" cellpadding="2">
         <tr>
           <td valign="top"><img src="{$atkroot}atk/images/arrow.gif" border="0"></td>
           <td>
             {$mra} {$listend}
           </td>
         </tr>
       </table>
     </td>
   </tr>
  {/if}
</table>
{if isset($formend)}{$formend}{/if}