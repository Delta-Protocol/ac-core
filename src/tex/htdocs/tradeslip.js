function tradeslip(parent) {
	this.parentid=parent;
	this.nid="nt";
	this.n=new orders("tickets");
	this.uid="ut";
	this.u=new orders("orders");
	this.mid="mt";
	this.m=new matched_trades();

	this.handle_error_status=function (data) { //true means error, operation must be 
//alert(data);
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			if (err=="E18;") {  //unable to locate order
				//alert(err+" "+msg);
				alert(msg);
			}
			else if (err=="E101;") {  //wallet not found
				//alert(err+" "+msg);
				alert(msg);
			}
			else if (err=="E301;") {  //wallet without funds
				//alert(err+" "+msg);
				alert(msg);
			}
			else if (err=="E11;") {  //wallet without enough funds
				//alert(err+" "+msg);
				alert(msg);
				return false;
			}
			else {
				alert(data);
			}
			return true;
		}
		return false;
	}

	this.parse_status=function(data) { //U <order id> <amount paid> <currency> <rate> <rate units>
//alert(data);
//old -> U 9959 70.4656 GBP 1.4600 EUR/GBP
//U 9969 S 5.0000 EUR 1.4600 EUR/GBP

//M 5682 68.4931 GBP 1.4600 EUR/GBP 100.0000 EUR 0.0000 EUR
//alert(data);
		var lines = data.split('\n');
		var length = lines.length-1;
		var wallets="";
		var errors=false;
		var numU=0;
		var numM=0;
		for(var i = 0; i < length; i++){
			if (this.handle_error_status(lines[i])) {
				errors=true;
				return;
			}
		        var fields=lines[i].split(" ");
			if (fields.length<1) {
				return;
			}
			var type=fields[0];
			if (type=="U") {
				if (fields.length!=7) {
					return;
				}
				numU++;
			}
			else if (type=="M") {
				if (fields.length!=11) {
					return;
				}
				numM++;
			}
			else { //wallet
				if (fields.length!=3) { //curid cur amount
					return;
				}
			}
		}
		var otd= new Array(numU);
		var mtd= new Array(numM);
		var clean_orders_exclude=new Array(numU);
		var iU=0;
		var iM=0;
		for(var i = 0; i < length; i++){
		        var fields=lines[i].split(" ");
			var type=fields[0];
			if (type=="U") {
				var orderid=fields[1];
				if (fields[2]=="B") var buy=true;
				else if (fields[2]=="S") var buy=false;
				else return;

				var amount=fields[3];
				var cur=fields[4];
				var rate=fields[5];
				var units=fields[6];
				var fieldsunits=units.split("/");
				if (fieldsunits.length!=2) {
					alert("error! h78661324")
					return;
				}
				var weakcur=fieldsunits[0];
				var strongcur=fieldsunits[1];
				if (cur==weakcur)
					var weak=true;
				else
					var weak=false;
				var dstcur=fieldsunits[0];
				otd[iU]={id:orderid,buy:buy,weak:weak,am:amount,rt:rate,weakcur:weakcur,strongcur:strongcur};
				clean_orders_exclude[iU]=orderid;
				iU++;
			}
			else if (type=="M") {
				var tradeid=fields[1];
				var type=fields[2];
				var amount_traded=fields[3];
				var cur_traded=fields[4];
				var rate=fields[5];
				var units=fields[6];
				var fieldsunits=units.split("/");
				if (fieldsunits.length!=2) {
					alert("error! h78661325")
					return;
				}
				var weakcur=fieldsunits[0];
				var strongcur=fieldsunits[1];
				var amount_outcome=fields[7];
				var cur_outcome=fields[8];
				var commission_paid=fields[9];
				var cur_commission=fields[10];
				mtd[iM]={id:tradeid,
					 type:type,
					  amount_traded:amount_traded,
					  cur_traded:cur_traded,
					  rate:rate,
					  weakcur:weakcur,
					  strongcur:strongcur,
					  amount_outcome:amount_outcome,
					  cur_outcome:cur_outcome,
					  commission_paid:commission_paid,
					  cur_commission:cur_commission};
				iM++;
			}
			else { //wallet
				wallets+=lines[i]+"\n";
			}
		}
//alert("w="+wallets);
		tw.clean_orders(clean_orders_exclude);
		tw.update_orders_and_trades(otd,mtd);
		if (errors) {
			this.n.resetflag_submited_tickets();
//			this.n.repaint($('#'+this.nid));
//			this.full_repaint_orders_and_trades($("#ts")); //could bring new orders
		}
		else {
			this.n.cancel_submited_tickets();
//			this.full_repaint($("#ts")); //could bring new orders
		}
//		this.m.repaint($('#'+this.mid));
		this.repaint();
		setTimeout(function(){puo.set();},1000);
		aw.refresh_data(wallets);
		tw.update_liquidity(false);
	}

	this.refresh=function() {
		if (!aw.loggedin) return;
		var o=this;
		$.ajax({
		  url: endpoint+"/?status "+aw.uid+" "+aw.pwd,
		  dataType: "text"
		}).done(function(data) {o.parse_status(data); });
	}

	this.clean=function() {
		this.u.clean();
		this.m.clean();
		
	}


	this.valid_rates_available=function (cur,invcur,valid_rates) {
		var b1=this.n.valid_rates_available(cur,invcur,valid_rates);
		var b2=this.u.valid_rates_available(cur,invcur,valid_rates);
		return b1 || b2;
	}

	this.new_trade=function (buy, weak, weakcur,strongcur,rate,valid_rates) {
//alert("tradeslip "+rate);

		this.n.add_ticket(buy,weak,weakcur,strongcur,rate,valid_rates);
	}
