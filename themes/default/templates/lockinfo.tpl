<div style="background-color: #FEFCD1; padding: 2px; border: 1px solid #666; color: #000; font: 11px arial, helvetica">
<strong>{atktext 'in_use_by'}:</strong><br/>
{foreach from=$locks item='lock'}
  {assign var='stamp' value="$lock.lock_stamp|strtotime}
  {capture assign='format'}{atktext 'date_format_view'} H:i{/capture}
  {capture assign='date'}{$stamp|atkFormatDate:$format}{/capture}
  {atktext 'lock_info_line' user_id=$lock.user_id user_ip=$lock.user_ip lock_date=$date}<br/>
{/foreach}
</div>