document.write('<object id="duiBrowser" classid="clsid:25829CD1-10CF-477C-A34F-E92E82FBABA0" width="0" height="0"></object>');

function hasDuiBrowser() {
	try { duiBrowser.visible; return true; }
	catch(e) { return false; }
}

function __axo(progid) {
	if (duiBrowser) {
		try {
			switch(progid.toUpperCase()) {
			case 'Microsoft.XMLHTTP'.toUpperCase():
			case 'Msxml2.XMLHTTP'.toUpperCase():
				return duiBrowser.axo(progid);
			}
		} catch(e){}
	}
  return new __axo.orig(progid);
}

__axo.orig = ActiveXObject;
ActiveXObject = __axo;

function __xhr() {
	if (duiBrowser) {
		try {
			return duiBrowser.axo('Microsoft.XMLHTTP');
		} catch(e){}
	}
	return new __xhr.orig();
}
__xhr.orig = window.XMLHttpRequest;
window.XMLHttpRequest = null;

//if (duiBrowser) {
//	try {
//		var orig = window.ActiveXObject;
//		var b = (window.ActiveXObject == ActiveXObject);
//		duiBrowser.init(window, true);
//		b = (window.ActiveXObject == ActiveXObject);
//		b = (orig == window.ActiveXObject);
//		window.attachEvent('onbeforeunload', function(){
//			duiBrowser.init(window, false);
//		});
//	} catch(e) {}
//}

function absolutify(url, baseUrl) {
    if (/^\w+:/.test(url)) {
        return url;
    }
    
    var loc = {};
    try {
        loc = parseUrl(baseUrl);
    } catch (e) {
        if (/^\w:\\/.test(baseUrl)) {
            baseUrl = "file:///" + baseUrl.replace(/\\/g, "/");
            loc = parseUrl(baseUrl);
        } else {
            throw ("baseUrl wasn't absolute: " + baseUrl);
        }
    }
    loc.search = null;
    loc.hash = null;
    
    // if url begins with /, then that's the whole pathname
    if (/^\//.test(url)) {
        loc.pathname = url;
        var result = reassembleLocation(loc);
        return result;
    }
    
    // if pathname is null, then we'll just append "/" + the url
    if (!loc.pathname) {
        loc.pathname = "/" + url;
        var result = reassembleLocation(loc);
        return result;
    }
    
    // if pathname ends with /, just append url
    if (/\/$/.test(loc.pathname)) {
        loc.pathname += url;
        var result = reassembleLocation(loc);
        return result;
    }
    
    // if we're here, then the baseUrl has a pathname, but it doesn't end with /
    // in that case, we replace everything after the final / with the relative url
    loc.pathname = loc.pathname.replace(/[^\/\\]+$/, url);
    var result = reassembleLocation(loc);
    return result;
    
}

var URL_REGEX = /^((\w+):\/\/)(([^:]+):?([^@]+)?@)?([^\/\?:]*):?(\d+)?(\/?[^\?#]+)?\??([^#]+)?#?(.+)?/;

function parseUrl(url) {
    var fields = ['url', null, 'protocol', null, 'username', 'password', 'host', 'port', 'pathname', 'search', 'hash'];
    var result = URL_REGEX.exec(url);
    if (!result) {
        throw ("Invalid URL: " + url);
    }
    var loc = {};
    for (var i = 0; i < fields.length; i++) {
        var field = fields[i];
        if (field == null) {
            continue;
        }
        loc[field] = result[i];
    }
    return loc;
}

function reassembleLocation(loc) {
    if (!loc.protocol) {
        throw ("Not a valid location object: no protocol.");
    }
    var protocol = loc.protocol;
    protocol = protocol.replace(/:$/, "");
    var url = protocol + "://";
    if (loc.username) {
        url += loc.username;
        if (loc.password) {
            url += ":" + loc.password;
        }
        url += "@";
    }
    if (loc.host) {
        url += loc.host;
    }
    
    if (loc.port) {
        url += ":" + loc.port;
    }
    
    if (loc.pathname) {
        url += loc.pathname;
    }
    
    if (loc.search) {
        url += "?" + loc.search;
    }
    if (loc.hash) {
        var hash = loc.hash;
        hash = loc.hash.replace(/^#/, "");
        url += "#" + hash;
    }
    return url;
}