//(entry.id,entry.buy,entry.weak,entry.am,entry.rt,entry.weakcur,entry.strongcur,o.rates[key]);
	this.update_order=function (orderid,buy,weak,amount,rate,weakcur,strongcur,valid_rates) {
		this.u.update_order(orderid,buy,weak,amount,rate,weakcur,strongcur,valid_rates);

	}
//this=tradeslip
	this.clean_orders=function (clean_orders_exclude) {
		this.u.clean_orders(clean_orders_exclude);
	}

	this.reset_orders=function() {
		this.u.reset_orders();
		this.u.repaint($('#'+this.uid));
	}

	this.update_trades=function (trades) {
		this.m.update(trades);
	}


	this.cancel_ticket=function (id) {
		this.n.cancel_ticket(id);
		this.n.repaint($('#'+this.nid));

	}
//this=tradeslip
	this.cancel_order=function (id) {
		this.u.cancel_order(id);
		this.repaint_orders();
	}
	this.toggle_cancel_all_orders=function () {
		this.u.toggle_cancel_all_orders();
		this.repaint_orders();
	}

	this.cancel_all_tickets=function () {
		this.n.cancel_all();
		this.n.repaint($('#'+this.nid));
	}
	this.cancel_all_orders=function () {
		var o=this;
		this.u.cancel_all();
		this.u.repaint($('#'+this.uid));
		
	}

	this.get_order=function(id) {
		var r=this.n.get_order(id);
		if (typeof r=='undefined') {
			return this.u.get_order(id);
		}
		return r;		

	}
	this.submit=function(type) {
		if (!aw.loggedin) {
			alert("Please login to your account or register and fund a new one before ordering a trade.");
			return;
			
		}
		if (type=="tickets") {
			var args=this.n.get_trade_line();
			this.submit_new_trade(args);
		}
		else { //update orders
			var args=this.u.get_update_line();
			this.reset_orders();
			this.submit_update_multiple(args);
		}
	}

	this.submit_update_multiple=function (args) {
		puo.hide2();
		var o=this;
		$.ajax({
		  url: endpoint+"/?update "+aw.uid+" "+aw.pwd+" "+args,
		  dataType: "text"
		}).done(function(data) { o.parse_status(data); });
	}

	this.submit_new_trade=function (args) {
		puo.hide2();
		var o=this;
		$.ajax({
		  url: endpoint+"/?trade "+aw.uid+" "+aw.pwd+" "+args,
		  dataType: "text"
		}).done(function(data) { o.parse_status(data); });
	}

//this=tradeslip
	this.repaint_u=function() {
		this.u.repaint($('#'+this.uid));
	}

	this.editing=function() {
		return this.u.editing();
	}

/*
	this.full_repaint_orders_and_trades=function(parent) {
		parent.html(this.html());
		this.u.full_repaint($('#'+this.uid));
		this.m.repaint($('#'+this.mid));
	}
*/
	this.full_repaint=function(parent) {
		parent.html(this.html());
		this.n.full_repaint($('#'+this.nid));
		this.u.full_repaint($('#'+this.uid));
		this.m.repaint($('#'+this.mid));

	}

	this.repaint_orders=function() {
		this.n.repaint($('#'+this.nid));
		this.u.repaint($('#'+this.uid));
	}

	this.repaint=function() {
		this.n.repaint($('#'+this.nid));
		this.u.repaint($('#'+this.uid));
		this.m.repaint($('#'+this.mid));
	}
//this=tradeslip

	this.html=function() {
		var ans='<div>';
		ans+='<div class="ts_title">Trade slip</div>';
		ans+='<div id="'+this.nid+'">';
//		ans+=this.n.html();
		ans+='</div>';

		ans+='<div id="'+this.uid+'">';
//		ans+=this.u.html();
		ans+='</div>';

		ans+='<div id="'+this.mid+'">';
//		ans+=this.m.html();
		ans+='</div>';

		ans+='</div>';
		ans+='</div>';
		return ans;
	}
}

