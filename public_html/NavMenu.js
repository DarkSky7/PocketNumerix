//----------DHTML Menu Created using AllWebMenus PRO ver 4.2-#630---------------
//C:\Documents and Settings\Mike\My Documents\PocketNumerix\Menus\BlackAndBlueMenus.awm
var awmMenuName='NavMenu';
var awmLibraryBuild=630;
var awmLibraryPath='/awmdata';
var awmImagesPath='/awmdata';
var awmSupported=(navigator.appName + navigator.appVersion.substring(0,1)=="Netscape5" || document.all || document.layers || navigator.userAgent.indexOf('Opera')>-1 || navigator.userAgent.indexOf('Konqueror')>-1)?1:0;
if (awmAltUrl!='' && !awmSupported) window.location.replace(awmAltUrl);
if (awmSupported){
var nua=navigator.userAgent,scriptNo=(nua.indexOf('Safari')>-1)?7:(nua.indexOf('Gecko')>-1)?2:((document.layers)?3:((nua.indexOf('Opera')>-1)?4:((nua.indexOf('Mac')>-1)?5:1)));
var mpi=document.location,xt="";
var mpa=mpi.protocol+"//"+mpi.host;
var mpi=mpi.protocol+"//"+mpi.host+mpi.pathname;
if(scriptNo==1){oBC=document.all.tags("BASE");if(oBC && oBC.length) mpi=oBC[0].href;}
while (mpi.search(/\\/)>-1) mpi=mpi.replace("\\","/");
mpi=mpi.substring(0,mpi.lastIndexOf("/")+1);
var e=document.getElementsByTagName("SCRIPT");
for (var i=0;i<e.length;i++){if (e[i].src){if (e[i].src.indexOf(awmMenuName+".js")!=-1){xt=e[i].src.split("/");if (xt[xt.length-1]==awmMenuName+".js"){xt=e[i].src.substring(0,e[i].src.length-awmMenuName.length-3);if (e[i].src.indexOf("://")!=-1){mpi=xt;}else{if(xt.substring(0,1)=="/")mpi=mpa+xt; else mpi+=xt;}}}}}
while (mpi.search(/\/\.\//)>-1) {mpi=mpi.replace("/./","/");}
var awmMenuPath=mpi.substring(0,mpi.length-1);
while (awmMenuPath.search("'")>-1) {awmMenuPath=awmMenuPath.replace("'","&#39;");}
document.write("<SCRIPT SRC='"+awmMenuPath+awmLibraryPath+"/awmlib"+scriptNo+".js'><\/SCRIPT>");
var n=null;
awmzindex=1000;
}

var awmSubmenusFrame='';
var awmSubmenusFrameOffset;
var awmOptimize=0;
var awmUseTrs=0;
var awmSepr=["0","","",""];
function awmBuildMenu(){
if (awmSupported){
awmImagesColl=["main-header.gif",7,29,"main-footer.gif",7,29,"main-button-tile.gif",5,29,"main-buttonOver-tile.gif",5,29,"sub-button-tile.gif",5,21,"sub-buttonOver-tile.gif",5,21,"sub-button-left.gif",3,21,"sub-buttonOver-left.gif",3,21,"sub-button-right.gif",3,21,"sub-buttonOver-right.gif",3,21];
awmCreateCSS(1,2,1,'#FFFFFF',n,n,'14px sans-serif',n,'none',0,'#000000','0px 0px 0px 0',0);
awmCreateCSS(0,2,1,'#FFFFFF',n,n,'14px sans-serif',n,'none',0,'#000000','0px 0px 0px 0',0);
awmCreateCSS(1,2,1,'#000000',n,n,'14px sans-serif',n,'none',0,'#000000','0px 0px 0px 0',0);
awmCreateCSS(0,1,0,n,n,n,n,n,'none',0,'#000000',0,0);
awmCreateCSS(1,2,1,'#00FFFF',n,2,'bold 12px Tahoma',n,'none',0,'#000000','5px 10px 5px 10',1);
awmCreateCSS(0,2,1,'#FFFF00',n,3,'bold 12px Tahoma',n,'none',0,'#000000','5px 10px 5px 10',1);
awmCreateCSS(1,2,0,'#FFFF00',n,4,'bold 11px Tahoma',n,'none',0,'#000000','0px 10px 0px 10',1);
awmCreateCSS(0,2,0,'#FFFF00',n,5,'bold 11px Tahoma',n,'none',0,'#000000','0px 10px 0px 10',1);
awmCreateCSS(1,2,0,'#00FFFF',n,4,'bold 11px Tahoma',n,'none',0,'#000000','0px 10px 0px 10',1);
var s0=awmCreateMenu(0,0,0,0,1,0,0,1,0,0,0,0,1,3,0,0,1,n,n,100,0,0,0,0,0);
it=s0.addItemWithImages(0,1,1,"","","","",0,0,0,3,3,3,n,n,n,"",n,n,n,n,n,0,0,0,n,n,n,n,n,n,0,0,0,0);
it=s0.addItem(4,5,5,"Home",n,n,"","Welcome.php",n,n,n,"Welcome.php",n,0,0,2,0);
var s1=it.addSubmenu(0,0,-4,0,0,0,0,3,0,1,0,n,n,100,0,2,0);
it=s1.addItemWithImages(6,7,7,"Welcome",n,n,"",n,n,n,3,3,3,n,n,n,"Welcome.php",n,n,n,"Welcome.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s1.addItemWithImages(8,7,7,"About",n,n,"",n,n,n,3,3,3,n,n,n,"About.php",n,n,n,"About.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s0.addItem(4,5,5,"Products",n,n,"","Products.php",n,n,n,"Products.php",n,0,0,2,0);
var s1=it.addSubmenu(0,0,-4,0,0,0,0,3,0,1,0,n,n,100,0,1,0);
it=s1.addItemWithImages(8,7,7,"BondManager",n,n,"",n,n,n,3,3,3,n,n,n,"BondManager.php",n,n,n,"BondManager.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s1.addItemWithImages(8,7,7,"NillaHedge",n,n,"",n,n,n,3,3,3,n,n,n,"NillaHedge.php",n,n,n,"NillaHedge.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s1.addItemWithImages(8,7,7,"StrategyExplorer",n,n,"",n,n,n,3,3,3,n,n,n,"StrategyExplorer.php",n,n,n,"StrategyExplorer.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s1.addItemWithImages(8,7,7,"YieldCurveFitter",n,n,"",n,n,n,3,3,3,n,n,n,"YieldCurveFitter.php",n,n,n,"YieldCurveFitter.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s0.addItem(4,5,5,"Purchasing",n,n,"","Store.php",n,n,n,"Store.php",n,0,0,2,0);
var s1=it.addSubmenu(0,0,-4,0,0,0,0,3,0,1,0,n,n,100,0,4,0);
it=s1.addItemWithImages(8,7,7,"Store",n,n,"",n,n,n,3,3,3,n,n,n,"Store.php",n,n,n,"Store.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s0.addItem(4,5,5,"Support",n,n,"","Documentation.php",n,n,n,"Documentation.php",n,0,0,2,0);
var s1=it.addSubmenu(0,0,-4,0,0,0,0,3,0,1,0,n,n,100,0,3,0);
it=s1.addItemWithImages(8,7,7,"Documentation",n,n,"",n,n,n,3,3,3,n,n,n,"Documentation.php",n,n,n,"Documentation.php",n,0,0,2,6,7,7,8,9,9,1,1,1,0);
it=s0.addItemWithImages(2,1,1,"","","","",1,1,1,3,3,3,n,n,n,"",n,n,n,n,n,0,0,0,n,n,n,n,n,n,0,0,0,0);
s0.pm.buildMenu();
}}
