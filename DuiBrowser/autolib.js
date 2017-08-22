// host:
//  isDebug - [get/put] [boolean]
//  log - [put] print log info to debugger window. sample: log = 'abc' + 3;
if (host.isDebug) debugger;

var version = host.fileVersion('');
var coreVersion = host.fileVersion('redui');

function getVersion(path) {
	return host.fileVersion(path||'');
}

function axo(progid) {
//	if (progid.toUpperCase()=='Microsoft.XMLHTTP'.toUpperCase())
//		return new xhrproxy();
	return new ActiveXObject(progid);
}
//function new_xhr() {
//	return new ActiveXObject('Microsoft.XMLHTTP');
//}

//function xhrproxy() {
//	this.xhr = new ActiveXObject('Microsoft.XMLHTTP');
//	this.onreadystatechange = null;
//	var _this = this;
//  this.xhr.onreadystatechange = function() {
//		_this._update();
//		if (_this.onreadystatechange) { try{ _this.onreadystatechange(); }catch(e){} } };
//	this._update();
//}
//xhrproxy.prototype.open = function(a,b,c,d,e) { host.info = 'open('+b+')';
//	return this._update(this.xhr.open(a,b,c,d,e)); };
//xhrproxy.prototype.send = function(a) { 
//	this._update(this.xhr.send(a)); };
//xhrproxy.prototype.abort = function() { 
//	this._update(this.xhr.abort()); };
//xhrproxy.prototype.getAllResponseHeaders = function() { 
//	return this._update(this.xhr.getAllResponseHeaders()); };
//xhrproxy.prototype.getResponseHeader = function(a) { 
//	return this._update(this.xhr.getResponseHeader(a)); };
//xhrproxy.prototype.setRequestHeader = function(a,b) { 
//	this._update(this.xhr.setRequestHeader(a,b)); };
//xhrproxy.prototype._update = function(ret) {
//	try{ this.readyState=this.xhr.readyState; }catch(e){ this.readyState=undefined; }
//	try{ this.responseBody=this.xhr.responseBody; }catch(e){ this.responseBody=undefined; }
//	try{ this.responseText=this.xhr.responseText; }catch(e){ this.responseText=undefined; }
//	try{ this.responseXML=this.xhr.responseXML; }catch(e){ this.responseXML=undefined; }
//	try{ this.status=this.xhr.status; }catch(e){ this.status=undefined; }
//	try{ this.statusText=this.xhr.statusText; }catch(e){ this.statusText=undefined; }
//	return ret;
//};


// 如果 success 回调参数有效，则使用异步方式，否则同步加载并且立即返回结果串
function httpLoad(url, success) {
	var xml = new ActiveXObject('Msxml2.DOMDocument');
	if (success) {
		xml.async = true;
		xml.onreadystatechange = function(){
			if (xml.readyState==4) {
				success(xml.xml);
			}
		}
	}
	xml.load(url);
	if (!success) return xml.readyState==4 ? xml.xml : '';
}

var coll = {
	find: function(coll, fn) {
		var ec = new Enumerator(coll);
		for (; !ec.atEnd(); ec.moveNext()) {
			if (fn(ec.item())) return ec.item();
		}
		return false;
	},

	iter: function(coll, fn) {
		var ec = new Enumerator(coll);
		for (; !ec.atEnd(); ec.moveNext()) {
			if (fn) fn(ec.item());
		}
	}
};

function prepareFile(path, filename) {
	if (!path || !(typeof(path)==='string' || path.length)) return false;
	if (typeof(path) === 'string')
		path = path.split('\\'); // 变成数组，文件夹次序
	
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	function folder(_path, ref) {
		try { return ref ? (coll.find(ref.SubFolders, function(item){return item.Name.toUpperCase()==_path.toUpperCase();})||ref.SubFolders.Add(_path)) : fso.GetFolder(_path); }
		catch(e) { return ref ? ref.SubFolders.Add(_path) : fso.CreateFolder(_path); }
	}
	//var rootDir = null;
	//if (!fso.FolderExists(host.cacheDir))
	var rootDir = folder(host.cacheDir);
	if (!rootDir) return false;
	var articleDir = folder((typeof(filename)==='string')?'Samples':'Articles', rootDir);
	if (!articleDir) return false;
	
	var curDir = articleDir;
	for (var i=0; i<path.length; i++) {
		curDir = folder(path[i], curDir);
	}
	
	if (!filename) filename = 'index.htm';
	else if (filename==='__auto__') {
		var i=1;
		while (fso.FileExists(curDir.Path + '\\' + (filename='sample '+i+'.xml'))) i++;
	}
	return curDir.CreateTextFile(filename, true, true);
}

/*
	html - (string) [requird]
	path - (string, array) [requird] 'abc\\def' or ['abc', 'def']
	filename - (string) [optional] 'xxx.htm' or 'xxx.xml' or null, default is 'index.htm'
*/
function writeToFile(text, path, filename) {
	if (!text) return false;
	
	var file = prepareFile(path, filename);
	if (!file) return false;
	file.Write(text);
	file.Close();
	return true;

//	// write to file
//	var stream = new ActiveXObject('ADO.Stream'); // FAILED TO CREATE
////	enum {
////    adTypeBinary = 1,
////    adTypeText = 2
////	} StreamTypeEnum;
//	stream.Type = 2;

////	enum {
////    adModeUnknown = 0,
////    adModeRead = 1,
////    adModeWrite = 2,
////    adModeReadWrite = 3,
////    adModeShareDenyRead = 4,
////    adModeShareDenyWrite = 8,
////    adModeShareExclusive = 12,
////    adModeShareDenyNone = 16,
////    adModeRecursive = 0x00400000
////	} ConnectModeEnum;
//	stream.Mode = 3;
//	stream.Charset = 'utf-8';
//	stream.Open();
//	stream.WriteText(text);
//	
////	enum {
////    adSaveCreateNotExist = 1,
////    adSaveCreateOverWrite = 2
////	} SaveOptionsEnum;
//	stream.SaveToFile(path, 2);
//	stream.Close();
}

function readFromFile(path, fn, hasFiles) {
	if (!hasFiles) hasFiles = false;
	var fso = new ActiveXObject('Scripting.FileSystemObject');

	var data = '';
	if (fso.FileExists(path)) {
		var file = fso.OpenTextFile(path, 1, false, -1);
		if (file) {
			data = file.ReadAll();
			file.Close();
		}
	
	// 如果只是文件夹，认为是读取子文件夹或者读取子文件列表
	} else if (fso.FolderExists(path)) {
		// 读取子文件夹列表
		coll.iter(fso.GetFolder(path).SubFolders, function(sub){
			var htmlfile = coll.find(sub.Files, function(item){return item.Name.toLowerCase()=='index.htm';});
			data += '<div><span';
			if (sub.SubFolders.Count>0 || (hasFiles && sub.Files.Count>0)) data += ' src="' + sub.Name + '"';
			if (htmlfile && !hasFiles) data += ' href="' + sub.Name + '\\index.htm"';
			data += '>' + sub.Name + '</span></div>\n';
		});
		// 读取子文件列表
		if (hasFiles) {
			coll.iter(fso.GetFolder(path).Files, function(file){
				data += '<div><span';
				data += ' href="' + file.Name + '"';
				data += '>' + file.Name + '</span></div>\n';
			});
		}
	}
	
	if (fn) fn(data);
	else return data;
}