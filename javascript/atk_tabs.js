            var NS4 = (document.layers) ? 1 : 0;
            var IE = (document.all) ? 1 : 0;
            var DOM = 0; 
            if (parseInt(navigator.appVersion) >=5) {DOM=1};
            
            var lastHeader;
            var currShow;

            function changeCont(tgt,header) {

                target=('T' +tgt);
                if (DOM) {

                    // Hide the last one, and flip the tab color back.
                    currShow.style.visibility="hidden";
                    if ( lastHeader ) { 
                       lastHeader.style.background = tab_off; 
                       lastHeader.style.fontWeight="normal"; 
                    }
                
                    // Show this one, and make the tab silver
                    var flipOn = document.getElementById(target);			
                    flipOn.style.visibility="visible";

                    var thisTab = document.getElementById(header);			
                    thisTab.style.background = tab_on;

                    // Save for next go'round
                    currShow=document.getElementById(target);
                    lastHeader = document.getElementById(header);

                    return false;
                }

                else if (IE) {

                    // Hide the last one, and flip the tab color back.
                    currShow.style.visibility = 'hidden';
                    if (lastHeader) { 
                        lastHeader.style.background = tab_off; 
                        lastHeader.style.fontWeight="normal";
                    }

                    // Show this one, and make the tab silver
                    document.all[target].style.visibility = 'visible';
                    document.all[header].style.background = tab_on;

                    // Save for next go'round
                    currShow=document.all[target];
                    lastHeader=document.all[header];

                    return false;
                }
                    
                else if (NS4) {

                    // Hide the last one, and flip the tab color back.
                    currShow.visibility = 'hide';
                    // if (lastHeader) { lastHeader.bgColor = tab_off; }

                    // Show this one, and make the tab silver
                    document.layers[target].visibility = 'show';
                    // document.layers[header].bgColor  = tab_on;

                    // Save for next go'round
                    currShow=document.layers[target];
                    // lastHeader=document.layers[header];

                    return false;
                }
                    
                // && (version >=4)
                else {
                    window.location=('#A' +target);
                    return true;
                }


            }

            function generateTabs() {

                var output = '';

                for ( var x = 1; x <= num_rows; x++ ) { 

                    if( x > 1 ) { 
                      top = top + 20;
                      left = left - 15;
                      width = width + 15;
                    }

                    output += '<div id="tabstop" style="position:absolute; ';
                    output += 'left:' + left + 'px;';
                    output += 'top:' + top + 'px; ';
                    output += 'width:' + width + 'px; z-index:1;">\n';
                    output += '<table border="0" cellpadding="0" cellspacing="1">\n';                    
                    output += '<tr valign="top">\n';

                       for ( var z = 1; z < rows[x].length; z++ ) {

                          var tid = "tab" + x + z;
                          var did = x + z;

                          if (tabSelectMode == "click")
                          {
                           output += '<td id="' + tid +'" class="tab-button">&nbsp;<a href="#" onClick="changeCont(\'' + x + z + '\', \'' + tid + '\'); return false;" onFocus="if(this.blur)this.blur()">' + rows[x][z] + '</a>&nbsp;</td>\n';
                          }
                          else
                          {
                           output += '<td id="' + tid +'" class="tab-button">&nbsp;<a href="#" onMouseOver="changeCont(\'' + x + z + '\', \'' + tid + '\'); return false;" onFocus="if(this.blur)this.blur()">' + rows[x][z] + '</a>&nbsp;</td>\n';
                          }
                          output += '<td>&nbsp;</td>\n';
                       }

                    output += '</tr>\n';
                    output += '</table>\n';
                    output += '</div>\n\n';

                }

                self.document.write(output);

            }
