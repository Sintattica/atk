<table border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left">      
        <table class="recordlist" cellspacing=0>
            <!-- header -->
            <tr> 
              {foreach from=$header item=col}
                <th valign="{$vorientation}" {$col.htmlattributes}>
                  {if $col.content != ""}{$col.content}{else}&nbsp;{/if}                     
                </th>
              {/foreach}
            </tr>
                            
            {if count($search)}
              <!-- search row -->
              <tr>
              {$searchstart}
              {foreach from=$search item=col}
                  <th valign="{$vorientation}" {$col.htmlattributes}>
                    {if $col.content != ""}{$col.content}{else}&nbsp;{/if}                     
                  </th>
              {/foreach}
              {$searchend}
              </tr>
            {/if}
              
            <!-- records -->
            {$liststart}
              
            {foreach from=$rows item=row}
              <tr class="row{if $row.rownum % 2 == 0 }1{else}2{/if}" {if $row.background!=""}style="background-color:{$row.background}" {/if}
                   onmouseover="if (typeof(this.style) != 'undefined') this.style.backgroundColor = '{$row.highlight}'"
                   onmouseout="if (typeof(this.style) != 'undefined') this.style.backgroundColor = '{$row.background}'">
              {foreach from=$row.cols item=col}
                <td valign="{$vorientation}" {$col.htmlattributes}>
                  {if $col.content != ""}{$col.content}{else}&nbsp;{/if}
                </td>
              {/foreach}
            </tr>
            {/foreach}
            
            {if count($total)}
            <!-- totals row -->
              <tr>              
              {foreach from=$total item=col}
                  <th valign="{$vorientation}" {$col.htmlattributes}>
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
