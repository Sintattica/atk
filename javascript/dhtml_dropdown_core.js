/*

  CASCADING POPUP MENUS v5.2beta (c) 2001-2003 Angus Turnbull, http://www.twinhelix.com
  This notice may not be altered or removed. See my site for licensing and more scripts!

*/

var isDOM=document.getElementById?1:0;
var isIE=document.all?1:0;
var isNS4=navigator.appName=='Netscape'&&!isDOM?1:0;
var isIE4=isIE&&!isDOM?1:0;
var isOp=window.opera?1:0;
var isDyn=isDOM||isIE||isNS4;
function getRef(id,par)
{
par=!par?document:(par.navigator?par.document:par);
return(isIE?par.all[id]:
(isDOM?(par.getElementById?par:par.ownerDocument).getElementById(id):
(isNS4?par.layers[id]:null)));
}
function getSty(id,par)
{
var r=getRef(id,par);
return r?(isNS4?r:r.style):null;
}
if(!window.LayerObj)var LayerObj=new Function('id','par',
'this.ref=getRef(id,par);this.sty=getSty(id,par);return this');
function getLyr(id,par){return new LayerObj(id,par)}
function LyrFn(fn,fc)
{
LayerObj.prototype[fn]=new Function('var a=arguments,p=a[0],px=isNS4||isOp?0:"px";'+
'with(this){'+fc+'}');
}
LyrFn('x','if(!isNaN(p))sty.left=p+px;else return parseInt(sty.left)');
LyrFn('y','if(!isNaN(p))sty.top=p+px;else return parseInt(sty.top)');
LyrFn('vis','sty.visibility=p');
LyrFn('bgColor','if(isNS4)sty.bgColor=p?p:null;'+
'else sty.background=p?p:"transparent"');
LyrFn('bgImage','if(isNS4)sty.background.src=p?p:null;'+
'else sty.background=p?"url("+p+")":"transparent"');
LyrFn('clip','if(isNS4)with(sty.clip){left=a[0];top=a[1];right=a[2];bottom=a[3]}'+
'else sty.clip="rect("+a[1]+"px "+a[2]+"px "+a[3]+"px "+a[0]+"px)" ');
LyrFn('write','if(isNS4)with(ref.document){write(p);close()}else ref.innerHTML=p');
LyrFn('alpha','var f=ref.filters,d=(p==null);if(f){'+
'if(!d&&sty.filter.indexOf("alpha")==-1)sty.filter+=" alpha(opacity="+p+")";'+
'else if(f.length&&f.alpha)with(f.alpha){if(d)enabled=false;else{opacity=p;enabled=true}}}'+
'else if(isDOM)sty.MozOpacity=d?"":p/100');
function setLyr(lVis,docW,par)
{
if(!setLyr.seq)setLyr.seq=0;
if(!docW)docW=0;
var obj=(!par?(isNS4?window:document.body):
(!isNS4&&par.navigator?par.document.body:par));
var IA='insertAdjacentHTML',AC='appendChild',newID='_js_layer_'+setLyr.seq++;
if(obj[IA])obj[IA]('beforeEnd','<div id="'+newID+'" style="position:absolute"></div>');
else if(obj[AC])
{
var newL=document.createElement('div');
obj[AC](newL);newL.id=newID;newL.style.position='absolute';
}
else if(isNS4)
{
var newL=new Layer(docW,obj);
newID=newL.id;
}
var lObj=getLyr(newID,par);
with(lObj)if(ref){vis(lVis);x(0);y(0);sty.width=docW+(isNS4?0:'px')}
return lObj;
}
if(!window.page)var page={win:window,minW:0,minH:0,MS:isIE&&!isOp,
db:document.compatMode&&document.compatMode.indexOf('CSS')>-1?'documentElement':'body'}
page.winW=function()
{with(this)return Math.max(minW,MS?win.document[db].clientWidth:win.innerWidth)}
page.winH=function()
{with(this)return Math.max(minH,MS?win.document[db].clientHeight:win.innerHeight)}
page.scrollX=function()
{with(this)return MS?win.document[db].scrollLeft:win.pageXOffset}
page.scrollY=function()
{with(this)return MS?win.document[db].scrollTop:win.pageYOffset}
function addProps(obj,data,names,addNull)
{
for(var i=0;i<names.length;i++)if(i<data.length||addNull)obj[names[i]]=data[i];
}
function PopupMenu(myName)
{
this.myName=myName;
this.showTimer=this.hideTimer=this.showDelay=0;
this.hideDelay=500;
this.menu=[];
this.litNow=[];
this.litOld=[];
this.overM='';
this.overI=0;
this.hideDocClick=1;
this.actMenu=null;
PopupMenu.list[myName]=this;
}
PopupMenu.list=[];
with(PopupMenu)
{
prototype.callEvt=function(mN,iN,evt)
{
var i=this.menu[mN][iN],r=this[evt]?(this[evt](mN,iN)===false):0;
if(i[evt])
{
if(i[evt].substr)i[evt]=new Function('mN','iN',i[evt]);
r|=(i[evt](mN,iN)===false);
}
return r;
}
prototype.over=function(mN,iN){with(this)
{
clearTimeout(hideTimer);
overM=mN;
overI=iN;
var cancel=iN?callEvt(mN,iN,'onmouseover'):0;
litOld=litNow;
litNow=[];
var litM=mN,litI=iN;
if(mN)do
{
litNow[litM]=litI;
litI=menu[litM][0].parentItem;
litM=menu[litM][0].parentMenu;
}while(litM);
var same=1;
for(var z in menu)same&=(litNow[z]==litOld[z]);
if(same)return;
clearTimeout(showTimer);
for(var thisM in menu)with(menu[thisM][0])
{
if(!lyr)continue;
lI=litNow[thisM];
oI=litOld[thisM];
if(lI!=oI)
{
if(lI)changeCol(thisM,lI);
if(oI)changeCol(thisM,oI);
}
if(!lI)clickDone=0;
if(isRoot)continue;
if(lI&&!visNow)doVis(thisM,true);
if(!lI&&visNow)doVis(thisM,false);
}
nextMenu='';
if(!cancel&&menu[mN]&&menu[mN][iN].type=='sm:')
{
var m=menu[mN],targ=m[iN].href;
if(!menu[targ])return true;
if(m[0].clickSubs&&!m[0].clickDone)return true;
nextMenu=targ;
if(showDelay)showTimer=setTimeout(myName+'.doVis("'+targ+'",true)',showDelay);
else doVis(targ,true);
}
}}
prototype.out=function(mN,iN){with(this)
{
if(mN!=overM||iN!=overI)return;
var thisI=menu[mN][iN],cancel=callEvt(mN,iN,'onmouseout');
if(thisI.href!=nextMenu)
{
clearTimeout(showTimer);
nextMenu='';
}
if(hideDelay&&!cancel)
{
var delay=(menu[mN][0].isRoot&&(thisI.type!='sm:'))?50:hideDelay;
hideTimer=setTimeout(myName+'.over("",0)',delay);
}
overM='';
overI=0;
}}
prototype.click=function(mN,iN){with(this)
{
var m=menu[mN];
if(callEvt(mN,iN,'onclick'))return;
with(m[iN])switch(type)
{
case 'sm:':
{
if(m[0].clickSubs)
{
m[0].clickDone=1;
doVis(href,true);
return false;
}
break;
}
case 'js:':{eval(href);break}
case '':type='window';
default:if(href)eval(type+'.location.href="'+href+'"');
}
over('',0);
}}
prototype.changeCol=function(mN,iN,fc){with(this.menu[mN][iN])
{
if(!lyr||!lyr.ref)return;
var bgFn=outCol!=overCol?(outCol.indexOf('.')==-1?'bgColor':'bgImage'):0;
var ovr=(this.litNow[mN]==iN)?1:0,doFX=(!fc&&this.litNow[mN]!=this.litOld[mN]);
var col=ovr?overCol:outCol;
if(fade[0])
{
clearTimeout(timer);
col='#';
count=Math.max(0,Math.min(count+(2*ovr-1)*parseInt(fade[ovr][0]),100));
var oc,nc,hexD='0123456789ABCDEF';
for(var i=1;i<4;i++)
{
oc=parseInt('0x'+fade[0][i]);
nc=parseInt(oc+(parseInt('0x'+fade[1][i])-oc)*(count/100));
col+=hexD.charAt(Math.floor(nc/16)).toString()+hexD.charAt(nc%16);
}
if(count%100>0)timer=setTimeout(this.myName+'.changeCol("'+mN+'",'+iN+',1)',50);
}
if(bgFn&&isNS4)lyr[bgFn](col);
var reCSS=(overClass!=outClass||outBorder!=overBorder);
if(doFX)with(lyr)
{
if(!this.noRW&&(overText||overInd||isNS4&&reCSS))write(this.getHTML(mN,iN,ovr));
if(!isNS4&&reCSS)
{
ref.className=(ovr?overBorder:outBorder);
var chl=(isDOM?ref.childNodes:ref.children);
if(chl&&!overText)for(var i=0;i<chl.length;i++)
chl[i].className=ovr?overClass:outClass;
}
}
if(bgFn&&!isNS4)lyr[bgFn](col);
if(doFX&&outAlpha!=overAlpha)lyr.alpha(ovr?overAlpha:outAlpha);
}}
prototype.position=function(posMN){with(this)
{
for(mN in menu)if(!posMN||posMN==mN)with(menu[mN][0])
{
if(!lyr||!lyr.ref||!visNow)continue;
var pM,pI,newX=eval(offX),newY=eval(offY);
if(!isRoot)
{
pM=menu[parentMenu];
pI=pM[parentItem].lyr;
if(!pI)continue;
}
var eP=eval(par),pW=(eP&&eP.navigator?eP:window);
with(pW.page)var sX=scrollX(),wX=sX+winW(),sY=scrollY(),wY=winH()+sY;
wX=isNaN(wX)||!wX?9999:wX;
wY=isNaN(wY)||!wY?9999:wY;
var sb=page.MS?5:20;
if(pM&&typeof(offX)=='number')newX=Math.max(sX,
Math.min(newX+pM[0].lyr.x()+pI.x(),wX-menuW-sb));
if(pM&&typeof(offY)=='number')newY=Math.max(sY,
Math.min(newY+pM[0].lyr.y()+pI.y(),wY-menuH-sb));
lyr.x(newX);
lyr.y(newY);
}
}}
prototype.doVis=function(mN,show){with(this)
{
var m=menu[mN],mA=(show?'show':'hide')+'Menu';
if(!m)return;
m[0].visNow=show;
if(show)position(mN);
if(this[mA])this[mA](mN);
else m[0].lyr.vis(show?'visible':'hidden');
}}
window.ItemStyle=function()
{
var names=['len','spacing','popInd','popPos','pad','outCol','overCol','outClass',
'overClass','outBorder','overBorder','outAlpha','overAlpha','normCursor','nullCursor'];
addProps(this,arguments,names,true);
}
prototype.startMenu=function(mName){with(this)
{
if(!menu[mName]){menu[mName]=new Array();menu[mName][0]=new Object();}
actMenu=menu[mName];
aM=actMenu[0];
actMenu.length=1;
var names=['name','isVert','offX','offY','width','itemSty','par','clickSubs',
'clickDone','visNow','parentMenu','parentItem','oncreate','isRoot'];
addProps(aM,arguments,names,true);
aM.extraHTML='';
aM.menuW=aM.menuH=0;
if(!aM.lyr)aM.lyr=null;
if(mName.substring(0,4)=='root')
{
aM.isRoot=true;
aM.oncreate=new Function('this.visNow=true;'+
myName+'.position("'+mName+'");this.lyr.vis("visible")');
}
return aM;
}}
prototype.addItem=function(){with(this)with(actMenu[0])
{
var aI=actMenu[actMenu.length]=new Object();
var names=['text','href','type','itemSty','len','spacing','popInd','popPos',
'pad','outCol','overCol','outClass','overClass','outBorder','overBorder',
'outAlpha','overAlpha','normCursor','nullCursor',
'iX','iY','iW','iH','overText','overInd','lyr','onclick','onmouseover','onmouseout'];
addProps(aI,arguments,names,true);
var iSty=(arguments[3]?arguments[3]:actMenu[0].itemSty);
for(prop in iSty)if(aI[prop]+''=='undefined')aI[prop]=iSty[prop];
var r=RegExp,re=/^SWAP:(.*)\^(.*)$/;
if(aI.text.match(re)){aI.text=r.$1;aI.overText=r.$2}
if(aI.popInd.match(re)){aI.popInd=r.$1;aI.overInd=r.$2}
aI.timer=aI.count=0;
aI.fade=[];
for(var i=0;i<2;i++)
{
var oC=i?'overCol':'outCol';
if(aI[oC].match(/^(\d+)\#(..)(..)(..)$/))
{
aI[oC]='#'+r.$2+r.$3+r.$4;
aI.fade[i]=[r.$1,r.$2,r.$3,r.$4];
}
}
if(aI.outBorder&&isNS4)aI.pad++;
aI.iW=(isVert?width:aI.len);
aI.iH=(isVert?aI.len:width);
var lastGap=(actMenu.length>2)?actMenu[actMenu.length-2].spacing:0;
var spc=((actMenu.length>2)&&aI.outBorder?1:0);
if(isVert)
{
menuH+=lastGap-spc;
aI.iX=0;aI.iY=menuH;
menuW=width;menuH+=aI.iH;
}
else
{
menuW+=lastGap-spc;
aI.iX=menuW;aI.iY=0;
menuW+=aI.iW;menuH=width;
}
if(aI.outBorder&&(page.db=='documentElement'||isOp&&!document.compatMode||
document.doctype&&document.doctype.name.indexOf('.dtd')>-1||isDOM&&!isIE))
{
aI.iW-=2;
aI.iH-=2;
}
return aI;
}}
prototype.getHTML=function(mN,iN,isOver){with(this)
{
var itemStr='';
with(menu[mN][iN])
{
var textClass=(isOver?overClass:outClass),txt=(isOver&&overText?overText:text),
popI=(isOver&&overInd?overInd:popInd);
if((type=='sm:')&&popI)
{
if(isNS4)itemStr+='<layer class="'+textClass+'" left="'+((popPos+iW)%iW)+
'" top="'+pad+'" height="'+(iH-2*pad)+'">'+popI+'</layer>';
else itemStr+='<div class="'+textClass+'" style="position:absolute;left:'+
((popPos+iW)%iW)+'px;top:'+pad+'px;height:'+(iH-2*pad)+'px">'+popI+'</div>';
}
if(isNS4)itemStr+=(outBorder?'<span class="'+(isOver?overBorder:outBorder)+
'"><spacer type="block" width="'+(iW-8)+'" height="'+(iH-8)+'"></span>':'')+
'<layer left="'+pad+'" top="'+pad+'" width="'+(iW-2*pad)+'" height="'+
(iH-2*pad)+'"><a class="'+textClass+'" href="#" '+
'onClick="return false" onMouseOver="status=\'\';'+myName+'.over(\''+mN+'\','+
iN+');return true">'+txt+'</a></layer>';
else itemStr+='<div class="'+textClass+'" style="position:absolute;left:'+pad+
'px;top:'+pad+'px;width:'+(iW-2*pad)+'px;height:'+(iH-2*pad)+'px">'+
txt+'</div>';
}
return itemStr;
}}
prototype.update=function(docWrite,upMN){with(this)
{
if(!isDyn)return;
for(mN in menu)with(menu[mN][0])
{
if(upMN&&(upMN!=mN))continue;
var str='';
for(var iN=1;iN<menu[mN].length;iN++)with(menu[mN][iN])
{
var itemID=myName+'_'+mN+'_'+iN;
var targM=menu[href];
if(targM&&(type=='sm:'))
{
targM[0].parentMenu=mN;
targM[0].parentItem=iN;
}
var isImg=(outCol.indexOf('.')!=-1)?true:false;
if(!isIE)
{
if(normCursor=='hand')normCursor='pointer';
if(nullCursor=='hand')nullCursor='pointer';
}
if(isDOM||isIE4)
{
str+='<div id="'+itemID+'" '+(outBorder?'class="'+outBorder+'" ':'')+
'style="position:absolute;left:'+iX+'px;top:'+iY+'px;width:'+iW+
'px;height:'+iH+'px;z-index:1000;'+
(outCol?'background:'+(isImg?'url('+outCol+')':outCol):'')+
((typeof(outAlpha)=='number')?';filter:alpha(opacity='+outAlpha+');-moz-opacity:'+
(outAlpha/100):'')+
';cursor:'+((type!='sm:'&&href)?normCursor:nullCursor)+'" ';
}
else if(isNS4)
{
str+='<layer id="'+itemID+'" left="'+iX+'" top="'+iY+'" width="'+
iW+'" height="'+iH+'" z-index="1000" '+
(outCol?(isImg?'background="':'bgcolor="')+outCol+'" ':'');
}
var evtMN='(\''+mN+'\','+iN+')"';
str+='onMouseOver="'+myName+'.over'+evtMN+' onMouseOut="'+myName+'.out'+evtMN+
' onClick="'+myName+'.click'+evtMN+'>'+getHTML(mN,iN,false)+
(isNS4?'</layer>':'</div>');
}
var eP=eval(par);
var sR=myName+'.setupRef('+(docWrite?1:0)+',"'+mN+'")';
if(isOp)setTimeout(sR,1000);
var mVis=(isOp&&isRoot)?'visible':'hidden';
if(docWrite)
{
var targFr=(eP&&eP.navigator?eP:window);
targFr.document.write('<div id="'+myName+'_'+mN+'_Div" style="position:absolute;'+
'visibility:'+mVis+';left:-1000px;top:0px;width:'+(menuW+2)+'px;height:'+
(menuH+2)+'px;z-index:1000">'+str+extraHTML+'</div>');
}
else
{
if(!lyr||!lyr.ref)lyr=setLyr(mVis,menuW,eP);
else if(isIE4)setTimeout(myName+'.menu.'+mN+'[0].lyr.sty.width='+(menuW+2),50);
with(lyr){sty.zIndex=1000;write(str+extraHTML)}
}
if(!isOp)setTimeout(sR,100);
}
}}
prototype.setupRef=function(docWrite,mN){with(this)with(menu[mN][0])
{
if(docWrite||!lyr||!lyr.ref)lyr=getLyr(myName+'_'+mN+'_Div',eval(par));
for(var i=1;i<menu[mN].length;i++)
menu[mN][i].lyr=getLyr(myName+'_'+mN+'_'+i,(isNS4?lyr.ref:eval(par)));
menu[mN][0].lyr.clip(0,0,menuW+2,menuH+2);
if(menu[mN][0].oncreate)oncreate();
}}
}