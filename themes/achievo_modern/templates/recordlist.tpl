
<table cellspacing="0" cellpadding="0" class="recordListContainer">
  <tr>
    <td>
        <table id="{$listid}" class="recordList" cellpadding="0" cellspacing="0">
        {if count($search)}
              <!-- search row -->
              <tr>
              {$searchstart}
              {foreach from=$search item=col}
                  <th class="recordListSearch" valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                    {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                  </th>
              {/foreach}
              {$searchend}
              </tr>
            {/if}
            <!-- header -->
            <tr>
              {section name=headerloop loop=$header}
                <th valign="{$vorientation}" {if isset($header[headerloop].htmlattributes)}{$header[headerloop].htmlattributes}{/if} 
                 {if $smarty.section.headerloop.index===0}class="recordListThFirst"{else}class="recordListTh"{/if}> 
                  {if $header[headerloop].content != ""}{$header[headerloop].content}{else}&nbsp;{/if}
                </th>
              {/section}
            </tr>

            {if count($sort)}
              <!-- search row -->
              <tr>
              {$sortstart}
              {foreach from=$sort item=col}
                  <th valign="{$vorientation}" {if isset($col.htmlattributes)}{$col.htmlattributes}{/if}>
                    {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                  </th>
              {/foreach}
              {$sortend}
              </tr>
            {/if}

            <!-- records -->
            {$liststart}

            {foreach from=$rows item=row}
              <tr id="{$row.id}" class="row{if $row.rownum % 2 == 0 }1{else}2{/if}" {if $row.background!=""}style="background-color:{$row.background}" {/if}
                   onmouseover="highlightrow(this, '{$row.highlight}')"
                   onmouseout="resetrow(this)"
                   onclick="selectrow(this, '{$listid}', {$row.rownum})">
               {section name=colloop loop=$row.cols}
                <{if $row.type == "subtotal"}th{else}td {if $smarty.section.colloop.index===0}class="recordListTdFirst"{else}class="recordListTd"{/if} 
                 {/if} valign="{$vorientation}" {if isset($row.cols[colloop].htmlattributes)}{$row.cols[colloop].htmlattributes}{/if}>
                  {if $row.cols[colloop].content != ""}{$row.cols[colloop].content}{else}&nbsp;{/if}
                </td>
              {/section}
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
     <td>
       <table border="0" cellspacing="0" cellpadding="0">
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
