// Arena-based Gladiator mode inspired by Pizza 1's wave survival stages.
(() => {
  const { Engine, Bodies, Body, Composite, Events } = Matter;

  /**
   * Clamp helper used by the camera to stay within the arena bounds.
   */
  function clamp(value, min, max) {
    // Return the minimum when the value dips below the allowed range.
    if (value < min) {
      return min;
    }
    // Return the maximum when the value exceeds the allowed range.
    if (value > max) {
      return max;
    }
    return value;
  }

  /**
   * Convert the pizza's body radius to a ground contact threshold.
   */
  function contactGroundThreshold(body, radius) {
    // Guard against missing bodies so the caller cannot crash during early setup.
    if (!body) {
      return radius;
    }
    return body.position.y + radius * 0.35;
  }

  // Physics tuning for the arena mode.
  const physicsSettings = {
    rollForce: 0.0035,
    jumpSpeed: 10.5,
    maxSpeed: 4.5,
    gravity: 0.1,
    maxSpin: 4.2,
    slamMultiplier: 1.4
  };

  // Arena layout settings.
  const arena = {
    width: 1600,
    height: 340,
    floorY: 250,
    wallThickness: 220
  };

  // Platforms that give the arena some vertical variety.
  const arenaPlatforms = [
    { x: arena.width * 0.32, y: arena.floorY - 90, width: 180, height: 18 },
    { x: arena.width * 0.68, y: arena.floorY - 130, width: 180, height: 18 }
  ];

  // Camera that tracks the player across the arena.
  const camera = { x: 0 };

  // Player state for Gladiator mode.
  const pizza = {
    x: 180,
    y: 120,
    radius: 32,
    grounded: false,
    invincibleTimer: 0,
    health: 3,
    angle: 0
  };

  // Local particles for small dust puffs on skeleton defeats.
  const particles = [];

  // Skeleton roster for the current wave.
  const skeletons = [];

  // Sequence of waves to clear.
  const wavePlan = [
    { count: 3, speed: 65 },
    { count: 5, speed: 75 },
    { count: 7, speed: 85 }
  ];

  let engine = null;
  let physicsWorld = null;
  let pizzaBody = null;
  const groundedContacts = new Set();
  let waveIndex = 0;
  let betweenWaveTimer = 0.8;
  let state = 'intro'; // intro -> fighting -> between -> victory/defeat
  let defeatTimer = 0;
  let victoryTimer = 0;
  let pendingAction = null;
  let airborneTimer = 0;
  let groundedCoyoteTimer = 0;
  let jumpQueued = false;
  let lastJumpPressed = false;
  let playSound = () => {};

  // Shared collision helper reused from the platformer enemy system when available.
  const circleRectContact =
    (window.EnemySystem && window.EnemySystem.circleRectContact) ||
    function fallbackCircleRectContact(circle, rect) {
      // Calculate the clamped point on the rectangle closest to the circle center.
      const closestX = Math.max(rect.x, Math.min(circle.x, rect.x + rect.width));
      const closestY = Math.max(rect.y, Math.min(circle.y, rect.y + rect.height));
      const dx = circle.x - closestX;
      const dy = circle.y - closestY;
      const distanceSq = dx * dx + dy * dy;
      const distance = Math.sqrt(distanceSq);
      return {
        overlap: distanceSq < circle.radius * circle.radius,
        closestX,
        closestY,
        dx,
        dy,
        distance
      };
    };

  /**
   * Build the Matter world for the arena once and reuse it across runs.
   */
  function ensureEngine() {
    // Avoid rebuilding when the engine already exists.
    if (engine) {
      return;
    }

    engine = Engine.create({ enableSleeping: false });
    engine.gravity.y = physicsSettings.gravity;
    engine.gravity.scale = 0.01;
    physicsWorld = engine.world;

    // Create the pizza body inside the arena so it can roll around.
    pizzaBody = Bodies.circle(pizza.x, pizza.y, pizza.radius, {
      label: 'gladiator-pizza',
      restitution: 0.0,
      friction: 0.35,
      frictionStatic: 1.2,
      frictionAir: 0.02,
      density: 0.004
    });

    // Build the static floor and boundary walls.
    const floor = Bodies.rectangle(
      arena.width * 0.5,
      arena.floorY + arena.wallThickness * 0.5,
      arena.width + arena.wallThickness * 2,
      arena.wallThickness,
      { isStatic: true, friction: 1, restitution: 0.0 }
    );
    const leftWall = Bodies.rectangle(
      -arena.wallThickness * 0.5,
      arena.height * 0.5,
      arena.wallThickness,
      arena.height * 2,
      { isStatic: true }
    );
    const rightWall = Bodies.rectangle(
      arena.width + arena.wallThickness * 0.5,
      arena.height * 0.5,
      arena.wallThickness,
      arena.height * 2,
      { isStatic: true }
    );

    // Translate platform definitions into static bodies.
    const platformBodies = arenaPlatforms.map((platform) => {
      // Center the rectangle so Matter can place it correctly.
      const body = Bodies.rectangle(
        platform.x + platform.width * 0.5,
        platform.y + platform.height * 0.5,
        platform.width,
        platform.height,
        { isStatic: true, friction: 1, restitution: 0.0 }
      );
      return { ...platform, body };
    });

    Composite.add(physicsWorld, [pizzaBody, floor, leftWall, rightWall, ...platformBodies.map((p) => p.body)]);

    /**
     * Track contacts under the pizza to mark grounded frames.
     */
    function trackGrounding(pairs) {
      // Loop over all collision pairs reported by Matter.
      for (const pair of pairs) {
        // Skip invalid collision pairs so the loop stays safe.
        if (!pair || !pair.isActive) {
          continue;
        }
        // Ignore collisions that do not involve the pizza.
        const involvesPizza = pair.bodyA === pizzaBody || pair.bodyB === pizzaBody;
        if (!involvesPizza) {
          continue;
        }
        // Fetch the opposing body to record a grounding contact.
        const other = pair.bodyA === pizzaBody ? pair.bodyB : pair.bodyA;
        if (!other || other.isSensor) {
          continue;
        }

        // Safely read the support points from the collision.
        const supports = Array.isArray(pair.collision && pair.collision.supports)
          ? pair.collision.supports
          : [];
        let supportY = -Infinity;
        // Scan each support point to find the highest contact under the pizza.
        for (const point of supports) {
          // Only process valid points to avoid NaN checks.
          if (point && typeof point.y === 'number') {
            supportY = Math.max(supportY, point.y);
          }
        }
        const groundThreshold = contactGroundThreshold(pizzaBody, pizza.radius);

        // Extract the normal so we can treat upward normals as ground.
        const normal = (pair.collision && pair.collision.normal) || { x: 0, y: 0 };
        const pizzaIsA = pair.bodyA === pizzaBody;
        const normalY = pizzaIsA ? -normal.y : normal.y;

        // Count the body as ground when the support point sits below the threshold or the normal points upward.
        if (supportY >= groundThreshold || normalY < -0.35) {
          groundedContacts.add(other.id);
          pizza.grounded = true;
        }
      }
    }

    // Respond to new contacts to catch landing frames.
    Events.on(engine, 'collisionStart', (event) => {
      trackGrounding(event.pairs);
    });

    // Refresh contacts each frame to keep grounding accurate.
    Events.on(engine, 'collisionActive', (event) => {
      groundedContacts.clear();
      trackGrounding(event.pairs);
      pizza.grounded = groundedContacts.size > 0;
    });

    // Remove contacts when bodies separate to release the grounded flag.
    Events.on(engine, 'collisionEnd', (event) => {
      // Loop through all separating pairs to prune contact ids.
      for (const pair of event.pairs) {
        // Skip pairs unrelated to the pizza.
        const involvesPizza = pair.bodyA === pizzaBody || pair.bodyB === pizzaBody;
        if (!involvesPizza) {
          continue;
        }
        const other = pair.bodyA === pizzaBody ? pair.bodyB : pair.bodyA;
        // Only remove valid ids to avoid deleting undefined keys.
        if (other && typeof other.id !== 'undefined') {
          groundedContacts.delete(other.id);
        }
      }
      pizza.grounded = groundedContacts.size > 0;
    });
  }

  /**
   * Reset the pizza body and timers for a fresh attempt.
   */
  function resetPizzaState() {
    Body.setPosition(pizzaBody, { x: 180, y: 120 });
    Body.setVelocity(pizzaBody, { x: 0, y: 0 });
    Body.setAngle(pizzaBody, 0);
    Body.setAngularVelocity(pizzaBody, 0);
    groundedContacts.clear();
    pizza.grounded = false;
    pizza.invincibleTimer = 0;
    pizza.health = 3;
    pizza.angle = 0;
    airborneTimer = 0;
    groundedCoyoteTimer = 0;
    jumpQueued = false;
    lastJumpPressed = false;
  }

  /**
   * Clear enemies, reset wave flow, and respawn the pizza.
   */
  function resetArena() {
    waveIndex = 0;
    betweenWaveTimer = 0.8;
    state = 'intro';
    defeatTimer = 0;
    victoryTimer = 0;
    pendingAction = null;
    skeletons.length = 0;
    particles.length = 0;
    resetPizzaState();
  }

  /**
   * Construct a patrolling skeleton with arena-specific bounds.
   */
  function createSkeleton(spawnX, speed) {
    return {
      x: spawnX,
      y: arena.floorY - 76,
      width: 46,
      height: 76,
      dir: spawnX < arena.width * 0.5 ? 1 : -1,
      speed,
      minX: 80,
      maxX: arena.width - 80,
      alive: true,
      hitTimer: 0
    };
  }

  /**
   * Spawn a wave of skeletons using the configured plan.
   */
  function spawnWave(index) {
    skeletons.length = 0;
    const def = wavePlan[index] || wavePlan[wavePlan.length - 1];
    // Loop for the requested number of enemies in this wave.
    for (let i = 0; i < def.count; i++) {
      // Alternate sides so the pizza gets pinched from both directions.
      const spawnLeft = i % 2 === 0;
      const jitter = Math.random() * 80;
      const baseX = spawnLeft ? 120 + jitter : arena.width - 120 - jitter;
      skeletons.push(createSkeleton(baseX, def.speed + Math.random() * 10));
    }
  }

  /**
   * Add a burst of small particles when an enemy is defeated.
   */
  function spawnParticles(x, y) {
    // Create a handful of squares to keep the effect light.
    for (let i = 0; i < 10; i++) {
      particles.push({
        x,
        y,
        vx: (Math.random() - 0.5) * 200,
        vy: -Math.random() * 240,
        life: 0.4 + Math.random() * 0.3,
        color: i % 2 ? '#fbe7c4' : '#dcb48f'
      });
    }
  }

  /**
   * Advance particle motion and prune expired entries.
   */
  function updateParticles(dt) {
    // Iterate backwards so removals do not affect later indices.
    for (let i = particles.length - 1; i >= 0; i--) {
      const p = particles[i];
      p.life -= dt;
      // Remove particles that have faded out.
      if (p.life <= 0) {
        particles.splice(i, 1);
        continue;
      }
      p.vy += arena.height * 0.5 * dt;
      p.x += p.vx * dt;
      p.y += p.vy * dt;
    }
  }

  /**
   * Handle jump input edge detection so held keys do not spam slams.
   */
  function updateJumpQueue(keys) {
    // Only queue on a fresh press so the jump is not repeated.
    if (keys.jump && !lastJumpPressed) {
      jumpQueued = true;
    }
    lastJumpPressed = !!keys.jump;
  }

  /**
   * Apply movement, jumping, and physics updates to the pizza.
   */
  function updatePizza(dt, keys) {
    pizza.invincibleTimer = Math.max(0, pizza.invincibleTimer - dt);
    const isPhysicallyGrounded = groundedContacts.size > 0;
    groundedCoyoteTimer = isPhysicallyGrounded ? 0.18 : Math.max(0, groundedCoyoteTimer - dt);
    pizza.grounded = groundedCoyoteTimer > 0;

    // Track air time to gate slam inputs until the pizza is airborne.
    if (isPhysicallyGrounded) {
      airborneTimer = 0;
    } else {
      airborneTimer += dt;
    }

    const rollDir = (keys.left ? -1 : 0) + (keys.right ? 1 : 0);
    // Apply roll force when the player presses left or right.
    if (rollDir !== 0) {
      const rollForce = rollDir * physicsSettings.rollForce * pizzaBody.mass;
      Body.applyForce(pizzaBody, pizzaBody.position, { x: rollForce, y: 0 });
    }

    const clampedSpeed = Math.max(-physicsSettings.maxSpeed, Math.min(physicsSettings.maxSpeed, pizzaBody.velocity.x));
    // Clamp excessive horizontal speed so the pizza remains controllable.
    if (clampedSpeed !== pizzaBody.velocity.x) {
      Body.setVelocity(pizzaBody, { x: clampedSpeed, y: pizzaBody.velocity.y });
    }

    // Fire the queued jump or slam depending on the pizza's grounded state.
    if (jumpQueued && pizza.grounded) {
      Body.setVelocity(pizzaBody, { x: pizzaBody.velocity.x, y: -physicsSettings.jumpSpeed });
      jumpQueued = false;
      playSound('jump');
    } else if (jumpQueued && !pizza.grounded && airborneTimer > 0.25) {
      Body.setVelocity(pizzaBody, {
        x: pizzaBody.velocity.x,
        y: physicsSettings.jumpSpeed * physicsSettings.slamMultiplier
      });
      jumpQueued = false;
      playSound('jump');
    } else if (jumpQueued && !pizza.grounded) {
      // Clear queued jumps that occur too early in the air to avoid double slams.
      jumpQueued = false;
    }

    Engine.update(engine, dt * 1000);

    const clampedSpin = Math.max(-physicsSettings.maxSpin, Math.min(physicsSettings.maxSpin, pizzaBody.angularVelocity));
    // Limit spin so the pizza art remains readable.
    if (clampedSpin !== pizzaBody.angularVelocity) {
      Body.setAngularVelocity(pizzaBody, clampedSpin);
    }

    pizza.x = pizzaBody.position.x;
    pizza.y = pizzaBody.position.y;
    pizza.angle = pizzaBody.angle;
  }

  /**
   * Damage the pizza and handle defeat transitions.
   */
  function damagePizza(knockbackX, knockbackY) {
    // Ignore hits while invincible to prevent rapid health loss.
    if (pizza.invincibleTimer > 0) {
      return;
    }
    pizza.health -= 1;
    pizza.invincibleTimer = 1.2;
    Body.setVelocity(pizzaBody, {
      x: pizzaBody.velocity.x + knockbackX,
      y: pizzaBody.velocity.y + knockbackY
    });
    playSound('hurt');
    // Trigger defeat when health reaches zero.
    if (pizza.health <= 0) {
      state = 'defeat';
      defeatTimer = 1.2;
    }
  }

  /**
   * Update skeleton patrols, handle collisions, and detect stomp kills.
   */
  function updateSkeletons(dt) {
    // Skip processing when the fight is paused between waves or already finished.
    if (state !== 'fighting') {
      return;
    }
    // Step through each skeleton to move and resolve interactions.
    for (const skeleton of skeletons) {
      // Decay hit timers for defeated skeletons.
      if (!skeleton.alive) {
        skeleton.hitTimer = Math.max(0, skeleton.hitTimer - dt);
        continue;
      }
      skeleton.x += skeleton.dir * skeleton.speed * dt;
      // Reverse direction when hitting the left patrol bound.
      if (skeleton.x < skeleton.minX) {
        skeleton.x = skeleton.minX;
        skeleton.dir = 1;
      } else if (skeleton.x + skeleton.width > skeleton.maxX) {
        // Reverse direction when hitting the right patrol bound.
        skeleton.x = skeleton.maxX - skeleton.width;
        skeleton.dir = -1;
      }

      const contact = circleRectContact(pizza, skeleton);
      // Only resolve when the pizza and skeleton overlap.
      if (contact.overlap) {
        const downwardDot = contact.distance > 0 ? -contact.dy / contact.distance : 1;
        const hittingFromAbove = downwardDot > 0.6;
        // Kill the skeleton when the pizza stomps from above.
        if (hittingFromAbove && pizza.invincibleTimer <= 0) {
          skeleton.alive = false;
          skeleton.hitTimer = 0.6;
          Body.setVelocity(pizzaBody, {
            x: pizzaBody.velocity.x,
            y: -physicsSettings.jumpSpeed * 0.7
          });
          playSound('enemy');
          spawnParticles(skeleton.x + skeleton.width * 0.5, skeleton.y + skeleton.height * 0.5);
        } else if (pizza.invincibleTimer <= 0) {
          // Hurt the pizza when colliding from the side.
          const awayX = pizza.x - contact.closestX;
          const awayY = pizza.y - contact.closestY;
          const awayLen = Math.hypot(awayX, awayY) || 1;
          damagePizza((awayX / awayLen) * 6, (awayY / awayLen) * 6 - physicsSettings.jumpSpeed * 0.2);
        }
      }
    }
  }

  /**
   * Determine whether to spawn new waves, restart, or exit to the overworld.
   */
  function updateWaveFlow(dt) {
    // Count alive skeletons to know when a wave ends.
    let livingSkeletons = 0;
    for (const skeleton of skeletons) {
      // Increment for each living enemy.
      if (skeleton.alive) {
        livingSkeletons += 1;
      }
    }

    // Start the opening wave once the intro timer finishes.
    if (state === 'intro' && betweenWaveTimer <= 0) {
      state = 'fighting';
      spawnWave(waveIndex);
      playSound('coin');
    } else if (state === 'intro') {
      // Count down before spawning the first batch of skeletons.
      betweenWaveTimer = Math.max(0, betweenWaveTimer - dt);
    }

    // Transition to the next wave after clearing the current one.
    if (state === 'fighting' && livingSkeletons === 0) {
      // Move into the victory branch when the last wave ends.
      if (waveIndex >= wavePlan.length - 1) {
        state = 'victory';
        victoryTimer = 1.2;
        playSound('coin');
        pendingAction = { type: 'return-to-map' };
      } else {
        // Pause briefly before spawning the following wave.
        state = 'between';
        betweenWaveTimer = 1.2;
      }
    }

    // Start the next wave after a short breather.
    if (state === 'between') {
      betweenWaveTimer = Math.max(0, betweenWaveTimer - dt);
      // Begin the next wave once the pause elapses.
      if (betweenWaveTimer === 0) {
        waveIndex += 1;
        state = 'fighting';
        spawnWave(waveIndex);
        playSound('coin');
      }
    }

    // Restart the arena a moment after losing all health.
    if (state === 'defeat') {
      defeatTimer = Math.max(0, defeatTimer - dt);
      // Reset the whole encounter when the defeat timer expires.
      if (defeatTimer === 0) {
        resetArena();
      }
    }

    // Wait out the victory timer before handing control back to the overworld.
    if (state === 'victory') {
      victoryTimer = Math.max(0, victoryTimer - dt);
    }
  }

  /**
   * Ease the camera toward the pizza while clamping to the arena bounds.
   */
  function updateCamera() {
    const target = pizza.x - 256;
    const clampMax = arena.width - 512;
    const desired = clamp(target, 0, clampMax);
    camera.x += (desired - camera.x) * 0.14;
  }

  /**
   * Render the background, actors, and HUD for Gladiator mode.
   */
  function draw(ctx) {
    // Abort drawing when no context is provided.
    if (!ctx) {
      return;
    }

    const gradient = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);
    gradient.addColorStop(0, '#0e0b20');
    gradient.addColorStop(1, '#05040e');
    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    // Draw subtle spotlights on the arena floor.
    ctx.fillStyle = 'rgba(255,255,255,0.05)';
    for (let i = 0; i < 3; i++) {
      // Spread out soft lights so the arena has focal points.
      const lightX = (i + 1) * (ctx.canvas.width / 4);
      ctx.beginPath();
      ctx.ellipse(lightX, arena.floorY + 30, 180, 60, 0, 0, Math.PI * 2);
      ctx.fill();
    }

    ctx.save();
    ctx.translate(-camera.x, 0);
    ctx.fillStyle = '#2c2b45';
    ctx.strokeStyle = '#5c537f';
    ctx.lineWidth = 2;
    // Draw the floor strip across the arena.
    ctx.fillRect(0, arena.floorY, arena.width, arena.height - arena.floorY + 60);
    ctx.strokeRect(0, arena.floorY, arena.width, arena.height - arena.floorY + 60);

    // Render elevated platforms for extra movement options.
    for (const platform of arenaPlatforms) {
      ctx.fillRect(platform.x, platform.y, platform.width, platform.height);
      ctx.strokeRect(platform.x, platform.y, platform.width, platform.height);
    }

    // Render skeletons relative to the camera.
    for (const skeleton of skeletons) {
      const x = skeleton.x;
      // Draw solid bodies for living enemies.
      if (skeleton.alive) {
        ctx.fillStyle = '#d9d6e6';
        ctx.fillRect(x, skeleton.y, skeleton.width, skeleton.height);
        ctx.fillStyle = '#948daa';
        ctx.fillRect(x + 6, skeleton.y + 6, skeleton.width - 12, skeleton.height - 12);
        ctx.fillStyle = '#1f1c33';
        const eyeY = skeleton.y + 18;
        ctx.fillRect(x + 12, eyeY, 6, 6);
        ctx.fillRect(x + skeleton.width - 18, eyeY, 6, 6);
      } else if (skeleton.hitTimer > 0) {
        // Fade out recently defeated enemies.
        ctx.fillStyle = 'rgba(255,255,255,0.15)';
        ctx.fillRect(x, skeleton.y, skeleton.width, skeleton.height);
      }
    }

    // Render the pizza at its current rotation.
    const renderX = pizza.x;
    ctx.save();
    ctx.translate(renderX, pizza.y);
    ctx.rotate(pizza.angle);
    ctx.fillStyle = '#ffb347';
    ctx.beginPath();
    ctx.arc(0, 0, pizza.radius, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillStyle = '#d65a31';
    // Sprinkle pepperoni around the top.
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

    // Render dust particles from defeated enemies.
    for (const p of particles) {
      ctx.fillStyle = p.color;
      ctx.globalAlpha = Math.max(0, p.life / 0.6);
      ctx.fillRect(p.x, p.y, 4, 4);
      ctx.globalAlpha = 1;
    }

    ctx.restore();

    // Draw a HUD for wave and health tracking.
    ctx.save();
    ctx.fillStyle = 'rgba(0,0,0,0.35)';
    ctx.fillRect(10, 10, 200, 70);
    ctx.fillStyle = '#ff4d6d';
    for (let i = 0; i < pizza.health; i++) {
      // Draw one heart for each remaining hit point.
      ctx.fillRect(20 + i * 24, 20, 18, 18);
    }
    ctx.fillStyle = '#ffffff';
    ctx.font = '14px monospace';
    ctx.fillText(`Wave ${waveIndex + 1} / ${wavePlan.length}`, 20, 52);
    ctx.fillText('Stage: Gladiator', 20, 70);

    // Show contextual messaging during transitions.
    if (state === 'intro') {
      // Explain the goal while the first wave counts down.
      ctx.fillText('Survive the skeleton waves', 280, 36);
    } else if (state === 'between') {
      // Alert the player that more enemies are coming.
      ctx.fillText('Next wave incoming...', 280, 36);
    } else if (state === 'victory') {
      // Let the player know the arena will return to the map.
      ctx.fillText('All waves cleared! Returning...', 280, 36);
    } else if (state === 'defeat') {
      // Notify the player of an automatic retry after losing.
      ctx.fillText('You were shattered! Retrying...', 280, 36);
    }
    ctx.restore();
  }

  /**
   * Prepare the arena for play whenever the mode is entered.
   */
  function enter(playSoundHook) {
    playSound = typeof playSoundHook === 'function' ? playSoundHook : () => {};
    ensureEngine();
    resetArena();
  }

  /**
   * Advance simulation, handle input, and report actions to the host game.
   */
  function update(dt, keys, playSoundHook) {
    playSound = typeof playSoundHook === 'function' ? playSoundHook : playSound;
    const safeKeys = keys || {};
    updateJumpQueue(safeKeys);
    // Skip simulation if the engine is missing.
    if (!engine) {
      return null;
    }
    // Continue updating physics unless the mode is already celebrating.
    if (state !== 'victory') {
      updatePizza(dt, safeKeys);
      updateSkeletons(dt);
      updateParticles(dt);
      updateCamera();
    }
    updateWaveFlow(dt);
    const action = pendingAction;
    pendingAction = null;
    return action;
  }

  window.GladiatorMode = {
    enter,
    update,
    draw
  };
})();
