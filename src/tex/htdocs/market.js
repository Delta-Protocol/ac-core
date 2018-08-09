function market( c,ic) {
	this.self=this;
	this.cur=c;
	this.invcur=ic;
	this.fweak=null;
	this.fstrong=null;
	this.valid_rates_weak=new Array();
	this.valid_rates_strong=new Array();
	this.depth=8;

	this.has_valid_rates=function () {
		return this.valid_rates_weak.length==0;
	}

	this.get_name=function () {
		return this.cur+"/"+this.invcur;
	}
	this.get_id=function () {
		return "mkt_"+this.cur+"_"+this.invcur;
	}

	this.parselo=function(data) {
//alert(data);
		//lines
		//1.6250 6.0000 1
		var lines = data.split('\n');
		var length = lines.length;
		var weakcur=curid[this.cur];
		//alert("weaksig "  +weaksig);
		var weakdata= new Array;
		var strongdata= new Array;
		var thedata=new Array;
		for(var i = 0; i < length; i++){
		        var fields=lines[i].split(" ");
			if (fields.length!=3) {
				continue;
			}
			var rt=fields[0];
			var v=fields[1];
			var cur=fields[2];
			if (cur==weakcur) {
				weakdata[weakdata.length]={ rate:rt, vol:v };
			}
			else {
				strongdata[strongdata.length]={ rate:rt, vol:v };
			}

		}
		//weakdata.sort(function(va,vb) { return parseFloat(vb.rate)-parseFloat(va.rate); });
		//strongdata.sort(function(va,vb) { return parseFloat(vb.rate)-parseFloat(va.rate); });
		var build=false;
		if (this.fweak==null) {
			this.fweak=new front(weakdata,this.cur,this.invcur,true,this.depth);
			build=true;
		}
		else {
			this.fweak.update(weakdata);
		}
		if (this.fstrong==null) {
			this.fstrong=new front(strongdata,this.cur,this.invcur,false,this.depth);
			build=true;
		}
		else {
			this.fstrong.update(strongdata);
		}
		if (build) {
			var id=this.get_id();
			$("#"+id+"_cnt").html(this.html());
		}



	}
	this.load_valid_rates=function() {
		$.ajax({
		  url: endpoint+"/?valid_rates_simple "+curid[this.cur]+" "+curid[this.invcur],
		  dataType: "text"
		}).done(function(data){tw.parselo_valid_rates(this.cur,this.invcur,data);});
	}
	this.slide_to_front=function () {
//alert("#"+this.get_id());
//document.getElementById("mkt_AUD_USD").scrollLeft += 100;
		var gap=$("#"+this.get_id()).scrollLeft()+$('#gap_'+this.cur+"_"+this.invcur).offset().left;

	//	alert(gap);
		gap-=$("#"+this.get_id()).width()/2;
		if (gap<0) return;
		if (parseInt($("#"+this.get_id()).scrollLeft())!=parseInt(gap)) {
			$("#"+this.get_id()).animate({
					    scrollLeft: ''+parseInt(gap)+''
					}, 1000, function () { } );
		}
/*
$("#"+this.get_id()).scrollLeft(100);
alert("#"+this.get_id()+" "+$("#"+this.get_id()).scrollLeft());
			   
*/		
	}

	this.load=function() {
		var o=this;
		$.ajax({
		  url: endpoint+"/?liquidity "+curid[this.cur]+" "+curid[this.invcur]+" "+this.depth,
		  dataType: "text"
		}).done(function(data){o.parselo(data);});
	}

	this.html=function () {
		var ans="";
		var handlerslide="'"+this.get_id()+"'";
		ans+='<div class="mt" onclick="tw.slide_to_front('+handlerslide+')">'+this.cur+"/"+this.invcur+"</div>";
		ans+='<div class="m" id="'+this.get_id()+'">';
		ans+='<div class="mf" id="front_'+this.cur+"_"+this.invcur+'_w">';
		ans+=this.fweak.html();
		ans+="</div>";
		ans+='<span id="gap_'+this.cur+"_"+this.invcur+'">-</span>';
		ans+='<div class="mf" id="front_'+this.cur+"_"+this.invcur+'_s">';
		ans+=this.fstrong.html();
		ans+="</div>";
		ans+="</div>";
/*
		ans+='<div class="m" id="'+this.get_id()+'">';
		ans+='<div class="mt">'+this.cur+"/"+this.invcur+"</div>";
		ans+='<div class="mf">';
		ans+=this.fweak.html();
		ans+="</div>";
		ans+='<div class="mt" id="'+this.get_id()+'">'+this.invcur+"/"+this.cur+"</div>";
		ans+='<div class="mf">';
		ans+=this.fstrong.html();
		ans+="</div>";
		ans+="</div>";
*/
		return ans;
	}

}

