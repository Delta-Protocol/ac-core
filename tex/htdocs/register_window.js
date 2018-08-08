function register_window() {
/*
	this.parselo=function(data) {
		var lines = data.split('\n');
		var length = lines.length;
		this.curs= new Array;
		for(var i = 0; i < length; i++){
		        var fields=lines[i].split(" ");
			if (fields.length!=2) {
				continue;
			}
			var curid=fields[0];
			var curname=fields[1];
			this.curs.push({id:curid, cur:curname});
		}
		tw.show_deposit_window();
	}
*/
	this.focus=function() {
		$('#newpwd').focus();
		$('#newpwd').select();
	}

	this.handle_error=function (data) { //true means error, operation must be aborted
		if (data.startsWith("Error: E")) {
			var v1=data.substring(data.indexOf(' ')+1)
			var i=v1.indexOf(' ');
			var err=v1.substring(0,i);
			var msg=v1.substring(i+1)
			if (err=="E2;") {  //PASSWORD IS TOO SHORT
				alert(msg);
				this.focus();
			}
			else {
				alert(data);
			}
			return true;
		}
		return false;
	}
//this=register_window

	this.parse_answer=function (data){
		if (this.handle_error(data)) return; 
		alert("Registration of new account has ben completed successfully, your unique id is "+data.trim());
		$("#uid").val(data.trim()),$("#pwd").val(this.get_pwd())
		aw.refresh();
		aw.focus_login_button();
		this.hide()

	}
	this.do_register= function (){
		var o=this;
		$.ajax({
		  url: endpoint+"/?new_account "+o.get_pwd(),
		  dataType: "text"
		}).done(function(data) { o.parse_answer(data); });
	}

	this.get_pwd=function() {
		return $("#newpwd").val();
	}

	this.show=function() {
		$("#dw").html(this.html());
		$("#dw").css('display','block');
		$('#newpwd').focus();
		$('#newpwd').select();
	}
	this.hide=function() {
		$("#dw").css('display','none');
	}
//this=register_window

	this.html=function() {
		var ans="A brand new and unique user id will be asigned to you, give it a password!.<br><br>";
		ans+='new password <input type="text" id="newpwd" value=""><br>';
		ans+='<input type="submit" value="Submit" onclick="aw.rw.do_register()">';
		ans+='<input type="button" value="Cancel" onclick="aw.rw.hide()">';
		return ans;
	}

}

