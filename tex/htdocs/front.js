function front(v,weakcur,strongcur,weak,depth) {
	this.weak=weak;
	this.spots=new Array(v.length);
	this.weakcur=weakcur;
	this.strongcur=strongcur;
	this.depth=depth;

	var self=this;
	var i=0;
	v.forEach(function(r) {
		self.spots[i]=new tradebox(r.rate,r.vol,weakcur,strongcur,weak);
		i++;
	});

	this.dosort=function() {
		this.spots.sort(function (a, b) { return a.rate < b.rate ? -1 : 1; });
	}

	this.dosort();


	this.find=function (data, rate) {
	    var minIndex = 0;
	    var maxIndex = data.length - 1;
	    var currentIndex;
	    var current;
	 
	    while (minIndex <= maxIndex) {
		currentIndex = (minIndex + maxIndex) / 2 | 0;
		current = data[currentIndex].rate;
	 
		if (current < rate) {
		    minIndex = currentIndex + 1;
		}
		else if (current > rate) {
		    maxIndex = currentIndex - 1;
		}
		else {
		    return currentIndex;
		}
	    }
	 
	    return -1;
	}


	this.frontid=function () {
		return "front_"+this.weakcur+"_"+this.strongcur+"_"+(this.weak?"w":"s");
	}
	this.delete_spot=function (rate) {
		var ndx=this.find(this.spots, rate);
		if (ndx==-1) return;
		$('#'+this.spots[ndx].getid()).remove();
		this.spots.splice(ndx, 1);
	}

	this.is_spot_visible=function (visibledata, rate) {
		if (visibledata.length<this.depth) return true;
		if (this.weak) {
			if (rate<visibledata[0].rate) return false;
		}
		else {
			if (rate>visibledata[visibledata.length-1].rate) return false;
		}
		return true;
	}
	
	this.update=function (data) {//data comes ordered lowest rates first
		var o=this;
		var numout=0;
		while(
		this.spots.some(function(s) {
			var ndx=o.find(data, s.rate);
			if (ndx==-1) {
				//eliminar spot
				//si la profundidad del que elimino es mayor que data.length no animar, solo ha salido del viewport
				if (o.is_spot_visible(data,s.rate)) {
					s.update(0,function () {
						o.delete_spot(s.rate);
					});	
					numout++; //pueden entrar nuevos en scope que no deben flashear
					return false;
				}
				else {
					o.delete_spot(s.rate); //sale de scope
					return true; //empezar de nuevo con el nuevo contenido
				}
			}			
			return false;
		})

		) {
		}
		data.forEach(function(d) {
			var ndx=o.find(o.spots, d.rate);
//alert(ndx);
			if (ndx==-1) {
				var val=0;
				var dndx=o.find(data,d.rate);
				if (o.weak && dndx==0) {
					if (numout>0) {
						val=d.vol; //impide el flash
						--numout; //este entra porque otro ha salido 
					}
				}
				if (!o.weak && dndx==data.length-1) {
					if (numout>0) {
						val=d.vol; //impide el flash
						--numout; //este entra porque otro ha salido 
					}
				}
				var tb=new tradebox(d.rate,val,o.weakcur,o.strongcur,o.weak)
				o.spots.push(tb);
				o.dosort();
				ndx=o.find(o.spots, d.rate);
				if (ndx==0) {
					if (o.spots.length==1) {
						$("#"+o.frontid()).html(o.spots[ndx].html());
					}
					else {
						$(o.spots[ndx].html()).insertBefore($('#'+o.spots[1].getid()));
					}
				}
				else {
					$(o.spots[ndx].html()).insertAfter($('#'+o.spots[ndx-1].getid()));
				}
			}
			o.spots[ndx].update(d.vol,function () {});	
		});
	}

	this.html=function () {
		var ans="";
		this.spots.forEach(function(i) {
			ans+=i.html();
		});
//		ans+='<div class="frontend">&gt;</div>';
/*
var n=1.2040;
for (i=0; i<6; ++i) {
	n+=0.0005;
	ans+=n+" "+1/n+"<br/>";	
}
*/
		return ans;
	};
}

