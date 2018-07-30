function deposit_window() {
	this.curid=0;
	this.withdraw=false;

	this.focus=function() {
		$('#depositamount').focus();
		$('#depositamount').select();
	}
	this.focus2=function() {
		$('#depositcurid').focus();
		$('#depositcurid').slideDown();
	}


	this.get_cur_input=function() {
		var code='<select id="depositcurid">';

		if (aw.curs.length==0) {
			code+='<option selected>loading...</option>';
		}
		else {
			var o=this;
			code+='<option value="0" selected>select...</option>';
			aw.curs.forEach(function(r) {
				var selected="";
				if (o.curid==r.id) selected=" selected";
				code+='<option value='+r.id+' '+selected+'>';
				code+=r.cur+"</option>"
			});
		}
		code+="</select>";
		return code;
	}
//this=deposit_window
	this.get_amount=function() {
		return $("#depositamount").val();
	}
	this.get_curid=function() {
		return $("#depositcurid").val();
	}

	this.verb=function() {
		if (this.withdraw) return "withdrawing";
		else return "depositing";
	}
	this.handle_error=function (data) { //true means error, operation must be aborted
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			if (err=="E3;") {  //accounts must be funded with at least 10 units
				alert(msg);
				this.focus_deposit_window();
			}
			else if (err=="E4;") {  //unrecognized currency code
				alert("PLease select the currency you wish for "+this.verb()+".");
				this.focus_deposit_window2();
			}
			else if (err=="E11;") {  //not enough funds
				alert(msg);
				return false;  //continue processing
			}
			else if (err=="E301;") {  //no funds
				alert(msg);
				return false;  //continue processing
			}
			else {
				alert(data);
			}
			return true;
		}
		return false;
	}
	this.handle_errors=function (data) { //true means error, operation must be aborted
//alert(data);
		var lines = data.split('\n');
		var length = lines.length;
		var fail=false;
		for(var i = 0; i < length; i++){
			if (this.handle_error(lines[i])) fail=true;
		}
		return fail;
	}

	this.parse_answer=function (data) {
		if (this.handle_errors(data)) return; 
		//alert(data);
		aw.hide_deposit_window();
		aw.refresh();
	}

	this.get_api=function() {
		if (this.withdraw) return "withdraw";
		else return "deposit";
	}
	this.get_verb1=function() {
		if (this.withdraw) return "Withdraw";
		else return "Deposit";
	}

	this.commit=function() {
		var amount=this.get_amount();
		var curid=this.get_curid();
//		alert("deposit "+amount+" "+curid);
	
		var o=this;
		$.ajax({
		  url: endpoint+"/?"+o.get_api()+" "+aw.uid+" "+aw.pwd+" "+curid+" "+amount,
		  dataType: "text"
		}).done(function(data) { o.parse_answer(data); });
	}

	this.repaint=function(parent) {
		parent.html(this.html());
	}
//this=deposit_window

	this.html=function() {
		var ans="";
		ans+=this.get_verb1()+' amount <input type="text" id="depositamount" class="amount" value="0">';
		ans+=this.get_cur_input();
		ans+="<br/>";
		ans+='<input type="submit" value="Submit" onclick="aw.dw.commit()">';
		ans+='<input type="button" value="Cancel" onclick="aw.hide_deposit_window()">';
		return ans;
	}

}

