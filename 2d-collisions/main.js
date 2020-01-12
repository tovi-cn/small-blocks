let canvas = document.querySelector('#canvas');
canvas.width = 800;
canvas.height = 600;
let ctx = canvas.getContext('2d');

let pressed = [];
let lastTime = Date.now();

let bodies = [];
let varyingSizeBody;

let player;
let speed = 250;

let playerNormalWidth = 100;
let playerNormalHeight = 100;

let maxSizeDimension = 0;
let sizeDimension = 1;
let minSizeDimension = 4;

let log = console.log;

initialize();

function initialize() {
  player = new RectangleBody(playerNormalWidth, playerNormalHeight);
  player.position = [75, 75];
  player.color = '#f00';
  updatePlayerSize();
  bodies.push(player);

  createWorld();

  run();
}

function createWorld() {
  addRectangleRock(160, 175, 40, 40);
  addRectangleRock(200, 175, 40, 40);
  addRectangleRock(240, 165, 40, 60);
  addRectangleRock(375, 175, 80, 80);
  addRectangleRock(600, 175, 110, 110);

  varyingSizeBody = addRectangleRock(600, 425, 110, 10);

  addCircleRock(200, 425, 20);
  addCircleRock(375, 425, 40);
  // addCircleRock(600, 425, 55);
}

function addRectangleRock(x, y, width, height) {
  let rock = new RectangleBody(width, height);
  rock.fixed = true;
  rock.color = '#888';
  rock.position = [x, y];
  bodies.push(rock);
  return rock;
}

function addCircleRock(x, y, radius) {
  let rock = new CircleBody(radius);
  rock.fixed = true;
  rock.color = '#888';
  rock.position = [x, y];
  bodies.push(rock);
  return rock;
}

function run() {
  requestAnimationFrame(run);

  let time = Date.now();
  let dt = (time - lastTime) / 1000;
  lastTime = time;

  update(dt);
  render();
}

function update(dt) {
  let direction = [0, 0];
  if (pressed[keyCode['Left']]) {
    direction[0] -= 1;
  }
  if (pressed[keyCode['Right']]) {
    direction[0] += 1;
  }
  if (pressed[keyCode['Up']]) {
    direction[1] -= 1;
  }
  player.velocity[0] = 0;
  if (magnitude(direction)) {
    direction = normalize(direction);
    player.velocity[0] = direction[0] * speed;
    if (direction[1]) {
      player.velocity[1] = direction[1] * speed;
    }
  }
  player.acceleration[1] = 1000;

  varyingSizeBody.width = 150 + Math.cos(Date.now() * 0.001 + 1) * 50;
  varyingSizeBody.height = 50 + Math.sin(Date.now() * 0.02) * 25;

  for (let i = 0; i < bodies.length; ++i) {
    bodies[i].update(dt);
  }

  handleCollisions();
}

function handleCollisions() {
  for (let i = 0; i < bodies.length; ++i) {
    bodies[i].collides = false;
  }

  for (let i = 0; i < bodies.length; ++i) {
    let b1 = bodies[i];
    for (let j = i + 1; j < bodies.length; ++j) {
      let b2 = bodies[j];

      if (b1.collidesWith(b2) && !(b1.fixed && b2.fixed)) {
        b1.collides = true;
        b2.collides = true;
        resolveCollision(b1, b2);
      }
    }
  }
}

function resolveCollision(b1, b2) {
  // We only handle rectangles for now.
  if (!(b1 instanceof RectangleBody) ||
      !(b2 instanceof RectangleBody)) {
    return;
  }

  let left1 = b1.position[0] - b1.width / 2;
  let right1 = b1.position[0] + b1.width / 2;
  let top1 = b1.position[1] - b1.height / 2;
  let bottom1 = b1.position[1] + b1.height / 2;

  let left2 = b2.position[0] - b2.width / 2;
  let right2 = b2.position[0] + b2.width / 2;
  let top2 = b2.position[1] - b2.height / 2;
  let bottom2 = b2.position[1] + b2.height / 2;

  if (magnitude(b2.velocity)) {
    let tmp = b1;
    b1 = b2;
    b2 = tmp;
  }

  let overlapX = Math.min(right1 - left2, right2 - left1);
  let overlapY = Math.min(bottom1 - top2, bottom2 - top1);

  var bounce = 0.0;

  if (overlapX <= overlapY) {
    if (b1.position[0] < b2.position[0]) {
      b1.position[0] -= overlapX;
      if (b1.velocity[0] > 0) {
        b1.velocity[0] *= -bounce;
      }
    } else {
      b1.position[0] += overlapX;
      if (b1.velocity[0] < 0) {
        b1.velocity[0] *= -bounce;
      }
    }
  } else {
    if (b1.position[1] < b2.position[1]) {
      b1.position[1] -= overlapY;
      if (b1.velocity[1] > 0) {
        b1.velocity[1] *= -bounce;
      }
    } else {
      b1.position[1] += overlapY;
      if (b1.velocity[1] < 0) {
        b1.velocity[1] *= -bounce;
      }
    }
  }
}

function render() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  for (let i = 0; i < bodies.length; ++i) {
    renderBody(bodies[i]);
  }
}

function renderBody(body) {
  if (body.collides) {
    ctx.fillStyle = '#00f';;
  } else {
    ctx.fillStyle = body.color;
  }
  if (body instanceof RectangleBody) {
    ctx.fillRect(body.position[0] - body.width / 2,
                 body.position[1] - body.height / 2,
                 body.width, body.height);
  } else if (body instanceof CircleBody) {
    ctx.beginPath();
    ctx.arc(body.position[0], body.position[1],
            body.radius, 0, 2 * Math.PI);
    ctx.closePath();
    ctx.fill();
  }
}

function updatePlayerSize() {
  player.width = Math.pow(2, -sizeDimension) * playerNormalWidth;
  player.height = Math.pow(2, -sizeDimension) * playerNormalHeight;
}

function playerCollidesWithWorld() {
  for (let i = 0; i < bodies.length; ++i) {
    if (player !== bodies[i] && player.collidesWith(bodies[i])) {
      return true;
    }
  }
  return false;
}

window.addEventListener('keydown', function(e) {
  pressed[e.keyCode] = true;

  if (pressed[keyCode['Q']]) {
    ++sizeDimension;
    sizeDimension = Math.min(sizeDimension, minSizeDimension);
    // There is nothing preventing you from shrinking.
    updatePlayerSize();
  }
  if (pressed[keyCode['E']]) {
    --sizeDimension;
    sizeDimension = Math.max(sizeDimension, maxSizeDimension);
    updatePlayerSize();
    if (playerCollidesWithWorld()) {
      ++sizeDimension;
      updatePlayerSize();
    }
  }
});

window.addEventListener('keyup', function(e) {
  pressed[e.keyCode] = false;
});
