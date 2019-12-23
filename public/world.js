function Node(value) {
  this.children = new Array(4);
  this.value = value || null;
  this.parent = null;
}

Node.prototype.addChild = function(index, child) {
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
