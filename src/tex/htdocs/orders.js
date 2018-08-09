function orders(thetype) {
	this.type=thetype; //"tickets" "orders"
	this.items=new Array();
	this.ticketid=0;

	this.focus=function() {
		if (this.items.length==0) return;
		$('#'+this.items[0].amount_txt_id()).focus();
		$('#'+this.items[0].amount_txt_id()).select();
	}

	this.get_order=function(id) {
		var o=this.items;
		var t;
//alert("A "+id);
		this.items.some(function(r) {
			if (r.is_me2(id)) {
				t=r;
				return true;
			}
			return false;
		});
		return t;
	}
	this.get_trade_line=function() {
		var s="";
		this.items.forEach(function(r) {
			s+=" "+r.get_trade_line();
		});
		return s.trim();
	}
	this.get_update_line=function() {
		var s="";
		this.items.forEach(function(r) {
			var l=r.get_update_line();
			if (l=="") return;
			s+=" "+l;
		});
		return s.trim();
	}


	this.add_ticket=function(buy,weak,weakcur,strongcur,rate,valid_rates) {
//alert("orders "+rate+" "+valid_rates);
		parent=$('#'+this.type+'_cnt');
		this.ticketid++;
		var t=new order(this.ticketid,0,buy,weak,0,rate,weakcur,strongcur,valid_rates);
		this.items.unshift(t);
		var html='<div class="otid" id="'+t.get_id()+'">'+this.items[0].html()+'</div>';
		if (this.items.length==1) {
			parent.html(html);
			puo.set();

		}
		else {
			$(html).insertBefore($('#'+this.items[1].get_id()));
		}
//alert(parent.parent().html());
		this.showhide_buttons();

	}
	this.update_order=function(id,buy,weak,amount,rate,weakcur,strongcur,valid_rates) {
		var o=this.items;
		var found=false;
		this.items.some(function(r) {
			if (r.is_me(id)) {
				found=true;
				r.update(buy,weak,amount,rate,weakcur,strongcur,valid_rates);
				return true;
			}
			return false;
		});
		if (!found) {
//ticketid,orderid,buy,weak,theamount,therate,weakcur,strongcur,rates
			var t=new order(0,id,buy,weak,amount,rate,weakcur,strongcur,valid_rates);
			this.items.unshift(t);
			var html='<div class="otid" id="'+t.get_id()+'">'+this.items[0].html()+'</div>';
			parent=$('#'+this.type+'_cnt');
			if (this.items.length==1) {
				parent.html(html);
			}
			else {
				$(html).insertBefore($('#'+this.items[1].get_id()));
			}
		}
	}
	this.exists=function (item,list) {
                return list.some(function(xid) {
                        if (item.is_me(xid)) return true;
                        return false;
                });
	}
	this.clean=function () {
		this.items=new Array();
		$('#'+this.type+'_cnt').html("");
	}

	this.clean_orders=function (clean_orders_exclude) {
		var self=this;
		var o=this.items;
		var more=true;
		while(more) {
			more=false;
			o.some(function(r) { //break is true
				if (!self.exists(r,clean_orders_exclude)) {
					$('#'+r.get_id()).remove();
					var index = o.indexOf(r);
					o.splice(index, 1);
					more=true;
					return true;
				}
				return false;
			});
		}		
	}
	this.cancel_ticket=function (id) {
		var o=this.items;
		this.items.some(function(r) {
			if (r.ticketid==id) {
				var index = o.indexOf(r);
				o.splice(index, 1);
				$('#'+r.get_id()).remove();
				return true;
			}
			return false;
		});
	}
	this.cancel_order=function (id) {
		var o=this.items;
		var sid="order_"+id;
		this.items.some(function(r) {
			if (r.orderid==sid) {
				r.tocancel=!r.tocancel;
//				var index = o.indexOf(r);
//				o.splice(index, 1);
				return true;
			}
			return false;
		});
	}

	this.reset_orders=function() {
		this.items.forEach(function(r) {
			r.reset();
		});
	}

	this.editing=function() {
		if (this.type=="tickets") return false;
		var o=this.items;
		var ret=false;
		this.items.some(function(r) {
			if (r.editing()) {
				ret=true;
				return true;
			}
			return false;
		});
		return ret;
	}

	this.cancel_all=function (handler) {
		this.items=new Array();
		$('#'+this.type+'_cnt').html("");
	}
	this.resetflag_submited_tickets=function () {
		var o=this.items;
		this.items.forEach(function(r) {
			r.submitted=false;
		});
	}

	this.cancel_submited_tickets=function () {
		var o=this.items;
		while(
		this.items.some(function(r) {
			if (r.submitted) {
				var index = o.indexOf(r);
				o.splice(index, 1);
				$('#'+r.get_id()).remove();
				return true;
			}
			return false;
		})
		) {
		}
	}
	this.toggle_cancel_all_orders=function () {
		var o=this.items;
		this.items.forEach(function(r) {
			r.tocancel=!r.tocancel;
		});
	}

	this.valid_rates_available=function (cur,invcur,valid_rates) {
		var b=false;
		this.items.forEach(function(r) {
			b|=r.valid_rates_available(cur,invcur,valid_rates);
		});
		return b;
	}
	this.showhide_buttons=function () {
		if (this.type=="tickets") {
			if (this.items.length>0) {
				$('#'+this.type+'_cncallbtn').css('display','inline-block');
				$('#'+this.type+'_submitbtn').css('display','inline-block');
			}
			else {
				$('#'+this.type+'_cncallbtn').css('display','none');
				$('#'+this.type+'_submitbtn').css('display','none');
			}
		}
		else {
			if (this.items.length>0) {
				$('#'+this.type+'_cncallbtn').css('display','inline-block');
			}
			else {
				$('#'+this.type+'_cncallbtn').css('display','none');
			}
			if (this.editing()) {
				$('#rstbtn').css('display','inline-block');
				$('#'+this.type+'_submitbtn').css('display','inline-block');
			}
			else {
				$('#rstbtn').css('display','none');
				$('#'+this.type+'_submitbtn').css('display','none');
			}
		}
	}

	this.full_repaint=function(parent) {
		parent.html(this.html());
//alert(this.html());
//alert("fullk "+$('#'+this.type+'_cncallbtn').html());
		this.repaint(parent);
	}
	this.repaint=function(parent) {
		var o=this;
		this.items.forEach(function(r) {
			if (o.type=="orders") {
				r.repaint($("#"+r.orderid));
			}
			else {
				r.repaint($("#"+r.ticketid));
			}
		});
		this.showhide_buttons();
	}


	this.html=function() {
		if (this.type=="tickets") {
//			if (this.items.length==0) return "";
			var msg="Place new orders";
		}
		else {
			var msg='Open orders'; //<input type="button" onclick="tw.refresh_tradeslip()"/>
		}
		var ans='<div class="trade">';
		ans+='<div id="'+this.type+'_lbl" class="lbl"><span>'+msg+'</span>';
		ans+='</div>';

		ans+='<div id="'+this.type+'_cnt" class="umc">';

		var o=this;
		this.items.forEach(function(r) {
			if (o.type=="orders") 
				ans+='<div class="otid" id="'+r.orderid+'"></div>';
			else
				ans+='<div class="otid" id="'+r.ticketid+'"></div>';
		});

		ans+="</div>";

		if (this.type=="tickets") {
			var cancelfn="tw.cancel_all_tickets()";
		}
		else	{
			var cancelfn="tw.toggle_cancel_all_orders()";
		}

		if (this.type=="tickets") {
			var cancellbl="Remove all";
			var canceltip="Remove all tickets from the list.";
			var submitlbl="Submit";
			var submittip="Submit all new orders for execution.";
		}
		else {
			var cancellbl="Toggle X";
			var canceltip="Toggle cancel state to all orders.";
			var submitlbl="Submit changes";
			var submittip="Send and commit updates made to active orders.";
			var resetlbl="Ignore changes";
			var resettip="Discard all changes made to active orders.";
			var resethandler="tw.ts.reset_orders()";
		}
		var submithandler="tw.ts.submit('"+this.type+"')";

		ans+='<div id="'+this.type+'_buttonstickets" style="width:100%; text-align:center;">';
		ans+='<input class="minwidget"  id="'+this.type+'_submitbtn" style="display: none" type="submit" value="'+submitlbl+'" onclick="'+submithandler+'" title="'+submittip+'"/>';
		ans+='<button class="minwidget" id="'+this.type+'_cncallbtn" style="display: none" onclick="'+cancelfn+'" title="'+canceltip+'"><div>'+cancellbl+'</div></button>';
		if (this.type=="orders") {

			ans+='<button class="minwidget" id="rstbtn" style="display: none" onclick="'+resethandler+'" title="'+resettip+'"><div>'+resetlbl+'</div></button>';
		}

		ans+="</div>";
		ans+="</div>";

		return ans;
	}

}


