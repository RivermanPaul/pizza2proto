// Bounce mode rebuilt on Matter.js physics: collect coins while rebounding off pads and smashing skulls.
(() => {
  const { Engine, Bodies, Body, Composite, Events } = Matter;

  /**
   * Clamp a value within an inclusive range to keep coordinates safe.
   */
  function clamp(value, min, max) {
    // Return the lower bound when the value dips below it.
    if (value < min) {
      return min;
    }
    // Return the upper bound when the value exceeds it.
    if (value > max) {
      return max;
    }
    return value;
  }

  // Collision categories so we can ignore coin-shard contacts cleanly.
  const COLLISION = {
    PIZZA: 0x0001,
    PAD: 0x0002,
    SKULL: 0x0004,
    COIN: 0x0008,
    SHARD: 0x0010
  };

  // World and tuning values for the bounce arena.
  const world = {
    width: 1100,
    height: 360,
    gravity: 0.9,
    cameraLerp: 0.14
  };

  // Pre-build static pad definitions to mirror BOLevelEasy.
  const padDefs = [];
  const padWidth = 144;
  // Create a row of pads spaced like the Pizza 1 reference.
  for (let i = 0; i < 7; i++) {
    padDefs.push({ x: 72 + padWidth * i, y: 220, width: padWidth, height: 18 });
  }

  // Camera that follows the pizza across the stage.
  const camera = { x: 0 };

  // Game state containers.
  const coins = [];
  const skulls = [];
  const shards = [];
  const particles = [];

  // Player state mirrors the physics body but keeps lightweight stats.
  const pizza = {
    radius: 32,
    health: 3,
    grounded: false
  };

  // Timers and win tracking.
  const goalCoins = 10;
  let collectedCoins = 0;
  let coinTimer = 0.6;
  let skullTimer = 1.8;
  let airborneTimer = 0;
  let slamTimer = 0;
  let victoryTimer = 0;
  let pendingAction = null;
  let playSound = () => {};
  let lastJumpPressed = false;
  let jumpQueued = false;

  // Physics engine references.
  let engine = null;
  let bounceWorld = null;
  let pizzaBody = null;
  let padBodies = [];
  let walls = [];

  // Track grounded contacts for coyote timing.
  const groundedContacts = new Set();

  /**
   * Create the Matter world once and wire collision listeners.
   */
  function ensureEngine() {
    // Skip setup when the engine already exists.
    if (engine) {
      // Refresh surface materials so pads stay bouncy even across reloads.
      for (const pad of padBodies) {
        if (pad) {
          pad.restitution = 1;
          pad.friction = 0;
          pad.frictionStatic = 0;
          // Resize pads to the desired width so they touch edge-to-edge.
          const desiredWidth = padDefs[padBodies.indexOf(pad)]?.width || padWidth;
          const currentWidth = pad.bounds.max.x - pad.bounds.min.x;
          if (Math.abs(currentWidth - desiredWidth) > 0.5) {
            const scaleX = desiredWidth / currentWidth;
            Body.scale(pad, scaleX, 1);
          }
          // Re-center pads to match the pad definition positions.
          const def = padDefs[padBodies.indexOf(pad)];
          if (def) {
            Body.setPosition(pad, {
              x: def.x + def.width * 0.5,
              y: def.y + def.height * 0.5
            });
          }
        }
      }
      for (const wall of walls) {
        if (wall) {
          wall.restitution = 1;
          wall.friction = 0;
          wall.frictionStatic = 0;
        }
      }
      if (pizzaBody) {
        pizzaBody.restitution = 0.9;
        pizzaBody.friction = 0.05;
        pizzaBody.frictionStatic = 0.05;
        pizzaBody.frictionAir = 0.01;
      }
      return;
    }

    engine = Engine.create({ enableSleeping: false });
    engine.gravity.y = world.gravity;
    engine.gravity.scale = 0.002;
    bounceWorld = engine.world;

    // Build static pads and boundary walls.
    padBodies = padDefs.map((pad) =>
      Bodies.rectangle(
        pad.x + pad.width * 0.5,
        pad.y + pad.height * 0.5,
        pad.width,
        pad.height,
        {
          isStatic: true,
          label: 'pad',
          restitution: 1,
          friction: 0,
          frictionStatic: 0,
          collisionFilter: { category: COLLISION.PAD, mask: 0xffff }
        }
      )
    );

    walls = [
      Bodies.rectangle(-200, world.height * 0.5, 400, world.height * 4, {
        isStatic: true,
        label: 'wall',
        restitution: 1,
        friction: 0,
        frictionStatic: 0,
        collisionFilter: { category: COLLISION.PAD, mask: 0xffff }
      }),
      Bodies.rectangle(world.width + 200, world.height * 0.5, 400, world.height * 4, {
        isStatic: true,
        label: 'wall',
        restitution: 1,
        friction: 0,
        frictionStatic: 0,
        collisionFilter: { category: COLLISION.PAD, mask: 0xffff }
      }),
      Bodies.rectangle(world.width * 0.5, world.height + 200, world.width + 800, 400, {
        isStatic: true,
        label: 'floor',
        restitution: 1,
        friction: 0,
        frictionStatic: 0,
        collisionFilter: { category: COLLISION.PAD, mask: 0xffff }
      })
    ];

    // Create the pizza body used throughout bounce mode.
    pizzaBody = Bodies.circle(180, 120, pizza.radius, {
      label: 'pizza',
      friction: 0.05,
      frictionStatic: 0.05,
      frictionAir: 0.01,
      restitution: 0.9,
      density: 0.004,
      collisionFilter: { category: COLLISION.PIZZA, mask: 0xffff }
    });

    Composite.add(bounceWorld, [...padBodies, ...walls, pizzaBody]);

    /**
     * Track grounding contacts for coyote timing.
     */
    function trackGrounding(pairs) {
      // Loop each collision pair to find pizza contacts.
      for (const pair of pairs) {
        if (!pair || !pair.isActive) {
          continue; // Skip invalid pairs to stay safe.
        }
        const involvesPizza = pair.bodyA === pizzaBody || pair.bodyB === pizzaBody;
        if (!involvesPizza) {
          continue; // Ignore collisions that do not involve the pizza.
        }
        const other = pair.bodyA === pizzaBody ? pair.bodyB : pair.bodyA;
        if (!other || other.isSensor) {
          continue; // Skip sensor-only touches to avoid false grounding.
        }
        groundedContacts.add(other.id);
      }
    }

    Events.on(engine, 'collisionStart', (event) => {
      trackGrounding(event.pairs);
      handleContacts(event.pairs);
    });

    Events.on(engine, 'collisionActive', (event) => {
      groundedContacts.clear();
      trackGrounding(event.pairs);
    });

    Events.on(engine, 'collisionEnd', (event) => {
      // Remove ended contacts from the grounded list.
      for (const pair of event.pairs) {
        const involvesPizza = pair.bodyA === pizzaBody || pair.bodyB === pizzaBody;
        if (!involvesPizza) {
          continue;
        }
        const other = pair.bodyA === pizzaBody ? pair.bodyB : pair.bodyA;
        groundedContacts.delete(other.id);
      }
    });
  }

  /**
   * Handle collision-start events for coins and skull crush checks.
   */
  function handleContacts(pairs) {
    // Loop through each new collision to detect gameplay triggers.
    for (const pair of pairs) {
      if (!pair || !pair.isActive) {
        continue; // Skip invalid pairs to keep the handler safe.
      }
      const bodies = [pair.bodyA, pair.bodyB];
      const labels = bodies.map((b) => b && b.label);
      // Process coin pickups when the pizza touches a coin.
      if (labels.includes('pizza') && labels.includes('coin')) {
        const coinBody = bodies.find((b) => b.label === 'coin');
        collectCoin(coinBody);
        continue;
      }
      // Process skull crush attempts when the pizza hits a skull.
      if (labels.includes('pizza') && labels.includes('skull')) {
        const skullBody = bodies.find((b) => b.label === 'skull');
        tryCrushSkull(skullBody);
      }
    }
  }

  /**
   * Remove a body and its entry from a tracking array.
   */
  function removeTrackedBody(body, list) {
    // Find the index of the element that wraps this body.
    const index = list.findIndex((entry) => entry.body === body);
    if (index !== -1) {
      list.splice(index, 1);
    }
    if (body && bounceWorld) {
      Composite.remove(bounceWorld, body);
    }
  }

  /**
   * Collect a coin body and award score/progress.
   */
  function collectCoin(coinBody) {
    // Exit early when the coin was already removed.
    if (!coinBody) {
      return;
    }
    removeTrackedBody(coinBody, coins);
    collectedCoins += 1;
    spawnParticles(coinBody.position.x, coinBody.position.y);
    playSound('coin');
    // Trigger victory when the goal is reached.
    if (collectedCoins >= goalCoins && !pendingAction) {
      pendingAction = { type: 'return-to-map' };
      victoryTimer = 1.0;
    }
  }

  /**
   * Attempt to crush a skull when hit by the pizza with enough force.
   */
  function tryCrushSkull(skullBody) {
    // Skip when the skull body no longer exists.
    if (!skullBody) {
      return;
    }
    const pizzaPos = pizzaBody.position;
    const skullPos = skullBody.position;
    const hittingFromAbove = pizzaPos.y < skullPos.y - 8 && pizzaBody.velocity.y > 0;
    const heavyImpact = pizzaBody.velocity.y > 6 || slamTimer > 0;
    // Only crush when the pizza is descending hard enough.
    if (hittingFromAbove && heavyImpact) {
      removeTrackedBody(skullBody, skulls);
      playSound('enemy');
      spawnSkullFragments(skullPos.x, skullPos.y);
    }
  }

  /**
   * Reset timers and gameplay state for a new attempt.
   */
  function resetBounce() {
    // Ensure physics exists before clearing bodies.
    ensureEngine();
    // Remove all dynamic bodies from the world.
    for (const entry of coins) {
      Composite.remove(bounceWorld, entry.body);
    }
    for (const entry of skulls) {
      Composite.remove(bounceWorld, entry.body);
    }
    for (const entry of shards) {
      Composite.remove(bounceWorld, entry.body);
    }
    coins.length = 0;
    skulls.length = 0;
    shards.length = 0;
    particles.length = 0;
    collectedCoins = 0;
    coinTimer = 0.6;
    skullTimer = 1.8;
    victoryTimer = 0;
    pendingAction = null;
    pizza.health = 3;
    airborneTimer = 0;
    slamTimer = 0;
    groundedContacts.clear();
    // Reset the pizza pose and velocities.
    Body.setPosition(pizzaBody, { x: 180, y: 120 });
    Body.setVelocity(pizzaBody, { x: 0, y: 0 });
    Body.setAngle(pizzaBody, 0);
    Body.setAngularVelocity(pizzaBody, 0);
  }

  /**
   * Queue jump input on a rising edge so slams stay deliberate.
   */
  function updateJumpQueue(keys) {
    // Store a jump press when it transitions from unpressed to pressed.
    if (keys.jump && !lastJumpPressed) {
      jumpQueued = true;
    }
    // Track the last frame state for edge detection.
    lastJumpPressed = !!keys.jump;
  }

  /**
   * Spawn a coin body with slight horizontal velocity.
   */
  function spawnCoin() {
    // Select a pad so coins fall toward reachable spots.
    const pad = padDefs[Math.floor(Math.random() * padDefs.length)];
    if (!pad) {
      return; // Bail out if pads failed to generate.
    }
    const body = Bodies.circle(pad.x + pad.width * 0.5, 40, 10, {
      label: 'coin',
      restitution: 0.6,
      friction: 0.2,
      frictionAir: 0.01,
      collisionFilter: { category: COLLISION.COIN, mask: 0xffff ^ COLLISION.SHARD }
    });
    Body.setVelocity(body, { x: (Math.random() - 0.5) * 2.5, y: 0 });
    coins.push({ body });
    Composite.add(bounceWorld, body);
  }

  /**
   * Spawn a skull enemy that bounces around the pads.
   */
  function spawnSkull() {
    // Select a pad and jitter the spawn position.
    const pad = padDefs[Math.floor(Math.random() * padDefs.length)];
    if (!pad) {
      return; // Do nothing when no pads exist.
    }
    const body = Bodies.circle(pad.x + pad.width * 0.5 + (Math.random() - 0.5) * 80, 20, 22, {
      label: 'skull',
      restitution: 0.45,
      friction: 0.05,
      frictionAir: 0.015,
      density: 0.001,
      collisionFilter: { category: COLLISION.SKULL, mask: 0xffff }
    });
    Body.setVelocity(body, { x: (Math.random() - 0.5) * 3, y: 0 });
    skulls.push({ body });
    Composite.add(bounceWorld, body);
  }

  /**
   * Spawn shards as triangular physics bodies that bounce off everything but coins.
   */
  function spawnSkullFragments(x, y) {
    // Create eight shards with random spin and velocity.
    for (let i = 0; i < 8; i++) {
      const size = 9 + Math.random() * 6;
      const body = Bodies.polygon(x, y, 3, size, {
        label: 'shard',
        restitution: 0.25,
        friction: 0.15,
        frictionAir: 0.03,
        collisionFilter: {
          category: COLLISION.SHARD,
          // Collide with everything except coins by omitting the COIN bit.
          mask: COLLISION.PIZZA | COLLISION.PAD | COLLISION.SKULL | COLLISION.SHARD
        }
      });
      Body.setVelocity(body, {
        x: (Math.random() - 0.5) * 5,
        y: -3 - Math.random() * 2
      });
      Body.setAngularVelocity(body, (Math.random() - 0.5) * 6);
      shards.push({ body, life: 3.5 + Math.random() * 1.0, size });
      Composite.add(bounceWorld, body);
    }
  }

  /**
   * Spawn celebratory particles when a coin is grabbed.
   */
  function spawnParticles(x, y, lingering = false) {
    // Emit eight particles with randomized velocities.
    for (let i = 0; i < 8; i++) {
      particles.push({
        x,
        y,
        vx: (Math.random() - 0.5) * 140,
        vy: -Math.random() * 200,
        life: lingering ? 2.4 + Math.random() * 1.2 : 0.45 + Math.random() * 0.25,
        color: i % 2 ? '#ffd166' : '#ffe9a3'
      });
    }
  }

  /**
   * Remove expired particles each frame.
   */
  function updateParticles(dt) {
    // Iterate backward so removals do not affect later indices.
    for (let i = particles.length - 1; i >= 0; i--) {
      const p = particles[i];
      p.life -= dt;
      // Remove particles that have faded out.
      if (p.life <= 0) {
        particles.splice(i, 1);
        continue;
      }
      // Apply gravity to particles so they arc downward.
      p.vy += world.gravity * 200 * dt;
      p.x += p.vx * dt;
      p.y += p.vy * dt;
    }
  }

  /**
   * Despawn coins and skulls that leave the arena bounds.
   */
  function cullOutOfBounds() {
    // Remove coins that drift too far from the playfield.
    for (let i = coins.length - 1; i >= 0; i--) {
      const coin = coins[i];
      // Skip missing coin bodies so removal does not throw.
      if (!coin.body) {
        continue; // Skip missing bodies to stay safe.
      }
      const { x, y } = coin.body.position;
      // Remove the coin when it leaves the playable rectangle.
      if (x < -120 || x > world.width + 120 || y > world.height + 200 || y < -160) {
        removeTrackedBody(coin.body, coins);
      }
    }
    // Remove skulls that leave the arena.
    for (let i = skulls.length - 1; i >= 0; i--) {
      const skull = skulls[i];
      // Skip missing skull bodies so the cull loop stays safe.
      if (!skull.body) {
        continue; // Skip missing bodies to stay safe.
      }
      const { x, y } = skull.body.position;
      // Remove skulls that fall or drift outside the arena.
      if (x < -120 || x > world.width + 120 || y > world.height + 260 || y < -160) {
        removeTrackedBody(skull.body, skulls);
      }
    }
  }

  /**
   * Trim shards after their lifetime expires.
   */
  function cullShards(dt) {
    // Loop backwards so splicing does not skip entries.
    for (let i = shards.length - 1; i >= 0; i--) {
      const shard = shards[i];
      shard.life -= dt;
      // Remove the shard when its lifetime expires.
      if (shard.life <= 0) {
        removeTrackedBody(shard.body, shards);
      }
    }
  }

  /**
   * Apply input, slam logic, and time-step the physics engine.
   */
  function updatePhysics(dt, keys) {
    // Decay slam timer to limit crush windows.
    slamTimer = Math.max(0, slamTimer - dt);

    const isGrounded = groundedContacts.size > 0;
    pizza.grounded = isGrounded;
    // Track airborne time for slam gating.
    if (isGrounded) {
      airborneTimer = 0;
    } else {
      airborneTimer += dt;
    }

    const rollDir = (keys.left ? -1 : 0) + (keys.right ? 1 : 0);
    // Apply lateral force to roll the heavier pizza.
    if (rollDir !== 0) {
      Body.applyForce(pizzaBody, pizzaBody.position, { x: rollDir * 0.0036, y: 0 });
    }

    // Handle jump and slam based on grounded state.
    if (jumpQueued && isGrounded) {
      Body.setVelocity(pizzaBody, { x: pizzaBody.velocity.x, y: -10 });
      jumpQueued = false;
      playSound('jump');
    } else if (jumpQueued && !isGrounded && airborneTimer > 0.22) {
      Body.setVelocity(pizzaBody, { x: pizzaBody.velocity.x, y: 12 });
      slamTimer = 0.35;
      jumpQueued = false;
      playSound('jump');
    } else if (jumpQueued && !isGrounded) {
      jumpQueued = false;
    }

    // Step the physics world.
    Engine.update(engine, dt * 1000);
  }

  /**
   * Update spawn timers for coins and skulls.
   */
  function updateSpawns(dt) {
    // Count down to the next coin.
    coinTimer -= dt;
    if (coinTimer <= 0) {
      spawnCoin();
      coinTimer = 0.9 + Math.random() * 0.6;
    }
    // Count down to the next skull.
    skullTimer -= dt;
    if (skullTimer <= 0) {
      spawnSkull();
      skullTimer = 1.6 + Math.random() * 0.8;
    }
  }

  /**
   * Update camera to follow the pizza smoothly.
   */
  function updateCamera(viewWidth) {
    const target = pizzaBody.position.x - viewWidth * 0.5;
    const maxX = Math.max(0, world.width - viewWidth);
    const desired = clamp(target, 0, maxX);
    camera.x += (desired - camera.x) * world.cameraLerp;
  }

  /**
   * Render the current bounce scene.
   */
  function draw(ctx) {
    // Abort drawing when no context exists.
    if (!ctx) {
      return;
    }

    const gradient = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);
    gradient.addColorStop(0, '#0d0b1f');
    gradient.addColorStop(1, '#05030a');
    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    ctx.save();
    ctx.translate(-camera.x, 0);

    // Draw pads as solid slabs for clear landing spots.
    ctx.fillStyle = '#2c2b45';
    ctx.strokeStyle = '#5c537f';
    ctx.lineWidth = 2;
    for (const pad of padDefs) {
      ctx.fillRect(pad.x, pad.y, pad.width, pad.height);
      ctx.strokeRect(pad.x, pad.y, pad.width, pad.height);
    }

    // Draw each active coin with a gold fill.
    for (const coin of coins) {
      // Skip coins lacking a body to avoid crashes.
      if (!coin.body) {
        continue;
      }
      ctx.fillStyle = '#ffd166';
      ctx.beginPath();
      ctx.arc(coin.body.position.x, coin.body.position.y, 10, 0, Math.PI * 2);
      ctx.fill();
      ctx.strokeStyle = '#fcefb4';
      ctx.lineWidth = 2;
      ctx.stroke();
    }

    // Draw skulls as round targets that bounce unless crushed.
    for (const skull of skulls) {
      if (!skull.body) {
        continue; // Skip removed skulls.
      }
      ctx.fillStyle = '#e6e6e6';
      ctx.beginPath();
      ctx.arc(skull.body.position.x, skull.body.position.y, 22, 0, Math.PI * 2);
      ctx.fill();
      ctx.fillStyle = '#262238';
      ctx.beginPath();
      ctx.arc(skull.body.position.x - 6, skull.body.position.y - 4, 4, 0, Math.PI * 2);
      ctx.fill();
      ctx.beginPath();
      ctx.arc(skull.body.position.x + 6, skull.body.position.y - 4, 4, 0, Math.PI * 2);
      ctx.fill();
    }

    // Draw each particle with fading alpha.
    for (const p of particles) {
      ctx.fillStyle = p.color;
      ctx.globalAlpha = Math.max(0, p.life / 0.6);
      ctx.fillRect(p.x, p.y, 4, 4);
      ctx.globalAlpha = 1;
    }

    // Draw each shard as a small triangle spinning through the air.
    for (const shard of shards) {
      if (!shard.body) {
        continue; // Skip removed shards.
      }
      ctx.save();
      ctx.translate(shard.body.position.x, shard.body.position.y);
      ctx.rotate(shard.body.angle);
      ctx.fillStyle = '#fcefb4';
      ctx.beginPath();
      ctx.moveTo(-shard.size, shard.size);
      ctx.lineTo(shard.size, shard.size);
      ctx.lineTo(0, -shard.size);
      ctx.closePath();
      ctx.fill();
      ctx.restore();
    }

    // Draw the bouncing pizza.
    ctx.save();
    ctx.translate(pizzaBody.position.x, pizzaBody.position.y);
    ctx.fillStyle = '#ffb347';
    ctx.beginPath();
    ctx.arc(0, 0, pizza.radius, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillStyle = '#d65a31';
    // Scatter pepperoni slices around the pizza.
    for (let i = 0; i < 12; i++) {
      const angle = (Math.PI * 2 * i) / 12;
      const dotX = Math.cos(angle) * pizza.radius * 0.6;
      const dotY = Math.sin(angle) * pizza.radius * 0.6;
      ctx.beginPath();
      ctx.arc(dotX, dotY, 5, 0, Math.PI * 2);
      ctx.fill();
    }
    ctx.strokeStyle = '#f7d08e';
    ctx.lineWidth = 6;
    ctx.beginPath();
    ctx.arc(0, 0, pizza.radius - 6, 0, Math.PI * 2);
    ctx.stroke();
    ctx.restore();

    ctx.restore();

    // Draw HUD elements for coins and health.
    ctx.save();
    ctx.fillStyle = 'rgba(0,0,0,0.35)';
    ctx.fillRect(10, 10, 200, 70);
    ctx.fillStyle = '#ff4d6d';
    for (let i = 0; i < pizza.health; i++) {
      ctx.fillRect(20 + i * 24, 20, 18, 18);
    }
    ctx.fillStyle = '#ffffff';
    ctx.font = '14px monospace';
    ctx.fillText(`Coins: ${collectedCoins}/${goalCoins}`, 20, 52);
    ctx.fillText('Stage: Bounce', 20, 70);
    ctx.restore();
  }

  /**
   * Initialize the bounce mode when entering from the overworld.
   */
  function enter(playSoundHook) {
    playSound = typeof playSoundHook === 'function' ? playSoundHook : () => {};
    resetBounce();
  }

  /**
   * Advance gameplay and return an action when the mode is complete.
   */
  function update(dt, keys, playSoundHook) {
    playSound = typeof playSoundHook === 'function' ? playSoundHook : playSound;
    const safeKeys = keys || {};
    ensureEngine();
    updateJumpQueue(safeKeys);
    // Stop simulation during victory countdowns.
    if (victoryTimer > 0) {
      victoryTimer = Math.max(0, victoryTimer - dt);
      const action = pendingAction;
      pendingAction = null;
      return action;
    }
    updatePhysics(dt, safeKeys);
    updateSpawns(dt);
    cullOutOfBounds();
    cullShards(dt);
    updateParticles(dt);
    updateCamera(512);
    // Handle death when falling out of the arena.
    if (pizzaBody.position.y > world.height + 240) {
      pizza.health -= 1;
      playSound('hurt');
      if (pizza.health <= 0) {
        resetBounce();
      } else {
        Body.setPosition(pizzaBody, { x: 180, y: 120 });
        Body.setVelocity(pizzaBody, { x: 0, y: 0 });
      }
    }
    const action = pendingAction;
    pendingAction = null;
    return action;
  }

  window.BounceMode = {
    enter,
    update,
    draw
  };
})();
