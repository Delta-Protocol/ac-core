function order(ticketid,orderid,buy,weak,theamount,therate,weakcur,strongcur,rates) {

	this.ticketid="ticket_"+ticketid;
	this.orderid="order_"+orderid;
	this.nakedorderid=orderid;
	this.buy=buy;
	this.weak=weak;
	this.weakcur=weakcur;
	this.strongcur=strongcur;
	this.amount=theamount;
	this.rate=therate;
	this.valid_rates=rates;
	this.orig_amount=this.amount;
	this.orig_rate=this.rate;
	this.last_input=-1;
	this.tocancel=false;
	this.justnew=true;
//alert(therate +" "+this.valid_rates);
//alert("order "+this.rate);
	this.submitted=false;

	this.is_me=function(id) {
		return this.orderid==("order_"+id);
	}
	this.is_me2=function(id) {
		return this.orderid==id || this.ticketid==id;
	}

	this.is_ticket=function() {
		return (this.ticketid!="ticket_0");
	}

	this.get_id=function() {
		if (this.is_ticket()) return this.ticketid;
		else return this.orderid;
	}

	this.amount_txt_id=function() {
		return this.get_id()+"_amount";
	}
	this.rate_input_id=function() {
		return this.get_id()+"_rate";
	}
//ans+='<spain id="'+this.rate_input_id+'"><option value="" selected>loading...</option></select>';

	this.get_rate_input=function() {
		var id=this.rate_input_id();
		var clickhandler="tw.ts.get_order('"+this.get_id()+"').set_rate_from_select_input();";
		var code='<select id="'+id+'" onchange="'+clickhandler+'">';
//alert("B "+this.valid_rates);
		if (this.valid_rates.length==0) {
			code+='<option selected>'+this.rate+'..</option>';
		}
		else {
            for (i=0; i<this.valid_rates.length; ++i) {
                code+='<option';
                if (this.valid_rates[i]==this.rate) code+=" selected";
                code+=">"+this.valid_rates[i]+"</option>"
            }
		}
		code+="</select>";
		return code;
	}
	this.increase_rate=function() {
		var o=this;
		var prev=o.rate;
		this.last_input=3;
		if (this.valid_rates.some(function(r) {
			if (r==o.rate) {
				return true;
			}
			prev=r;
			return false;
		})) {
			o.rate=prev;
			tw.ts.repaint_orders();
//			this.ts.repaint();
			puo.repaint(this);
		}
	}
	this.decrease_rate=function() {
		var o=this;
		var isnext=false;
		this.last_input=2;

		if (this.valid_rates.some(function(r) {
			if (isnext) {
				o.rate=r;
				return true
			}
			if (r==o.rate) {
				isnext=true;
			}
			return false;
		})) {

			tw.ts.repaint_orders();
//			puo.repaint(this);
		}
	}

	this.set_rate_from_select_input=function () {
		this.rate=$("#"+this.rate_input_id()).val();
		this.last_input=1;
//alert(this.last_input);
		tw.ts.repaint_orders();
////		this.repaint($("#"+this.get_id()));
		puo.repaint(this);
	}
	this.amount_changed=function () {
		this.amount=$("#"+this.amount_txt_id()).val();
		this.last_input=0;
//alert(this.last_input);
		tw.ts.repaint_orders();
////		this.repaint($("#"+this.get_id()));
		puo.repaint(this);
	}

	this.get_spin_controls=function() {
		var ans="";
		ans+=' <div class="abc" style="margin-left:-4px;">';
		var clickhandler="tw.ts.get_order('"+this.get_id()+"').increase_rate();";
		ans+='     <button id="'+this.get_id()+'_rateup" class="arrow-btn" onclick="'+clickhandler+'"><div class="arrow-up"></div></button>';
		clickhandler="tw.ts.get_order('"+this.get_id()+"').decrease_rate();";
		ans+='<div class="button-separator"></div>';
		ans+='     <button id="'+this.get_id()+'_ratedown" class="arrow-btn" onclick="'+clickhandler+'"><div class="arrow-down"></div></button>';
		ans+=' </div>';
		return ans;
	}

	this.valid_rates_available=function(srccur,dstcur,rates) {

//		var scur=this.srccur();
//		var dcur=this.dstcur();
		if(srccur!=weakcur || dstcur!=strongcur) {
			return false;
		}
//alert (srccur+"?"+scur+" "+dstcur+"?"+dcur+ " "+rates);
		this.valid_rates=rates;
		$("#"+this.rate_input_id()).replaceWith(this.get_rate_input());
		return true;
	}

	this.editing=function() {
		if (this.is_ticket()) return true;
		if (this.orig_amount!=this.amount) return true;
		if (this.orig_rate!=this.rate) return true;
		if (this.tocancel) return true;
		return false;
	}
	
	this.update=function(buy,weak,amount,rate,weakcur,strongcur,valid_rates) {
		if (!this.editing()) { //user is editing
                        this.amount=amount;
                        this.rate=rate;
                }
                this.orig_amount=amount;
                this.orig_rate=rate;
 	}

	this.srccur=function() {
		if (this.weak) {
			if (this.buy) {
				return this.strongcur;
			}
			else {
				return this.weakcur;
			}
		}
		else {
			if (this.buy) {
				return this.weakcur;
			}
			else {
				return this.strongcur;
			}
		}
	}
	this.dstcur=function() {
		if (this.weak) {
			if (this.buy) {
				return this.weakcur;
			}
			else {
				return this.strongcur;
			}
		}
		else {
			if (this.buy) {
				return this.strongcur;
			}
			else {
				return this.weakcur;
			}
		}
	}
	this.get_trade_line=function() {
		this.submitted=true;
		if (this.buy) {
			var scur=this.dstcur();
		}
		else {
			var scur=this.srccur();
		}
		var s=this.get_id()+" "+(this.buy?"B":"S")+" "+this.amount+" "+scur+" "+this.rate+' '+this.weakcur+'/'+this.strongcur;
		return s;
	}
	this.get_update_line=function() {
		if (!this.editing()) return "";
		var s=this.nakedorderid+" "+(this.tocancel?0:this.amount)+" "+this.rate;
		return s;
	}
	this.reset=function() {
		this.amount=this.orig_amount;
		this.rate=this.orig_rate;
		this.tocancel=false;
	}

	this.repaint_bits=function() {
		if (this.last_input!=0) $("#"+this.amount_txt_id()).val(this.amount);
		if (this.last_input!=1) $("#"+this.rate_input_id()).replaceWith(this.get_rate_input());
		if (this.tocancel) {
			$('#'+this.get_id()+'_cnlb').css("background-color","red");
		}
		else {
			$('#'+this.get_id()+'_cnlb').css("background-color","");
		}
	}
	this.bgcolor=function() {
		if (this.justnew) return "#ff8";
		var ed=this.editing();
		if (ed) {
			return 'white';
		}
		return '';
/*
		else {
			var foc=$("#"+this.amount_txt_id()).is(":focus");		
			if (foc) {
				return '';
			}
		}
*/

	}

	this.scheduledoff=false;

	this.flash=function(parent) {
		if (this.justnew && !this.scheduledoff) {
			this.scheduledoff=true;
			var o=this;
			setTimeout(function() { o.justnew=false; o.repaint(parent); },500);
			
		}
	}

	this.full_repaint=function(parent) {
		parent.html(this.html());
		$('#canvas_'+this.get_id()).css('background-color',this.bgcolor());
		this.flash(parent);
	}

	this.repaint=function(parent) {
		var ed=this.editing();
		$('#canvas_'+this.get_id()).css('background-color',this.bgcolor());
		if (ed) {
			this.repaint_bits();
		}
		else {
			var foc=$("#"+this.amount_txt_id()).is(":focus");		
			if (foc) {
				this.repaint_bits();
			}
			else {
				parent.html(this.html());
			}
		}
		this.flash(parent);
/*
		if (this.valid_rates.length<2) {
			$('#'+this.get_id()+'_rateup').prop('disabled', true);
			$('#'+this.get_id()+'_ratedown').prop('disabled', true);
		}
		else {
			if (this.rate==this.valid_rates[0]) {
				$('#'+this.get_id()+'_rateup').prop('disabled', true);
				$('#'+this.get_id()+'_ratedown').prop('disabled', false);
			}
			if (this.rate==this.valid_rates[this.valid_rates.length-1]) {
				$('#'+this.get_id()+'_rateup').prop('disabled', false);
				$('#'+this.get_id()+'_ratedown').prop('disabled', true);
			}
		}
*/
	}

	this.html=function() {
		if (this.buy) {
			var scur=this.dstcur();
			var dcur=this.srccur();
		}
		else {
			var scur=this.srccur();
			var dcur=this.dstcur();
		}
		var ans="";

		ans+='<div class="order" id="canvas_'+this.get_id()+'">';
		ans+='<table cellspacing="0" border="0">';
		ans+='<tr>';
		ans+='<td>';
		if (this.is_ticket()) {
			var cancelhandler="tw.cancel_ticket('"+this.ticketid+"')";
			var tit="Remove";
		}
		else {
			var tit="Cancel order "+this.orderid;
			var cancelhandler="tw.cancel_order('"+this.orderid.replace("order_","")+"')";
		}
		
		ans+='<input id="'+this.get_id()+'_cnlb" title="'+tit+'" type="button" value="X" style="font-size:8px; padding:1px;" onclick="'+cancelhandler+'"/>';
		ans+='</td>';
		ans+='<td>';
		if (this.buy) {
			ans+='Buy ';

		}
		else {
			ans+='Sell ';
		}
		var chghandler="tw.ts.get_order('"+this.get_id()+"').amount_changed();";
		var chghandlerinpt="tw.ts.get_order('"+this.get_id()+"').amount_changed();";
		ans+='<input id="'+this.amount_txt_id()+'" type="text" class="amount" value="'+this.amount+'" onchange="'+chghandler+'" oninput="'+chghandlerinpt+'" title="Enter amount, use only digits and . (dot) as the decimal separation character."> '+scur;
		ans+='</td>';
		ans+='<td>';
		ans+='<div class="main" style="display:block">';
		ans+='<div class="abc">@';
		ans+=this.get_rate_input();
//ans+="<br>A<br>A<br>A<br>A<br>A<br>A<br>A<br>A<br>A<br>A";
		ans+="</div>";
		ans+=this.get_spin_controls();
		ans+='<div class="abc">';
		ans+=this.weakcur+'/'+this.strongcur;
		ans+="</div>";
		ans+="</div>";



		ans+='</td>';
//		ans+='<td>';
/*
		if (this.weak)
			if (this.buy)
				var conv=this.amount*parseFloat(this.rate);
			else
				var conv=this.amount/parseFloat(this.rate);
		else
			if (this.buy)
				var conv=this.amount/parseFloat(this.rate);
			else
				var conv=this.amount*parseFloat(this.rate);
	
		ans+='to obtain <div style="width:100%; align: center; font-weight: bold;">'+conv.toFixed(2)+' '+dcur+'</div>';
*/
		ans+='</tr>';
		ans+='</table>';
//alert(ans);
		ans+='</div>';

		return ans;
	}

	this.htmlb=function() {
		if (this.buy) {
			var scur=this.dstcur();
			var dcur=this.srccur();
		}
		else {
			var scur=this.srccur();
			var dcur=this.dstcur();
		}
		var ans="";
		if (this.is_ticket()) {
			if (this.buy) {
				ans+='Buy ';
			}
			else {
				ans+='Sell ';
			}
		}
		else {
			ans+="<div>Order#"+this.orderid+"</div>";
			if (this.buy) {
				ans+='Buying ';
			}
			else {
				ans+='Selling ';
			}
		}
		ans+=parseFloat(this.amount).toLocaleString()+' '+scur;
		var relrt="";
		if (this.weak) {
			if (this.buy) {
				var conv=this.amount/parseFloat(this.rate);
				relrt="maximum";
			}
			else {
				var conv=this.amount/parseFloat(this.rate);
				relrt="minimum";
			}
		}
		else {
			if (this.buy) {
				var conv=this.amount*parseFloat(this.rate);
				relrt="maximum";
			}
			else {
				var conv=this.amount*parseFloat(this.rate);
				relrt="minimum";
			}
		}
		if (this.is_ticket()) {
			var atleast="at least ";
		}
		else {
			var atleast="";
			relrt="";
		}
		if (this.buy) {
			ans+=' for '+conv.toLocaleString()+' '+dcur;
		}
		else {
			ans+=' to obtain '+atleast+conv.toLocaleString()+' '+dcur;
		}
		ans+=', at a '+relrt+' rate of '+this.rate+' '+this.weakcur+'/'+this.strongcur;
	
		return ans;

	}

}

