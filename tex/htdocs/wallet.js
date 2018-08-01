function wallet(curid, cur, available) {
	this.curid=curid;
	this.cur=cur;
	this.available=parseFloat(available); //available;


	this.html=function() {
		var ans='';
		ans+='<div>';
		ans+='<div title="'+this.cur+' '+this.available+'" class="wlt">'+this.cur+" "+Math.round(this.available).toLocaleString()+"</div>";
		ans+='<input type="button" value="deposit '+this.cur+'" onclick="aw.show_deposit_window('+this.curid+')"/>';
		if (this.available>0) {
			ans+='<input type="button" value="withdraw '+this.cur+'" onclick="aw.show_withdraw_window('+this.curid+')"/>';
		}
		else {
			ans+='<input type="button" value="delete wallet" label="delete this wallet" onclick="aw.ww.delete('+this.curid+')"/>';
		}
		ans+="</div>";
		return ans;
	}
}


