function trading_window() {
	this.ms=new markets("mks");
	this.ts=new tradeslip(this.ts);

	this.load=function () {
		this.ms.load();
	}
	this.load_valid_rates=function () {
		this.ms.load_valid_rates();

	}

	this.rates=new Array;

	this.clean=function() {
		this.ts.clean();
		this.repaint_ts();
	}

	this.valid_rates_available=function (scur,dcur,valid_rates) {
		var key=scur+"_"+dcur;
		this.rates[key]=valid_rates;
		this.ts.valid_rates_available(scur,dcur,this.rates[key]);
//			this.repaint_ts();
			
	}
//trading_window
	this.parselo_valid_rates=function(scur, dcur, data) {
		var lines = data.split('\n');
		var length = lines.length-1;
		var valid_rates_= new Array(length);
		for(var i = 0; i < length; i++){
		        //var fields=lines[i].split(" ");
			//if (fields.length!=1) {
			//	return;
			//}
//			var v=fields[0].trim();
			
			//if (v=="") 
			//	return;
			valid_rates_[i]=lines[i];
		}
		this.valid_rates_available(scur,dcur,valid_rates_);
	}

	this.new_trade=function (buy, weak, weakcur, strongcur, rate) {
		var key=weakcur+"_"+strongcur;
//alert("new trade "+key+" "+rate);
		if (typeof this.rates[key] == 'undefined') {
			this.rates[key]=new Array;
		}
//alert(scur+" "+dcur+" "+this.rates[{c:scur, i:dcur}]);
//alert("trading window "+scur+" "+dcur+" "+this.rates[{c:dcur, i:scur}]);
//alert(weakcur+" "+strongcur+" "+rate+" "+this.rates[key]);
		this.ts.new_trade(buy, weak, weakcur,strongcur,rate,this.rates[key]);
		this.repaint_ts();
		this.ts.n.focus();

		if (this.rates[key].length==0) {
			var o=this;
			var sc=weakcur;
			var dc=strongcur;
			$.ajax({
			  url: endpoint+"/?valid_rates_simple "+curid[sc]+" "+curid[dc],
			  dataType: "text"
			}).done(function(data){o.parselo_valid_rates(sc,dc,data);});
		}
	}

//				otu.push({id:orderid,am:amount,src:cur,rt:rate,dst:dstcur});
//trading_window
	this.update_orders_and_trades=function (ords, trds) {
		var o=this;
		var a=new Array();
//{id:orderid,buy:buy,weak:weak,am:amount,rt:rate,weakcur:weakcur,strongcur:strongcur}
//{id:orderid,buy:false,weak:weak,am:amount,rt:rate,weakcur:weakcur,strongcur:strongcur}
		ords.forEach(function(entry) { 
//alert(entry);
			var key=entry.weakcur+"_"+entry.strongcur
			if (typeof o.rates[key] == 'undefined') {
				o.rates[key]=new Array;
				a.push({c:entry.weakcur, i:entry.strongcur});
			}
			o.ts.update_order(entry.id,entry.buy,entry.weak,entry.am,entry.rt,entry.weakcur,entry.strongcur,o.rates[key]);
		});

		this.ts.update_trades(trds);
		this.repaint_ts();
		puo.set();


		a.forEach(function(entry) {
			var c=entry.c;
			var ic=entry.i;
			$.ajax({
			  url: endpoint+"/?valid_rates_simple "+curid[c]+" "+curid[ic],
			  dataType: "text"
			}).done(function(data){o.parselo_valid_rates(c,ic,data);});
		});

	}


	this.update_order=function (orderid,amount,cur,rate,dstcur) {
		var key=cur+"_"+dstcur;
		if (typeof this.rates[key] == 'undefined') {
			this.rates[key]=new Array;
		}
		this.ts.update_order(orderid,amount,cur,rate,dstcur,this.rates[key]);
		if (this.rates[key].length==0) {
			var o=this;
			var c=cur;
			var ic=dstcur;
			$.ajax({
			  url: endpoint+"/?valid_rates_simple "+curid[c]+" "+curid[ic],
			  dataType: "text"
			}).done(function(data){o.parselo_valid_rates(c,ic,data);});
		}
	}

	this.clean_orders=function (clean_orders_exclude) {
//alert("1");
		this.ts.clean_orders(clean_orders_exclude);
	}
//this=trading_window
	this.handle_error_update_order=function (data) { //true means error, operation must be 
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			if (err=="E18;") {  //unable to locate order
				//alert(err+" "+msg);
				alert(msg);
			}
			else {
				alert(data);
			}
			return true;
		}
		return false;
	}
	this.parse_answer_update_order=function (data) {
		if (this.handle_error_update_order(data)) {
			this.repaint_ts(); //something went wrong, UI must be updated
			return; 
		}
		this.repaint_ts(); //could bring new orders
		aw.refresh();
	}
