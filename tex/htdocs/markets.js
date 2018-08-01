
function markets(container) {
	this.parent=container;
	this.m=new Array;

	this.load_valid_rates=function () {
		var o=this;
		setTimeout(function(){
			o.m.forEach(function(r) {
				r.load_valid_rates();
			});
		}, 100);
	}
	this.slide_to_front=function (mktid) {
		this.m.some(function(r) {
			if (r.get_id()==mktid) {
				r.slide_to_front();
				return true;
			}
			return false;
		});
	}

	this.slide_all_to_front=function () {
		this.m.forEach(function(r) {
			r.slide_to_front();
		});
	}

	this.parselo=function(data) {

		var lines = data.split('\n');
		var length = lines.length;
		this.m=new Array;
		for(var i = 0; i < length; i++){
		        var fields=lines[i].split(" ");
			if (fields.length!=2) {
				continue;
			}
		        var curs=fields[0].split("/");
			if (curs.length!=2) {
				continue;
			}
		        var curids=fields[1].split("/");
			if (curids.length!=2) {
				continue;
			}
			var mk=new market(curs[0],curs[1]);
			this.m.push(mk);
			curid[curs[0]]=curids[0];
			curid[curs[1]]=curids[1];
			//alert(curs[0]+" "+curids[0]+" ; "+curs[1]+" "+curids[1]);

		}
		//alert(this.m.length);
		$("#"+this.parent).html(this.html());
		//alert(this.m.length);
		var o=this;
		setTimeout(function(){
			o.m.forEach(function(r) {
				r.load();
			});
		}, 500);
	}
	this.update=function() {
		this.m.forEach(function(r) {
			r.load();
		});
	}

	this.load=function() {
		var o=this;
		$.ajax({
		  url: endpoint+"/?list_markets",
		  dataType: "text"
		}).done(function(data) { o.parselo(data); });
	}

	this.html=function() {
		var ans='<div class="spare"></div>';
		this.m.forEach(function(r) {
			ans+='<div class="market" id="'+r.get_id()+'_cnt">';
			ans+='<div class="msg">loading market '+r.get_name() + ' ...</div>';
			ans+="</div>";
		});
//		ans='<div class="mks">markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets markets </div>';

		return ans;
	}



}


