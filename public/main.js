var canvas = document.querySelector('#canvas');
canvas.width = 800;
canvas.height = 1000;

var ctx = canvas.getContext('2d');

var mouse = {
  x: 0,
  y: 0,
  lastX: 0,
  lastY: 0,
};

var universe = new Node();

var value = '#000';
var MAX_DIMENSION = 20;
var DEFAULT_DIMENSION = 3;
var dimension = DEFAULT_DIMENSION;
var MAX_ZOOM = 20000;
var MIN_ZOOM = .62;
var DEFAULT_ZOOM = 1;
var zoom = 1;
var offsetX = 0;
var offsetY = 0;
var showGrid = false;

var pressedKeys = [];
var lastTime = 0;

function update() {
  var time = Date.now();
  var deltaTime = (time - lastTime) / 1000;
  lastTime = time;

  requestAnimationFrame(update);

  var speed = 300 / zoom;
  var velocityX = 0;
  var velocityY = 0;
  if (pressedKeys[keyCode['A']]) {
    velocityX = -1;
  }
  if (pressedKeys[keyCode['D']]) {
    velocityX = 1;
  }
  if (pressedKeys[keyCode['W']]) {
    velocityY = -1;
  }
  if (pressedKeys[keyCode['S']]) {
    velocityY = 1;
  }
  var velocityMagnitude = Math.sqrt(velocityX * velocityX + velocityY * velocityY);
  if (velocityMagnitude > 0) {
    velocityX = velocityX / velocityMagnitude * speed;
    velocityY = velocityY / velocityMagnitude * speed;
    offsetX += velocityX * deltaTime;
    offsetY += velocityY * deltaTime;
  }

  if (pressedKeys[keyCode['E']]) {
    zoom += zoom * deltaTime;
    if (zoom > MAX_ZOOM) {
      zoom = MAX_ZOOM;
    }
  }
  if (pressedKeys[keyCode['Q']]) {
    zoom -= zoom * deltaTime;
    if (zoom < MIN_ZOOM) {
      zoom = MIN_ZOOM;
    }
  }

  if (mouse.down) {
    setValue((mouse.x - canvas.width / 2) / zoom + canvas.width / 2 + offsetX,
             (mouse.y - canvas.height / 2) / zoom + canvas.height / 2 + offsetY,
             dimension, value);
  }

  ctx.fillStyle = '#444';
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  ctx.save();
  ctx.translate(canvas.width / 2, canvas.height / 2);
  ctx.scale(zoom, zoom);
  ctx.translate(-canvas.width / 2, -canvas.height / 2);
  ctx.translate(-offsetX, -offsetY);

  ctx.fillStyle = '#fff';
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  draw(universe);

  ctx.restore();

  // hover((mouse.x - canvas.width / 2) / zoom + canvas.width / 2 + offsetX,
  //       (mouse.y - canvas.height / 2) / zoom + canvas.height / 2 + offsetY,
  //       dimension, value);
  hover(mouse.x, mouse.y, dimension, value);
}

update();

function setValue(x0, y0, dimension, value) {
  var node = universe;
  var size = canvas.height;
  var x = 0;
  var y = 0;
  for (var i = 0; i < dimension; ++i) {
    size /= 2;
    var index = 0;
    if (x0 < size + x && y0 < size + y) {
      index = 0;
    } else if (x0 >= size + x && y0 < size + y) {
      index = 1;
      x += size;
    } else if (x0 < size + x&& y0 >= size + y) {
      index = 2;
      y += size;
    } else if (x0 >= size + x && y0 >= size + y) {
      index = 3;
      x += size;
      y += size;
    }
    var child = node.children[index];
    if (!child) {
      child = new Node();
      node.addChild(child, index);
    }
    node = child;
  }
  node.setAllValues(value);
}

function hover(x, y) {
  ctx.lineWidth = 1;
  ctx.strokeStyle = '#f00';
  var width = canvas.width * Math.pow(2, -dimension) * zoom;
  var height = canvas.height * Math.pow(2, -dimension) * zoom;
  ctx.strokeRect(x - width / 2, y - height / 2, width, height);
}

function draw(universe) {
  var size = canvas.height;
  drawNode(universe, 0, 0, size);
  if (showGrid) {
    ctx.lineWidth = 1 / zoom;
    ctx.strokeStyle = '#f00';
    ctx.strokeRect(1, 1, size - 2, size - 2);
  }
}

function drawNode(node, x, y, size) {
  if (!node) {
    if (showGrid) {
      ctx.lineWidth = 1 / zoom;
      ctx.strokeStyle = '#f00';
      ctx.strokeRect(x, y, size, size);
    }
    return;
  }
  if (node.value) {
    ctx.fillStyle = node.value;
    ctx.fillRect(x, y, size, size);
    if (showGrid) {
      ctx.lineWidth = 1 / zoom;
      ctx.strokeStyle = '#f00';
      ctx.strokeRect(x, y, size, size);
    }
  }

  if (!node.isLeaf()) {
    size /= 2;
    drawNode(node.children[0], x, y, size);
    drawNode(node.children[1], x + size, y, size);
    drawNode(node.children[2], x, y + size, size);
    drawNode(node.children[3], x + size, y + size, size);
  }
}

window.addEventListener('keydown', e => {
  pressedKeys[e.keyCode] = true;

  switch (e.keyCode) {
    case keyCode['1']:
      value = '#000';
      break;
    case keyCode['2']:
      value = '#fff';
      break;
    case keyCode['Up']:
      --dimension;
      if (dimension < 0) {
        dimension = 0;
      }
      break;
    case keyCode['Down']:
      ++dimension;
      if (dimension > MAX_DIMENSION) {
        dimension = MAX_DIMENSION;
      }
      break;
    case keyCode['G']:
      showGrid = !showGrid;
      break;
  }
});

window.addEventListener('keyup', e => {
  pressedKeys[e.keyCode] = false;
});

function onMouseDown(x, y) {
  mouse.lastX = mouse.x;
  mouse.lastY = mouse.y;
  var offset = canvas.getBoundingClientRect();
  mouse.x = (x - offset.left) * canvas.width / offset.width;
  mouse.y = (y - offset.top) * canvas.height / offset.height;

  mouse.down = true;
}

canvas.addEventListener('mousedown', e => {
  onMouseDown(e.clientX, e.clientY);
});
canvas.addEventListener('touchstart', e => {
  onMouseDown(e.touches[0].clientX, e.touches[0].clientY);
});

function onMouseMove(x, y) {
  mouse.lastX = mouse.x;
  mouse.lastY = mouse.y;
  var offset = canvas.getBoundingClientRect();
  mouse.x = (x - offset.left) * canvas.width / offset.width;
  mouse.y = (y - offset.top) * canvas.height / offset.height;
}

canvas.addEventListener('mousemove', e => {
  onMouseMove(e.clientX, e.clientY);
});
canvas.addEventListener('touchmove', e => {
  onMouseMove(e.touches[0].clientX, e.touches[0].clientY);
});

function onMouseUp() {
  mouse.down = false;
}

window.addEventListener('mouseup', e => {
  onMouseUp();
});
window.addEventListener('touchend', e => {
  onMouseUp();
});

canvas.addEventListener('mouseleave', e => {
});
