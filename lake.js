// Underwater lake mode where the pizza swims and grows by eating smaller fish.
(() => {
  // Clamp helper to keep coordinates within the stage bounds.
  function clamp(value, min, max) {
    // Restrict values so actors never leave the playable rectangle.
    if (value < min) return min; // Prevent values from dipping below the lower bound.
    if (value > max) return max; // Prevent values from exceeding the upper bound.
    return value;
  }

  // Translate a size category into a rendered radius.
  function sizeToRadius(size) {
    // Start smaller than the platformer pizza and ramp size steadily.
    return 8 + size * 4;
  }

  // Track all state for the player-controlled pizza swimmer.
  const swimmer = {
    x: 256,
    y: 160,
    vx: 0,
    vy: 0,
    size: 2,
    radius: sizeToRadius(2),
    facing: 1,
    health: 3
  };

  // Maintain an array of fish swimming around the lake.
  const fish = [];

  // Configure the lake bounds and physics tuning values.
  const lake = {
    width: 512 * 3,
    height: 288 * 2,
    swimAccel: 180,
    swimBoost: 240 * 4,
    drag: 0.9,
    sinkForce: 96,
    maxSpeed: 160
  };

  // Camera that follows the swimmer while respecting the enlarged lake dimensions.
  const camera = { x: 0, y: 0, zoom: 1 };

  // Track growth progress when eating fish one size down.
  let growthProgress = 0;
  // Prevent repeated hits by granting brief invulnerability after damage.
  let damageCooldown = 0;
  // Remember the last requested return so callers can swap modes.
  let pendingAction = null;
  // Cache the active playSound hook from the platformer shell.
  let playSound = () => {};
  // Animate visual growth while immediately updating gameplay stats.
  const growthAnimation = {
    active: false,
    progress: 0,
    duration: 0.65,
    startRadius: 0,
    targetRadius: 0,
    pendingZoom: false
  };

  // Produce a mouth position for a circular character using its facing direction.
  function getMouthPoint(entity) {
    // Offset the mouth toward the direction of travel for clarity in collisions.
    return {
      x: entity.x + entity.facing * entity.radius * 0.75,
      y: entity.y
    };
  }

  // Create a single fish actor of the requested size.
  function createFish(size) {
    // Spawn fish at random positions so the lake feels populated.
    const radius = sizeToRadius(size);
    const heading = Math.random() < 0.5 ? -1 : 1;
    return {
      x: Math.random() * (lake.width - radius * 2) + radius,
      y: Math.random() * (lake.height - radius * 2) + radius,
      vx: heading * (30 + Math.random() * 30),
      vy: (Math.random() - 0.5) * 12,
      size,
      radius,
      facing: heading
    };
  }

  // Repopulate the lake with a mix of fish sizes.
  function spawnFishField() {
    // Clear any previous fish so the field resets on death or entry.
    fish.length = 0;
    const counts = [
      { size: 1, count: 14 },
      { size: 2, count: 8 },
      { size: 3, count: 5 },
      { size: 4, count: 2 }
    ];
    // Iterate through each size group to seed the lake.
    for (const entry of counts) {
      // Loop enough times to place the requested number of fish for this size.
      for (let i = 0; i < entry.count; i++) {
        fish.push(createFish(entry.size));
      }
    }
  }

  // Reset the swimmer and fish after death or initial entry.
  function resetLake() {
    swimmer.x = 256;
    swimmer.y = 160;
    swimmer.vx = 0;
    swimmer.vy = 0;
    swimmer.size = 2;
    swimmer.radius = sizeToRadius(swimmer.size);
    swimmer.health = 3;
    swimmer.facing = 1;
    growthProgress = 0;
    damageCooldown = 0;
    pendingAction = null;
    camera.x = 0;
    camera.y = 0;
    spawnFishField();
  }

  // Center the camera on the swimmer without revealing space outside the lake.
  function updateCameraView(viewWidth, viewHeight) {
    // Target the swimmer's position so the player stays near the middle of the screen.
    const targetX = swimmer.x - viewWidth * 0.5;
    const targetY = swimmer.y - viewHeight * 0.5;
    const maxX = Math.max(0, lake.width - viewWidth);
    const maxY = Math.max(0, lake.height - viewHeight);
    // Clamp the camera to the stage bounds so off-screen areas remain hidden.
    camera.x = clamp(targetX, 0, maxX);
    camera.y = clamp(targetY, 0, maxY);
  }

  // Start a growth animation while keeping gameplay stats at the new size.
  function startGrowthAnimation(previousRadius) {
    // Flag the animation so rendering can ease toward the new radius.
    growthAnimation.active = true;
    growthAnimation.progress = 0;
    growthAnimation.startRadius = previousRadius;
    growthAnimation.targetRadius = swimmer.radius;
    // Request a camera zoom adjustment after the animation completes.
    growthAnimation.pendingZoom = true;
  }

  // Calculate how large the swimmer should appear while animating growth.
  function computeSwimmerRenderRadius() {
    // Use the raw radius when no growth animation is active.
    if (!growthAnimation.active) {
      return swimmer.radius;
    }
    // Ease the radius toward the final size for a smoother look.
    const t = Math.min(1, growthAnimation.progress / growthAnimation.duration);
    const eased = t * t * (3 - 2 * t);
    return growthAnimation.startRadius + (growthAnimation.targetRadius - growthAnimation.startRadius) * eased;
  }

  // Advance and resolve growth animations and camera zoom steps.
  function updateGrowthAnimation(dt) {
    // Skip processing when no animation is in play.
    if (!growthAnimation.active) {
      return;
    }
    // Progress the timer toward completion.
    growthAnimation.progress = Math.min(growthAnimation.duration, growthAnimation.progress + dt);
    // Apply camera zoom and finalize the animation when the timer ends.
    if (growthAnimation.progress >= growthAnimation.duration) {
      growthAnimation.active = false;
      if (growthAnimation.pendingZoom) {
        // Zoom out slightly so the larger swimmer stays in frame.
        camera.zoom = Math.max(0.65, camera.zoom * 0.96);
        growthAnimation.pendingZoom = false;
      }
    }
  }

  // Translate size gaps into the number of growth points awarded.
  function getGrowthPoints(sizeDifference) {
    // Award full credit for fish just one size down.
    if (sizeDifference === 1) {
      return 1;
    }
    // Award half credit for fish that are two sizes smaller.
    if (sizeDifference === 2) {
      return 0.5;
    }
    // Award a quarter credit for fish three sizes smaller.
    if (sizeDifference === 3) {
      return 0.25;
    }
    return 0;
  }

  // Grow the swimmer after eating enough appropriately sized fish.
  function applyGrowthIfReady() {
    // Promote the swimmer while enough points are banked.
    while (growthProgress >= 3 && swimmer.size < 6) {
      const previousRadius = swimmer.radius;
      swimmer.size = Math.min(6, swimmer.size + 1);
      swimmer.radius = sizeToRadius(swimmer.size);
      growthProgress -= 3;
      startGrowthAnimation(previousRadius);
      // Play a celebratory ping when the player grows.
      playSound('coin');
      // Return to the overworld once the player reaches the final size.
      if (swimmer.size >= 6) {
        pendingAction = { type: 'return-to-map' };
      }
    }
  }

  // Update a single fish's movement and orientation.
  function updateFish(dt, actor) {
    actor.x += actor.vx * dt;
    actor.y += actor.vy * dt;
    // Bounce horizontally when reaching the lake walls.
    if (actor.x < actor.radius) {
      actor.x = actor.radius;
      actor.vx *= -1;
    }
    if (actor.x > lake.width - actor.radius) {
      actor.x = lake.width - actor.radius;
      actor.vx *= -1;
    }
    // Gently constrain vertical drift so fish remain visible.
    if (actor.y < actor.radius) {
      actor.y = actor.radius;
      actor.vy *= -0.5;
    }
    if (actor.y > lake.height - actor.radius) {
      actor.y = lake.height - actor.radius;
      actor.vy *= -0.5;
    }
    // Update facing based on the current horizontal motion.
    if (actor.vx !== 0) {
      actor.facing = actor.vx > 0 ? 1 : -1;
    }
  }

  // Move the swimmer using simplified underwater physics.
  function updateSwimmer(dt, keys) {
    // Apply horizontal input to nudge the pizza through the water.
    if (keys.left && !keys.right) {
      swimmer.vx -= lake.swimAccel * dt;
      swimmer.facing = -1;
    } else if (keys.right && !keys.left) {
      swimmer.vx += lake.swimAccel * dt;
      swimmer.facing = 1;
    } else {
      // Apply drag when no directional input is active.
      swimmer.vx *= Math.pow(lake.drag, dt * 60);
    }

    // Swim upward while jump is held to mimic paddling.
    if (keys.jump) {
      swimmer.vy -= lake.swimBoost * dt;
    }

    // Constant downward pull to simulate water pressure.
    swimmer.vy += lake.sinkForce * dt;

    // Cap velocities to keep motion controllable.
    swimmer.vx = clamp(swimmer.vx, -lake.maxSpeed, lake.maxSpeed);
    swimmer.vy = clamp(swimmer.vy, -lake.maxSpeed, lake.maxSpeed);

    swimmer.x = clamp(swimmer.x + swimmer.vx * dt, swimmer.radius, lake.width - swimmer.radius);
    swimmer.y = clamp(swimmer.y + swimmer.vy * dt, swimmer.radius, lake.height - swimmer.radius);
  }

  // Handle all collision rules between the swimmer and nearby fish.
  function handleCollisions() {
    // Nothing to do when a transition back to the overworld is pending.
    if (pendingAction) {
      return;
    }

    const mouth = getMouthPoint(swimmer);
    const mouthRadius = swimmer.radius * 0.35;

    // Iterate backwards so eaten fish can be spliced safely.
    for (let i = fish.length - 1; i >= 0; i--) {
      const target = fish[i];
      const fishMouth = getMouthPoint(target);
      const fishMouthRadius = target.radius * 0.35;

      const playerBiteDistance = Math.hypot(target.x - mouth.x, target.y - mouth.y);
      // Allow the swimmer to eat fish that are strictly smaller.
      if (playerBiteDistance <= target.radius + mouthRadius && target.size < swimmer.size) {
        const sizeDifference = swimmer.size - target.size;
        const growthGain = getGrowthPoints(sizeDifference);
        fish.splice(i, 1);
        fish.push(createFish(target.size));
        // Only add progress when the eaten fish is large enough to matter.
        if (growthGain > 0) {
          growthProgress += growthGain;
          applyGrowthIfReady();
        }
        playSound('enemy');
        continue;
      }

      // Ignore contact from fish that match the swimmer's current size.
      if (target.size === swimmer.size) {
        continue;
      }

      const fishBiteDistance = Math.hypot(swimmer.x - fishMouth.x, swimmer.y - fishMouth.y);
      const sizeGap = target.size - swimmer.size;
      // Resolve collisions only when the fish mouth reaches the swimmer.
      if (fishBiteDistance <= swimmer.radius + fishMouthRadius && sizeGap > 0) {
        if (sizeGap >= 2) {
          // Instantly reset the lake when a huge fish catches you.
          resetLake();
          return;
        }
        // Apply damage once per cooldown window for near-peer fish.
        if (damageCooldown <= 0) {
          swimmer.health -= 1;
          damageCooldown = 1.1;
          playSound('hurt');
          if (swimmer.health <= 0) {
            resetLake();
            return;
          }
        }
      }
    }
  }

  // Draw the underwater backdrop, fish, and HUD.
  function draw(ctx) {
    const viewWidth = ctx.canvas.width / camera.zoom;
    const viewHeight = ctx.canvas.height / camera.zoom;
    updateCameraView(viewWidth, viewHeight);

    ctx.save();
    ctx.scale(camera.zoom, camera.zoom);
    ctx.translate(-camera.x, -camera.y);
    const gradient = ctx.createLinearGradient(0, 0, 0, lake.height);
    gradient.addColorStop(0, '#0a1c2f');
    gradient.addColorStop(1, '#021020');
    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, lake.width, lake.height);

    // Render fish as simple ellipses tinted by size.
    for (const actor of fish) {
      ctx.save();
      ctx.translate(actor.x, actor.y);
      ctx.scale(actor.facing, 1);
      ctx.fillStyle = actor.size >= 4 ? '#88c0ff' : actor.size === 3 ? '#6be3c2' : actor.size === 2 ? '#7bc0ff' : '#d0f3ff';
      ctx.beginPath();
      ctx.ellipse(0, 0, actor.radius, actor.radius * 0.7, 0, 0, Math.PI * 2);
      ctx.fill();
      ctx.fillStyle = '#01243b';
      ctx.beginPath();
      ctx.arc(actor.radius * 0.6, 0, actor.radius * 0.18, 0, Math.PI * 2);
      ctx.fill();
      ctx.restore();
    }

    // Draw the swimmer pizza as a simple circle.
    ctx.save();
    ctx.translate(swimmer.x, swimmer.y);
    ctx.rotate(swimmer.facing === -1 ? Math.PI : 0);
    const swimmerRenderRadius = computeSwimmerRenderRadius();
    ctx.fillStyle = '#f2b705';
    ctx.beginPath();
    ctx.arc(0, 0, swimmerRenderRadius, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillStyle = '#c1440e';
    ctx.beginPath();
    ctx.arc(swimmerRenderRadius * 0.6, 0, swimmerRenderRadius * 0.2, 0, Math.PI * 2);
    ctx.fill();
    ctx.restore();
    ctx.restore();

    // Paint HUD text for size, growth, and health.
    ctx.fillStyle = 'rgba(0, 0, 0, 0.5)';
    ctx.fillRect(8, 8, 190, 64);
    ctx.fillStyle = '#ffffff';
    ctx.font = '14px monospace';
    ctx.fillText(`Lake size: ${swimmer.size}/6`, 16, 30);
    ctx.fillText(`Health: ${swimmer.health}`, 16, 50);
    // Show fractional progress with two decimals when needed.
    const growthDisplay = Number.isInteger(growthProgress) ? growthProgress : growthProgress.toFixed(2);
    ctx.fillText(`Growth: ${growthDisplay}/3`, 16, 70);
  }

  // Update the entire lake mode and surface transition requests.
  function update(dt, keys, playSoundHook) {
    // Cache the audio hook so helper functions can play effects.
    playSound = typeof playSoundHook === 'function' ? playSoundHook : () => {};
    damageCooldown = Math.max(0, damageCooldown - dt);
    updateSwimmer(dt, keys || {});
    updateGrowthAnimation(dt);
    // Advance each fish to keep the lake lively.
    for (const actor of fish) {
      updateFish(dt, actor);
    }
    handleCollisions();
    const action = pendingAction;
    pendingAction = null;
    return action;
  }

  // Prepare the lake for play whenever the player enters this mode.
  function enter(playSoundHook) {
    playSound = typeof playSoundHook === 'function' ? playSoundHook : () => {};
    resetLake();
  }

  window.LakeMode = {
    enter,
    update,
    draw
  };
})();
