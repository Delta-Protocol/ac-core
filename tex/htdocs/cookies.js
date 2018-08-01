function delete_cookies() {
	document.cookie="a=0; expires=Thu, 01 Jan 1970 00:00:00 GMT";
	document.cookie="b=0; expires=Thu, 01 Jan 1970 00:00:00 GMT";
}

function get_cookie(name) {
  var value = "; " + document.cookie;
  var parts = value.split("; " + name + "=");
  if (parts.length == 2) return parts.pop().split(";").shift();
}

