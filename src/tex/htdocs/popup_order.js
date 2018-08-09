function popup_order() {

	this.set=function () {

		var o=this;
		$('.order').hover(function(ev) {
//alert($(ev.target).closest(".order").parent().html());
			setTimeout(function() { o.show($(ev.target).closest(".order").parent()); },1000);
			//alert("show");
		},function() {
			//alert("exit");
			o.hide();
			
		});

	}

	this.count=0;
	this.show=function(order) {
//alert(order.attr("id")+" "+order.parent().html());
		this.count++;
		if (this.count!=1) {
			return; //shown is delayed but hide is not, so in a quick movement hide could be called before show
		}
		var o=tw.ts.get_order(order.attr("id"));
		if (typeof o == "undefined") return;
		var p=order.offset();
//		alert(p.left+" "+p.top);
//		var top=
//		var lh = $('#mtlbl').height();
//		var rh= parseInt($('#mtcnt').height() - lh); 
//		$('#mtcnt').height(rh);
		var x=parseInt(p.left-$("#tpu").width()-11)-$(document).scrollLeft();
		var y=p.top-$(document).scrollTop();
		$("#tpu").css( { left: x , top: y } )
		this.repaint(o);
	}
	this.hide=function() {
		this.count--;
		$("#tpu").css('display','none');
	}
	this.hide2=function() {
		$("#tpu").css('display','none');
	}
	this.repaint=function(o) {
		$("#tpu").html(o.htmlb());
		$("#tpu").css('display','block');
	}

	
}


