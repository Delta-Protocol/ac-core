function wallets_window() {
	this.items=new Array();

	this.handle_error_delete_wallet=function (data) { //true means error, operation must be aborted
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			alert(data);
			return true;
		}
		return false;
	}
//this=wallets_window

	this.parse_answer_delete_wallet=function(data) {
		if (this.handle_error_delete_wallet(data)) return; 
		aw.refresh();
	}

	this.delete=function(curid) {
		var o=this;
		$.ajax({
		  url: endpoint+"/?delete_wallet "+aw.uid+" "+aw.pwd+" "+curid,
		  dataType: "text"
		}).done(function(data) { o.parse_answer_delete_wallet(data); });
	}

	this.handle_error_new_wallet=function (data) { //true means error, operation must be aborted
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			alert(data);
			return true;
		}
		return false;
	}

	this.parse_answer_new_wallet=function(data) {
		if (this.handle_error_new_wallet(data)) return; 
		aw.refresh();
	}

	this.create=function() {
		var curid=$("#walletscurid").val();
		var o=this;
		$.ajax({
		  url: endpoint+"/?new_wallet "+aw.uid+" "+aw.pwd+" "+curid,
		  dataType: "text"
		}).done(function(data) { o.parse_answer_new_wallet(data); });
	}
//this=wallets_window

	this.find=function(curid) {
		var o=this;
		var found=this.items.some(function(w) {
			if (w.curid==curid) return true;
			return false;
		});
		return found;
	}

	this.handle_error_listwallets=function (data) { //true means error, operation must be aborted
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			if (err=="E1;") {  //incorrect uid or pwd
				alert(msg);
				return true; 
			}
			else {
				alert(data);
			}
			return true;
		}
		return false;
	}
//this=wallets_window

	this.parse_listwallets_answer= function (handler,data){
	//1 EUR 100000.0000
		if (this.handle_error_listwallets(data)) return; 

		var lines = data.split('\n');
		var length = lines.length-1;
		this.items=new Array(length);
		for(var i = 0; i < length; i++){
			var line=lines[i].trim();
			if (line=="") {
				return;
			}
		        var fields=line.split(" ");
			if (fields.length<3) {
				return;
			}
			var curid=fields[0];
			var cur=fields[1];
			var available=fields[2];
//alert(cur+" "+available);
			this.items[i]=new wallet(curid,cur,available);
		}
		handler();
	}
//this=wallets_window
	this.load=function(handler) {
		if (typeof handler == "undefined") alert("n68557462");
		var o=this;
		$.ajax({
		  url: endpoint+"/?list_account "+aw.uid+" "+aw.pwd,
		  dataType: "text"
		}).done(function(data) { o.parse_listwallets_answer(handler,data); });

	}
	this.refresh=function (handler) {
		if (typeof handler == "undefined") alert("n68557465");
		this.load(handler);
	}
	this.refresh_data=function (data) {
		this.parse_listwallets_answer(function(){},data);
	}

	this.get_pending_currencies=function() {
		var pc=new Array();
		var o=this;
		aw.curs.forEach(function(r) {
			if (o.find(r.id)) return;
			pc.push(r);
		});
		return pc;				
	}

	this.get_cur_input=function(pending_currencies) {
		var code='<select id="walletscurid">';

		var o=this;
		pending_currencies.forEach(function(r) {
			code+='<option value='+r.id+'>';
			code+=r.cur+"</option>"
		});

		code+="</select>";
		return code;
	}
//this=wallets_window
	this.repaint=function(parent) {
		parent.html(this.html());
	}

	this.html=function() {
		var ans='';
		ans+='<div style="position:relative; margin-top:6px;">';
//		ans+='<div class="arrowup" style="left:70px; top:-5px;"></div>';
		ans+='<div>';
		var o=this;
		this.items.forEach(function(w) {
			ans+=w.html();
		});
		ans+="</div>";
		var pc=this.get_pending_currencies();
		if (pc.length>0) {
			ans+="<hr>";
			ans+='<div style="text-align:center; width:100%;">set up a new wallet '+this.get_cur_input(pc);
			ans+='<input type="button" value="add" onclick="aw.ww.create()"/>';
			ans+="</div>";
		}
		ans+="</div>";

		return ans;
	}

}

