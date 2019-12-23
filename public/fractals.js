function mengerSquare(node, depth) {
  if (depth <= 0) {
    return;
  }

  if (!node.children[0]) {
    node.addChild(0, new Node());
  }
  if (!node.children[0].children[3]) {
    node.children[0].addChild(3, new Node());
  }
  node.children[0].children[3].addChild(3, new Node('#000'));

  if (!node.children[1]) {
    node.addChild(1, new Node());
  }
  if (!node.children[1].children[2]) {
    node.children[1].addChild(2, new Node());
  }
  node.children[1].children[2].addChild(2, new Node('#000'));

  if (!node.children[2]) {
    node.addChild(2, new Node());
  }
  if (!node.children[2].children[1]) {
    node.children[2].addChild(1, new Node());
  }
  node.children[2].children[1].addChild(1, new Node('#000'));

  if (!node.children[3]) {
  node.addChild(3, new Node());
  }
  if (!node.children[3].children[0]) {
    node.children[3].addChild(0, new Node());
  }
  node.children[3].children[0].addChild(0, new Node('#000'));

  --depth;
  mengerSquare(node.children[0], depth);
  mengerSquare(node.children[1], depth);
  mengerSquare(node.children[2], depth);
  mengerSquare(node.children[3], depth);
}

function simpleFractal(node, depth) {
  simpleFractal_(node, depth, 0);

  function simpleFractal_(node, depth, index) {
    if (depth <= 0) {
      return;
    }
    node.addChild(index % 2 === 0 ? 0 : 3, new Node('#f00'));
    node.addChild(index % 2 === 0 ? 3 : 0, new Node());

    simpleFractal_(node.children[index % 2 === 0 ? 3 : 0], depth - 1, index + 1);
  }
}

function quadrantIndex(index) {
  if (index === 2) {
    return 3;
  } else if (index === 3) {
    return 2;
  }
  return index;
}

function spiralFractal(node, depth) {
  spiralFractal_(node, depth, 0);

  function spiralFractal_(node, depth, index) {
    if (depth <= 0) {
      return;
    }

    node.addChild(quadrantIndex((index + 2) % 4), new Node('#f00'));
    var next = new Node();
    node.addChild(quadrantIndex((index + 1) % 4), next);

    spiralFractal_(next, depth - 1, index + 1);
  }
}

function loadUniverse(universe, node) {
  universe = Node.fromJson(JSON.parse(universe));
  node.children = universe.children;
  node.value = universe.value;
}

function loadUniverse1(node) {
  loadUniverse(universe1, node);
}

function loadUniverse2(node) {
  loadUniverse(universe2, node);
}
