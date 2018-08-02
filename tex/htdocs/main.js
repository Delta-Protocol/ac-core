var endpoint="https://trocavaluta.com/api";

var curcolor= {};
curcolor["EUR"]="rgba(255, 255, 100, 1)";
curcolor["GBP"]="rgba(200, 200, 255, 1)";
curcolor["USD"]="rgba(100, 255, 100, 1)";
curcolor["AUD"]="rgba(255, 200, 200, 1)";

var symbol= {};
symbol["EUR"]="€";
symbol["GBP"]="£";
symbol["USD"]="$";
symbol["AUD"]="A$";

var curid= new Object;

var puo=new popup_order();
var aw=new account_window();
var tw=new trading_window();

function rsz(){
	    var ts=$("#ts");
	    var hdr=$("#hdr");
	    var content=$("#content");
	    var all=$("#all");
	    var win=$(window);
	    all.css('top',0);
	    all.css('left',0);
	    all.width(win.width());
	    all.height(win.height());

	    var posa=ts.position();
	    var w=win.width()-(win.width()-posa.left)-0;
	    content.width(w);
	    hdr.width(w);
	    var pos=hdr.position();
//alert(hdr.width()+" "+hdr.height());
	    var h0=pos.top;
//alert(pos.left+" "+pos.top);
	    pos.top+=hdr.height();
//alert(pos.left+" "+pos.top);
	    content.css('top',pos.top);
	    content.css('left',pos.left);
	    content.height(win.height()-pos.top-0);
	    ts.css('top',0);
	    ts.height(win.height()-0);

	    $(".market").width(content.width()-15);
	    $(".m").width(content.width()-15);

}

function dodimstep(alpha) {
	alpha=Math.round(alpha*100)/100;
//	var v=parseInt(255*alpha).toString(16);
//	if (v.length < 2) v="0"+v;
	var newc = "rgba(0,0,0,"+alpha+")"; 
	$("#dim").css("background-color",newc);
	if (alpha>0) {
		setTimeout(function(){dodimstep(alpha-0.05)},15);
	}
	else {
		$("#dim").remove();
//		$("#dim").css("display","none");
	}
}


//$(document).ready(function () {
function entry() {
	var custom1=" \
.arrow-btn { \
    cursor: pointer; \
    display: block; \
    width: 15px; \
    height: 9px; \
    margin: 0; \
    padding: 0; \
    border: 1px solid #000; \
z    background-color: #eee; \
    -moz-border-radius: 0px; \
    -webkit-border-radius: 0px; \
    border-radius:0px; \
} \
";

	var custom2=" \
.arrow-up { \
    height: 7px; \
    background: transparent url(sprite.png) no-repeat 0px -146px; \
} \
";

	var custom3=" \
.arrow-down { \
    height: 7px; \
    background: transparent url(sprite.png) no-repeat 0px -151px; \
} \
";

	if(/chrom(e|ium)/.test(navigator.userAgent.toLowerCase())){
		custom1=custom1.replace("width: 15px;","width: 13px;");
		custom2=custom2.replace("0px","2px");
		custom3=custom3.replace("0px","2px");
	}
	$('head').append('<style id="customCSS">'+custom1+custom2+custom3+'</style>');
	

	$( window ).bind("resize", function(){rsz();});

//delete_cookies();

	tw.init();
	rsz();
	if (window.location.href.indexOf("quick")!=-1) {
		$("#dim").remove();
	}
	else {
		setTimeout(function(){dodimstep(1);},1000);
	}

}
//);

entry();
