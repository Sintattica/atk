<br><br>
<div align="center">
  <div class="box" style="width: 293px;">
    <table cellspacing="0px" cellpadding="0px" border="0">
      <tr>
        <td class="boxtitle" colspan="5" width="293" nobr nowrap>Login</td>
      </tr>
      <tr>
        <td height="100%" valign="top" align="center">
						
          <table width="100%" border="0" cellpadding="2" cellspacing="0">
      	  <tr>
      	  <td>
            <table width="100%" border="0" cellpadding="6" cellspacing="0">
              <tr>
                <td bgcolor="#FFFFFF" align="left" class="block">
                  {atkconfig var="theme_logo" smartyvar="theme_logo"}
                  {if $theme_logo != ''}
                    <img src="{$theme_logo}" alt='' align=left>
                  {/if}
                </td>            
              </tr>
            </table>
      	  </td>
      	  </tr>
          </table>

          <table width="100%" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td align="left" class="block">
                  {$content}
              </td>            
            </tr>
          </table>
							
        </td>
      </tr>
    </table>
  </div>
</div>
