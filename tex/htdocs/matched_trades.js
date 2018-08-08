function matched_trades() {
	this.trades=new Array();

/*
Array of 
{ id:tradeid,  amount_paid:amount_paid,  cur_paid:cur_paid,
  rate:rate,
  weakcur:weakcur,  strongcur:strongcur,
  amount_received:amount_received,  cur_received:cur_received,
  commission_paid:commission_paid,  cur_commission:cur_commission}
*/
	this.clean=function () {
		this.trades=new Array();
	}

	this.update=function (trds) {
		this.trades=trds;

	}

	this.repaint=function(parent) {
		parent.html(this.html());
//		var lh = $('#mtlbl').height();
//		var rh= parseInt($('#mtcnt').height() - lh); 
//		$('#mtcnt').height(rh);
	}

	this.html=function() {
		var ans='<div class="trade">';
		ans+='<div id="mtlbl" class="lbl">Completed trades</div>';
		ans+='<div id="mtcnt">';
		ans+='<table cellspacing="0" cellpadding="3">';
		if (this.trades.length>0) {
		ans+='<th class="thm1">id</th>';
		ans+='<th class="thm2"></th>';
		ans+='<th class="thm1">trade</th>';
		ans+='<th class="thm2">rate</th>';
		ans+='<th class="thm1">outcome</th>';
		ans+='<th class="thm2">commission</th>';
		}
		this.trades.forEach(function(r) {
			ans+='<tr><td class="tdm1">'+r.id+'</td>';
			ans+='<td class="tdm2" title="'+(r.type=="S"?"Sell":"Buy")+'">'+r.type+'</td>';
			if (r.type=="S") {
				var at="Sold "+r.amount_traded+" "+r.cur_traded;
			}
			else {
				var at="Bought "+r.amount_traded+" "+r.cur_traded;
			}
			ans+='<td class="tdm1" title="'+at+'">'+r.amount_traded+" "+r.cur_traded+"</td>";
			ans+='<td class="tdm2" title="rate">'+r.rate+" "+r.weakcur+"/"+r.strongcur+"</td>";
			if (r.type=="S")
			ans+='<td class="tdm1" title="Received">+ '+r.amount_outcome+" "+r.cur_outcome+"</td>";
			else
			ans+='<td class="tdm1" title="Paid">- '+r.amount_outcome+" "+r.cur_outcome+"</td>";

			ans+='<td class="tdm2">'+r.commission_paid+" "+r.cur_commission+"</td></tr>";
		});
		ans+='</table>';

		ans+='</div>';
		ans+='</div>';
		return ans;
	}
}

