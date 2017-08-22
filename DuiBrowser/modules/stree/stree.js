/*
 * stree - Simple Tree for jQuery Plugin
 *
 * Author: James Hu
 *
 * Licensed under the GPL license
 *
 * $Date: 2010-02-07 $
 * $version: 0.1
 */

// 树容器应该有类 stree
// 每个 DIV 都是节点，单行，类名 tree-node
// 判断节点是否父节点的唯一条件是子节点数 > 0
//

jQuery.stree = {
	classname: {
		container: 'stree',
		node: 'tree-node',
		content: 'tree-content',
		parent: 'tree-parent',
		closed: 'tree-closed',
		loading: 'tree-loading',
		loaded: 'tree-loaded',
		toolbar: 'tree-toolbar',
		button: 'tree-toolbar-button',
		flush: 'tree-flush'
	},
	icon: {
		//root				: 'base.gif',		folder			: 'folder.gif',		folderOpen	: 'folderopen.gif',		node				: 'page.gif',				empty				: 'empty.gif',		line				: 'line.gif',		join				: 'join.gif',		joinBottom	: 'joinbottom.gif',		plus				: 'plus.gif',		plusBottom	: 'plusbottom.gif',		minus				: 'minus.gif',		minusBottom	: 'minusbottom.gif',		nlPlus			: 'nolines_plus.gif',		nlMinus			: 'nolines_minus.gif',		loading			: 'loading.gif'	},
	iconpath: 'modules/stree/images/',
	getIcon: function(icontype) { return $t.iconpath + $t.icon[icontype]; },
	
	baseUrl: function(ctx) { return null; },
	absolutify: function(url, baseurl) { return $('<a/>').attr('href',url).get(0).href; },
	getAjax: function(url,ctx) { return null; },
	
	language_text: {
		def: {
			'reload': ['刷新','重新加载数据'] // [text, tips]
		}
	},
	language: null,
	getText: function(text, isTip, lang) {
		if (!text || typeof text !== 'string') throw 'parameter of getText() is invalid.';
		var index = 0;
		if (typeof(isTip) == 'string') {
			lang = isTip;
			isTip = false;
		}
		if (isTip === true) index = 1;
		if (!lang) lang = 'def';
		return ($t.language_text[lang] && $t.language_text[lang][text] 
					&& $t.language_text[lang][text].length && $t.language_text[lang][text].length>index) ? $t.language_text[lang][text][index] : text;
	},
	
	updates: [],
	
	update: function(ctx) {
		ctx = $(ctx).closest('.'+$c.node+', .'+$c.container);
		if (ctx.length==0) return;
		var tree = ctx.closest('.'+$c.container);
		
		// set node and parent
		tree.find('div:not(.'+$c.node+')').addClass($c.node)
			.parentsUntil(ctx).addClass($c.parent)
			.end().each(function(){$(this).children(':not(div,img):first').addClass($c.content);});
		// parent include 'src' attribute
		tree.find('.'+$c.node+':not(:has(div)):has(.'+$c.content+'[src])').addClass($c.parent);
			
		// update toolbar
		tree.find('.'+$c.content+'[src]').not(':has(.'+$c.toolbar+')').each(function(){
			var _this = $(this);
			(_this).append($('<span class="'+$c.toolbar+'"></span>')/*.attr('src',(_this).attr('src'))*/
				.append($('<span class="'+$c.button+' '+$c.flush+'" title="'+$t.getText('reload',true)+'"></span>').text($t.getText('reload')))
			);
		});
		
		// remove parent but no children
		ctx.find('.'+$c.parent).andSelf().filter(':not(:has(div, .'+$c.content+'[src]))').removeClass($c.parent);
//		ctx.find('.'+$c.parent).andSelf().each(function(){
//			if ($(this).has('.'+$c.node).length == 0)
//				$(this).removeClass($c.parent);
//		});
	
		// update visible state
		ctx.find('.'+$c.parent).filter('.'+$c.closed).children('.'+$c.node).hide();
		ctx.find('.'+$c.parent+':not(.'+$c.closed+')').children('.'+$c.node).show();

		$t.updateIcons(tree/*ctx*/);
		
		// now exec all update callbacks
		for (var i=0, num=$t.updates.length; i<num; i++) {
			if ($.isFunction($t.updates[i]))
				$t.updates[i].apply(tree/*this*/, [ctx]);
		}
		
		//$t.ctx = ctx;
		$t.tree = tree;
		setTimeout($t.download, 0);
	},
	
	download: function(dst) {
		var tree = null;
		if (!dst) {
			tree = $t.tree;
			dst = tree.find('.'+$c.node+':not(.'+$c.loading+',.'+$c.loaded+')').children('.'+$c.content+'[src]');
		} else {
			tree = dst.closest('.'+$c.container);
		}
		
		if (dst.length==0) {
			$t.tree = null;
			return;
		}
		dst = $(dst[0]);
		var url = dst.attr('src');
		//dst.removeAttr('src');
		if (!url) { $t.tree=null; return; }
		var url2 = $t.absolutify(url, $t.baseUrl(dst) || location.href);
		//var url2 = $('<a/>').attr('href',url).get(0).href;
		var ctx = dst.closest('.'+$c.node);
//		if (ctx.hasClass($c.loading) || ctx.hasClass($c.loaded)) { $t.tree=null; return; }
		ctx.addClass($c.loading);
		$t.updateIcons(ctx);
		
		($t.getAjax(url2,ctx) || $.post)(url2, function(data){
			ctx.removeClass($c.loading).addClass($c.loaded).children('div').remove();
			$(data).appendTo(ctx).find('div').andSelf().each(function(){
				$(this).children(':not(div,img):first').each(function(){$(this).attr('_text', $(this).text());});
				var caption = $(this).children(':not(div,img):not([title]):first');
				if (caption.length > 0)
					caption.attr('title',caption.text());
			});
			$t.update(ctx);
			//setTimeout(download, 0);
		});
	},

	updateIcons : function(item) {
		var tree = $(item).closest('.'+$c.node+', .'+$c.container);
		if (tree.length==0) return false;
		if (tree.is('.'+$c.node))
			tree.add(tree.find('.'+$c.node));
		else tree = tree.find('.'+$c.node);
		
		tree.each(function(){
			var node = $(this);
			// 删除已有的图标
			node.children('img.tree-icon').remove();
			// 重新添加图标
			// 节点(node), 有子节点[展开(folderOpen)，收起(folder)]
			var isParent = node.hasClass($c.parent);
			var isClosed = node.hasClass($c.closed);
			var isLoading = node.hasClass($c.loading);
			var isFirst = (node.prevAll('.'+$c.node).size()==0);
			var isLast = (node.nextAll('.'+$c.node).size()==0);
			var icon = $t.getIcon(isLoading ? 'loading' : (isParent ? (isClosed ? 'folder' : 'folderOpen') : 'node'));
			var line = null;
			var open = {};
			if (isParent) {
				if (isClosed) {
					if (isLast) line = 'plusBottom';
					else line = 'plus';
				} else {
					if (isLast) line = 'minusBottom';
					else line = 'minus';
				}
				open['class'] = 'openable';
			} else {
				if (isLast) line = 'joinBottom';
				else line = 'join';
			}
			line = $t.getIcon(line);
			$('<img/>').attr('src',line).attr(open).add($('<img/>').attr('src',icon).addClass('tree-lasticon')).addClass('tree-icon').prependTo(node);
			// 添加空白或者连接线
			node.parentsUntil('.'+$c.container).each(function(){
				var p = $(this);
				var _closed = p.hasClass($c.closed);
				var _first = (p.prevAll('.'+$c.node).size()==0);
				var _last = (p.nextAll('.'+$c.node).size()==0);
				var _line = (_last ? 'empty' : 'line');
				_line = $t.getIcon(_line);
				$('<img/>').attr('src', _line).addClass('tree-icon').prependTo(node);
			});
		});
	}
};

(function($){
	$.fn.extend( {
		// call: $(...).stree() / $(...).stree(context) / $(...).stree(url) / $(...).stree(url, context)
		stree: function(url, context) {
			// url == "" / null / undefined
			if (!url) {
				context = undefined;
			
			// url is function, so set callback list when updation is needed.
			} else if ($.isFunction(url)) {
				$t.updates.push(url);
				url = undefined;
			
			// url isn't string, or url is a valid selector, so it's context only
			} else if ((typeof url !== 'string') || (!context && $(url).length>0)) {
				context = url;
				url = undefined;
				
			// now url is an URL string
			} else {}
			
			return this.each(function() {
				var tree = $(this);
				if (context && $(context).length>0 && !$.contains(this, $(context)[0]))
					return;
				
				// set container
				if (!tree.hasClass($c.container))
					tree.addClass($c.container);
					
				var ctx = $(context||this).closest('.'+$c.node+', .'+$c.container);
				
				if (!url) {
					$t.update(ctx);
					
				// url is valid, so load tree content in ctx
				} else {
					var url2 = $t.absolutify(url, $t.baseUrl(ctx) || location.href);
//					var xml = new ActiveXObject("Msxml2.DOMDocument");
//					xml.async = true;
//					xml.onreadystatechange = function(){
//						if (xml.readyState == 4) {
//							$(xml.xml).appendTo(ctx);
//							$t.update(ctx);
//						}
//					};
//					xml.load(url2);
//					
					// 下面这段代码在跨域时会拒绝访问
					($t.getAjax(url2,ctx) || $.post)(url2, function(data, status, xhr){
						$(data).appendTo(ctx);
						$t.update(ctx);
					});
				}
			});
		}
	
		
	});
	
	window.$t = $.stree;
	window.$c = $t.classname;

	$('.'+$c.container+' img.openable'+', .'+$c.content).live('click', function(){
		var node = $(this).closest('.'+$c.node).filter('.'+$c.parent);
		if (node.children('.'+$c.content+'[src]').length>0 && node.is(':not(.'+$c.loading+',.'+$c.loaded+')')) {
			node.children('.'+$c.content).find('.'+$c.flush).click();
		} else {
			node.toggleClass($c.closed).children('.'+$c.node).toggle();
			$t.updateIcons(this);
		}
	});
	
	$('.'+$c.flush).live('click', function(e){
		$(this).closest('.'+$c.node).removeClass($c.loaded).removeClass($c.closed).children('div').remove();
		$t.update($(this));
		return false;
	});
})(jQuery);
