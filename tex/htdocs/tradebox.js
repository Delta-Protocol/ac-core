function round(x, n) {
     return Math.round(x * Math.pow(10, n)) / Math.pow(10, n)
}

function tradebox(rate,volume,weakcur,strongcur,weak) {
	this.weak=weak;
	this.rate=rate;
	this.volume=volume;
	this.weakcur=weakcur;
	this.strongcur=strongcur;
	this._id='tb_'+this.weakcur+'_'+this.strongcur+"_"+(this.rate).replace(".","_");

	this.update=function (vol, handler) {
//alert(data);
		if (vol==this.volume) return;

		if (vol==0) { //sacar el spot del front
			this.volume=vol;
			var id='#'+this.getid();
			var el=$(id);
			$(id+'_vc').replaceWith(this.getvc());
			el.css("background-color","#ff8");
			var position = el.offset();
			position.top-=$(document).scrollTop();
			position.left-=$(document).scrollLeft();
			el.css("position","fixed");
			el.css(position);
			el.animate({
				top: "+=20"
			},600,handler);

		}
		else {
			if (parseFloat(vol)<parseFloat(this.volume)) {
				var dir1="+";
				var dir2="-";
			}
			else {
				var dir1="-";
				var dir2="+";
			}
//alert("despues "+vol+"<- antes"+this.volume+" "+(vol<this.volume));
			this.volume=vol;
			$('#'+this.getid()).css("background-color","#ff8");

			var o=this;
			$('#'+this.getid()+'_vc').fadeOut(100).fadeIn('slow').delay(100).fadeOut(100).fadeIn(100).delay(100).fadeOut(100).fadeIn('slow');
			$('#'+this.getid()+'_vc').animate({
				top: dir1+"=10"
			},600,function(){
				$('#'+o.getid()+'_vc').html(o.getvc_value());
				$('#'+o.getid()+'_vc').animate({
					top: dir2+"=10"
					},100,function(){
						$('#'+o.getid()+'_vc').replaceWith(o.getvc());
						handler();
					});
					
			});
			var o=this;
			setTimeout(function() { $('#'+o.getid()).css("background-color",""); },1000);
		}
	}

 	this.format_rate=function (rate) {
 		rtup=round(parseFloat(rate)+0.00005,5).toFixed(5).toString();
 		rt=round(parseFloat(rate),5).toFixed(5).toString();
 		rtdown=round(parseFloat(rate)-0.00005,5).toFixed(5).toString();
 		df=-1;
 		for (var i = 0, len = rt.length; i < len; i++) {
  			if (rt[i]==rtup[i] && rt[i]==rtdown[i]) continue;
 			df=i;
 			break;
 		}
 		if (df==-1) return rt;
 		return '<font size="-1px">'+rt.substr(0,df)+"</font>"+rt.substr(df,rt.length-df);
 	}

	this.getid=function () {
		return this._id;
	}

	this.getvc_value=function () {
		var v=Math.round(this.volume).toLocaleString();
		var ans=symbol[this.weak?this.weakcur:this.strongcur]+' '+v;
		return ans;
	}

	this.getvc=function () {
		var ans="";
		ans+='<div class="vc" id="'+this.getid()+'_vc" title="liquidity: '+this.volume.toLocaleString()+' '+(this.weak?this.weakcur:this.strongcur)+' available at rate '+this.rate+' '+this.weakcur+'/'+this.strongcur+'">';
		ans+=this.getvc_value();
		ans+="</div>";
		return ans;
	}

	this.html=function () {
		var ans='<div class="tb" id="'+this.getid()+'">';
//style="color: '+curcolor[weak?this.strongcur:this.weakcur]+';"
		ans+='<div class="uc" title="Buy '+(this.weak?this.weakcur:this.strongcur)+' at a price of ';
		if (this.weak) { //buy, weak
			linkargs="true, true,";
			ans+=this.rate+' '+this.weakcur+' per '+this.strongcur;
			linkargs+="'"+this.weakcur+"','"+this.strongcur+"','"+this.rate+"'";
		}
		else {
			linkargs="true, false,";
			ans+=this.rate+' '+this.strongcur+' per '+this.weakcur;
			linkargs+="'"+this.weakcur+"','"+this.strongcur+"','"+this.rate+"'";
		}
		ans+='" onclick="tw.new_trade('+linkargs+');">';
		ans+="buy "+(this.weak?weakcur:strongcur);
		ans+="</div>";
//style="color: '+curcolor[weak?this.weakcur:this.strongcur]+';"
		ans+='<div class="mc" title="Sell '+(this.weak?this.weakcur:this.strongcur)+' at a price of ';
		if (this.weak) {
			var linkargs="false, true,";
			ans+=this.rate+' '+this.weakcur+' per '+this.strongcur;
			linkargs+="'"+this.weakcur+"','"+this.strongcur+"','"+this.rate+"'";
		}
		else {
			var linkargs="false, false,";
			ans+=this.rate+' '+this.strongcur+' per '+this.weakcur;
			linkargs+="'"+this.weakcur+"','"+this.strongcur+"','"+this.rate+"'";
		}
		ans+='" onclick="tw.new_trade('+linkargs+');">';
		ans+="sell "+(this.weak?this.weakcur:this.strongcur);
		ans+="</div>";
		ans+='<div class="tbrt">';
		ans+=this.format_rate(this.rate);
		ans+='</div>';
		ans+='<div class="tbrtu">';
		ans+=this.weakcur+'/'+this.strongcur;
		ans+='</div>';
		ans+=this.getvc();
		ans+="</div>";
		return ans;
	};

}

tradebox.prototype.valueOf = function () {
    return this.rate;
};