/*
	this.parse_answer_cancel_all_orders=function (data) {
		if (this.handle_error_update_order(data)) {
			this.repaint_ts(); //something went wrong, UI must be updated
			return; 
		}
		this.ts.cancel_all_orders();
		aw.refresh();
		this.msg("All orders has been cancelled.");
	}
*/
//this=trading_window

	this.submit_new_trade=function (ticketid,curid,amount_id,rate_id,dstcur) {
		this.ts.submit_new_trade(ticketid,curid,amount_id,rate_id,dstcur);
	}
	this.cancel_ticket=function (id) {
		puo.hide2();
		this.ts.cancel_ticket(id);
	}

	this.cancel_order=function (id) {
		this.ts.cancel_order(id);
/*
		//?update_order <account id> <password> <order id> <new amount>
		var o=this;
		$.ajax({
		  url: endpoint+"/?update_order "+aw.uid+" "+aw.pwd+" "+id+" 0",
		  dataType: "text"
		}).done(function(data) { 
			o.ts.cancel_order(id);
			o.parse_answer_update_order(data); 
		});
*/
	}


	this.cancel_all_tickets=function () {
		this.ts.cancel_all_tickets();
	}
	this.toggle_cancel_all_orders=function () { //cancel_all_orders <account id> <password>
		this.ts.toggle_cancel_all_orders();
/*
		var o=this;
		$.ajax({
		  url: endpoint+"/?cancel_all_orders "+aw.uid+" "+aw.pwd,
		  dataType: "text"
		}).done(function(data) { 
			o.parse_answer_cancel_all_orders(data); 
		});
*/
	}
//this=trading_window

	this.refresh_tradeslip=function() {
		this.ts.refresh();
	}

	this.repaint_ts=function () {
		this.ts.repaint();
	}

	this.full_repaint=function (parent) {
		parent.html(this.html());
		this.ts.full_repaint($("#ts"));
		var o=this;
		setTimeout(function() { 
			rsz();
			$("#content").scrollTop(100);
			o.slide_all_to_front();
			
//$("#mkt_AUD_USD").scrollLeft(100);
		},1000);
		
	}
	this.repaint_aw=function() {
		aw.repaint($("#ac"));
	}

	this.repaint=function() {
		//parent.html(this.html());
		this.repaint_ts();
	}

	this.slide_to_front=function (mktid) {
		this.ms.slide_to_front(mktid);
	}
	this.slide_all_to_front=function () {
		this.ms.slide_all_to_front();
	}

	this.update_liquidity=function() {
		this.ms.update();
	}

	this.update_liquidity_loop=function() {
		this.ms.update();
		this.refresh_tradeslip();
		this.slide_all_to_front();
		
		var o=this;
		setTimeout(function() { o.update_liquidity_loop(); },10000);
	}

	this.init=function() {
		setTimeout(this.load(),500);
		aw.repaint($("#ac"));
		this.full_repaint($("#content"));
		setTimeout(function() { tw.update_liquidity_loop(); },5000);
	}

	this.html=function() {
		var ans="";

		ans+='<div id="tw">';
		ans+='<div class="mks" id="mks">';
		ans+='<div class="msg">loading markets...</div>';
		ans+='</div>';

		ans+='</div>';
		return ans;
	}

	this.msg=function(m) {
		$("#msgwindow").html(m);
		$("#msgwindow").css('display','block');
		setTimeout(function() {
			$("#msgwindow").css('display','none');

		},3000);
	}
}

