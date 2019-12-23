function Node(value) {
  this.children = new Array(4);
  this.value = value || null;
  this.parent = null;
}

Node.prototype.addChild = function(index, child) {
  if (!child) {
    return;
  }
  this.children[index] = child;
  child.parent = this;
}

Node.prototype.isLeaf = function() {
  return !(this.children[0] || this.children[1] || this.children[2] ||
           this.children[3]);
}

Node.prototype.setAllValues = function(value) {
  this.value = value;
  for (var i = 0; i < 4; ++i) {
    this.children[i] = null;
  }
};

Node.prototype.toJson = function() {
  var json = {};
  json.value = this.value;
  json.children = new Array(4);
  for (var i = 0; i < 4; ++i) {
    json.children[i] = this.children[i] ? this.children[i].toJson() : null;
  }
  return json;
};

Node.fromJson = function(json) {
  if (!json) {
    return null;
  }
  var node = new Node(json.value);
  for (var i = 0; i < 4; ++i) {
    node.addChild(i, Node.fromJson(json.children[i]));
  }
  return node;
};
