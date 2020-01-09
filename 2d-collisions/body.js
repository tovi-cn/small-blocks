// Abstract class
class Body {
  constructor() {
    this.fixed = false;

    this.position = [0, 0];
    this.velocity = [0, 0];
    this.acceleration = [0, 0];

    this.collides = false;

    this.color = '#000';
  }

  update(dt) {
    this.velocity = add(this.velocity, scale(this.acceleration, dt));
    this.position = add(this.position, scale(this.velocity, dt));
  }

  collidesWith(body) {
    return false;
  }
}

class PointBody extends Body {
  constructor() {
    super();
  }

  collidesWith(body) {
    if (body instanceof PointBody) {
      return equals(this.position, body.position);
    } else if (body instanceof RectangleBody) {
      return !(this.x < body.position[0] - body.width / 2 ||
               this.x > body.position[0] + body.width / 2 ||
               this.y < body.position[1] - body.height / 2 ||
               this.y > body.position[1] + body.height / 2);
    } else if (body instanceof CircleBody) {
      let diff = sub(this.position, body.position);
      let dist = magnitude(diff);
      return dist <= body.radius;
    }
  }
}

class RectangleBody extends Body {
  constructor(width, height) {
    super();
    this.width = width;
    this.height = height;
  }

  collidesWith(body) {
    if (body instanceof PointBody) {
      return body.collidesWith(this);
    } else if (body instanceof RectangleBody) {
      let x1 = this.position[0];
      let y1 = this.position[1];
      let w1 = this.width;
      let h1 = this.height;

      let x2 = body.position[0];
      let y2 = body.position[1];
      let w2 = body.width;
      let h2 = body.height;

      return !(x1 + w1 / 2 <= x2 - w2 / 2 ||
               x1 - w1 / 2 >= x2 + w2 / 2 ||
               y1 + h1 / 2 <= y2 - h2 / 2 ||
               y1 - h1 / 2 >= y2 + h2 / 2);
    } else if (body instanceof CircleBody) {
      return body.collidesWith(this);
    }
  }
}

class CircleBody extends Body {
  constructor(radius) {
    super();
    this.radius = radius;
  }

  collidesWith(body) {
    if (body instanceof PointBody) {
      return body.collidesWith(this);
    } else if (body instanceof RectangleBody) {
      // Find the closest point to the circle within the rectangle
      let closestX = clamp(this.position[0],
                           body.position[0] - body.width / 2,
                           body.position[0] + body.width / 2);
      let closestY = clamp(this.position[1],
                           body.position[1] - body.height / 2,
                           body.position[1] + body.height / 2);

      // Calculate the distance between the circle's center and this
      // closest point
      let distanceX = this.position[0] - closestX;
      let distanceY = this.position[1] - closestY;

      // If the distance is less than the circle's radius, an
      // intersection occurs
      let distanceSquared = distanceX * distanceX + distanceY * distanceY;

      return distanceSquared < this.radius * this.radius;
    } else if (body instanceof CircleBody) {
      let diff = sub(this.position, body.position);
      let dist = magnitude(diff);
      return dist < this.radius + body.radius;
    }
  }
}
