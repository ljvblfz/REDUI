function classCreate() {
	return function() {
		this.initialize.apply(this, arguments);
	}
}

function objectExtend(destination, source, deep) {
	for (var property in source) {
		deep = deep || 1;
		if (typeof source[property] == "object" && deep > 1)
			destination[property] = objectExtend({}, source[property], deep - 1);
		else
			destination[property] = source[property];
	}
	return destination;
}

function objectClone(source, deep) {
	return objectExtend({}, source, deep);
}

// css parser
function parseCss(css) {
	var ret = [];
	// 去除注释
	css = css.replace(/\/\/[^\r\n]*[\r\n]|\/\*[\s\S]*\*\//mg, "");
	// 获取RULE集合
	var rules = css.split('}'); // css.match(/\s*(\S+)\s*\{\s*(\S*)\s*\}/gm);
	if (rules) {
		for (var i = 0; i < rules.length; i++) {
			var rule = rules[i]; // xxx { ... }
			var sel_con = rule.split('{');  //rule.match(/^\s*([^\s{}]+)\s*\{\s*(\S*)\s*\}/m);
			if (sel_con.length==2) {
				var style = { cont: sel_con[1].replace(/(^\s*)|(\s*$)/g, "") };
				var names = sel_con[0].match(/\s*([#:=+]?\s*[^#:=+\s]+)\s*/gm);
				if (names) {
					for (var j = 0; j < names.length; j++) {
						var name = names[j].replace(/(^\s*)|(\s*$)/g, "");
						switch (name.charAt(0)) {
							case '#': style.targetId = names[j].substr(1).replace(/(^\s*)|(\s*$)/g, ""); break; //.split('# \t\r\n').join(''); break;
							case ':': style.targetState = names[j].substr(1).replace(/(^\s*)|(\s*$)/g, ""); break;
							case '=': style.id = names[j].substr(1).replace(/(^\s*)|(\s*$)/g, ""); break;
							case '+': style.refid = names[j].substr(1).replace(/(^\s*)|(\s*$)/g, ""); break;
							default: style.targetName = name;
						}
					}
					ret.push(style);
				}
			}
		}
	}
	return ret;
}

// class coverFlow
var coverFlow = classCreate();
objectExtend(coverFlow.prototype, {
    initialize: function(parent, option) {
        this.parent = parent; // || throw 'Invalid parent parameter!';  // 场景或父实体，它的所有子实体将参与到this排列
        if (!parent) throw 'Invalid parent parameter!';

        this.options = objectExtend({
            cx: 1, // 平均间隔
            cx_center: 2.5,  // 中心点到左右的间隔
            z: 3,  // 平均Z值
            z_center: -.5, // 中心点Z值
            duration: .5
        }, option || {});

        this.curIndex = 0.0; // 当前实体的索引，浮点表示偏移的比例

        var _this = this;
        this.story = new StoryBoard().AddTo(window).newFrameSet(
			new FrameSet().targetType('float').target(this).targetAttribute('curIndex')
		).onStep(function() {
		    _this.relayout();
		});
    },

    relayout: function(newIndex) {
        if (!this.parent) return;

        this.curIndex = newIndex || this.curIndex;
        if (this.curIndex < 0) this.curIndex = 0;
        var index = Math.floor(this.curIndex);
        var percent = this.curIndex - index;
        // 修复所有实体的位置和旋转因子
        // 这个例子中仅设置变换矩阵的 x / z / rotationY 三个因子
        var cols = /*this.parent.Entities ||*/this.parent.children || this.parent.Entities;
        for (var i = 0; i < cols.length; i++) {
            var e = cols[i];

            // 中心的实体
            if (i == index) {
                e.transform.position
					.x(-this.options.cx_center * percent)
					.z(this.options.z_center * (1 - percent) + this.options.z * percent);
                e.transform
					.rotationY = -90 * percent;
            }

            // 中心右边的实体
            else if (i == index + 1) {
                e.transform.position
					.x(this.options.cx_center * (1 - percent))
					.z(this.options.z_center * percent + this.options.z * (1 - percent));
                e.transform
					.rotationY = 90 * (1 - percent);
            }

            // 左边的实体
            else if (i < index) {
                e.transform.position
					.x(-(this.options.cx_center + (index - 1 - i) * this.options.cx) - this.options.cx * percent)
					.z(this.options.z);
                e.transform
					.rotationY = -90;
            }

            // 右边的实体
            else if (i > index + 1) {
                e.transform.position
					.x((this.options.cx_center + (i - 1 - index) * this.options.cx) - this.options.cx * percent)
					.z(this.options.z);
                e.transform
					.rotationY = 90;
            }
        }
        this.parent.scene.update();
    },

    moveTo: function(index) {
        this.story.stop().frameSets[0]/*.from(this.curIndex)*/.to(index).duration(this.options.duration * Math.sqrt(Math.abs(this.curIndex - index)));
        this.story.start();
    },

    movePrev: function() {
        this.moveTo(Math.floor(this.curIndex) - 1);
    },

    moveNext: function() {
        this.moveTo(Math.floor(this.curIndex) + 1);
    }
}
);
