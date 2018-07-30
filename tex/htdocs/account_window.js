function account_window() {
	this.loggedin=false;
	this.uid="";
	this.pwd="";
	this.curs=new Array;
	this.walletsvisible=false;

	this.rw=new register_window();

	this.ww=new wallets_window();

	this.dw=new deposit_window();

	this.show_deposit_withdraw_window=function () { 
		$("#dw").html(this.dw.html());
		$("#dw").css("background-color","#ff8");
		$("#dw").css('z-index','9999');

		$("#dw").width($(".acwnd").width());
		var pos=$("#ac").position();
		var left=parseInt(pos.left)+parseInt($(".acwnd").width())/2-parseInt($("#dw").width())/2;
		var top=pos.top+parseInt($(".acwnd").height());
		$("#dw").css("left",left+"px");
		$("#dw").css("top",top+"px");
		$("#dw").css('display','block');

		this.refresh()
		var o=this;
		setTimeout(function(){	$("#dw").css("background-color",""); o.focus_deposit_window()},500);
	}

	this.show_deposit_window=function (curid) { 
		this.dw.curid=curid;
		this.dw.withdraw=false;
		this.show_deposit_withdraw_window();

	}
//this=account_window
	this.show_withdraw_window=function (curid) { 
		this.dw.curid=curid;
		this.dw.withdraw=true;
		this.show_deposit_withdraw_window();
	}
	this.focus_login_button=function() {
		$('#lgnb').focus();
	}
	this.focus_deposit_window=function () { 
		this.dw.focus();
	}
	this.focus_deposit_window2=function () { 
		this.dw.focus2();
	}
	this.hide_deposit_window=function () { 
		$("#dw").css('display','none');
	}


	this.updated_curs=function(curs) {
		this.curs=curs;
		this.repaint_wallets();
		this.repaint_deposit_window();
	}

	this.parselo=function(data,handler) {
		var lines = data.split('\n');
		var length = lines.length;
		var curs= new Array;
		for(var i = 0; i < length; i++){
		        var fields=lines[i].split(" ");
			if (fields.length!=2) {
				continue;
			}
			var curid=fields[0];
			var curname=fields[1];
			curs.push({id:curid, cur:curname});
		}
		this.updated_curs(curs);
	}
//this=account_window

	this.load_currencies=function() {
		if (this.curs.length!=0) {
			return;
		}
		var o=this;
		$.ajax({
		  url: endpoint+"/?list_currencies",
		  dataType: "text"
		}).done(function(data) { o.parselo(data); });
	}

	this.login_success=function (){
		document.cookie="a="+this.uid;
		document.cookie="b="+this.pwd;
		this.loggedin=true;
		this.walletsvisible=true;
		this.refresh();
		tw.refresh_tradeslip();
		tw.repaint_aw();

	}

	this.set=function(uid,pwd) {
		this.uid=uid;
		this.pwd=pwd;
	}
	this.hide_wallets=function() {
		$("#ww").css('display','none');
	}
	this.show_wallets=function() {
		$("#ww").css('display','block');
	}
//this=account_window
	this.login=function() {
		this.set($("#uid").val(),$("#pwd").val());
		tw.repaint_aw();
		this.ww.load(function () {aw.login_success();} );
	}
	this.logout=function() {
		this.loggedin=false;
		tw.repaint_aw();
		tw.clean();
		
	}


	this.get_an_uid=function() {
		if (this.loggedin) {
			return this.uid;
		}
		else {
			var v=get_cookie("a");
			if (typeof(v) == 'undefined') return "";
			return v;
		}
	}
	this.get_a_pwd=function() {
		if (this.loggedin) {
			return this.pwd;
		}
		else {
			var v=get_cookie("b");
			if (typeof(v) == 'undefined') return "";
			return v;
		}
	}
//this=account_window

	this.toggle=function() {
		this.walletsvisible=!this.walletsvisible;
		this.refresh();
		tw.repaint_aw();
	}

	this.repaint_wallets=function() {
		this.ww.repaint($("#ww"));
	}

	this.repaint_deposit_window=function() {
		this.dw.repaint($("#dw"));
	}

	this.repaint=function(parent) {
		parent.html(this.html());
		if (this.loggedin) {
			$("#uid").prop('disabled', true);
			$("#pwd").prop('disabled', true);
			if (this.walletsvisible) this.show_wallets(); else this.hide_wallets();
		}
		else {
			$("#uid").prop('disabled', false);
			$("#pwd").prop('disabled', false);
			this.hide_wallets();
		}
		
		this.repaint_wallets();
		//this.repaint_deposit_window();
	}
//this=account_window
	this.refresh_data=function (data) {
		if (this.walletsvisible) {
			this.ww.refresh_data(data);
			tw.repaint_aw();
		}
	}

	this.refresh=function () {
		this.load_currencies();

		if (this.walletsvisible) {

			var o=this;
			this.ww.refresh(function() { tw.repaint_aw(); });
		}
	}
//this=account_window

	this.html=function() {
		if (!this.loggedin) {
			var panelmsg="Enter your user id and password in the text boxes provided and then press 'login'. If you don't have an account yet just register for a new one, it is quick, easy and free.";
		}
		else {
			var panelmsg="";
		}
		var ans='<div class="acwnd" title="'+panelmsg+'">';
		ans+='<div class="panelhdr">account</div>';
		ans+='<div class="loginctrls""><span class="accountlbl">id</span> <input type="text" id="uid" class="accountctrl" value="'+this.get_an_uid()+'" title="Enter your user id here."/> <input type="password" id="pwd" class="accountctrl" value="'+this.get_a_pwd()+'" title="Enter your password here."/>';
		if (this.loggedin) {
			ans+='<input type="button" value="logout" onclick="aw.logout()"/>';
		}
		else {
			ans+='<input id="lgnb" type="button" value="login" onclick="aw.login()"/>';
			ans+='<div style="display:inline-block; float:right;"><input type="button" value="new account" onclick="aw.rw.show()"/></div>';
		}

		ans+='</div>';
		if (this.loggedin) {
			if (this.walletsvisible) {
				var offset=1922;
				var lblbtn="hide wallets";
			}
			else {
				var offset=1912;
				var lblbtn="show wallets";
			}
			ans+='<div class="walletstoggle"><button onclick="aw.toggle()" value="" style="height: 18px; width:26px;" title="'+lblbtn+'"><div style="background: transparent url(sprite.png) no-repeat 1px -'+offset+'px;width: 12px;height: 7px;"></div></button></div>';
		}
		ans+='<div style="position:relative; margin-top:6px;">';
		if (this.loggedin) {
		ans+='<hr>';
		ans+='<div class="panelhdrw" onclick="aw.toggle()" title="Show/hide wallets">wallets</div>';
		}
		ans+='<div class="ws" id="ww">';
		//ans+=this.ww.html();

		ans+='</div>';
		
		ans+='</div>';
		ans+='</div>';
		return ans;
	}
}

