(function () {
  const { Body } = Matter;

  // Compute closest point on rect to circle center and return contact info.
  function circleRectContact(circle, rect) {
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
  }

  // Create a simple patrolling skeleton.
  function createSkeleton(x, y, minX, maxX) {
    return {
      x,
      y,
      width: 46,
      height: 76,
      dir: 1,
      speed: 60,
      minX,
      maxX,
      alive: true,
      hitTimer: 0
    };
  }

  const skeletons = [];

  // Seed skeletons at a baseline y so they start grounded.
  function initSkeletons(baseY) {
    skeletons.length = 0;
    skeletons.push(
      createSkeleton(360, baseY, 320, 480),
      createSkeleton(960, baseY, 900, 1100),
      createSkeleton(1920, baseY, 1880, 2100)
    );
  }

  // Reset skeleton positions/states.
  function resetSkeletons(baseY) {
    skeletons.forEach((skeleton, index) => {
      skeleton.alive = true;
      skeleton.hitTimer = 0;
      skeleton.x = [360, 960, 1920][index] || skeleton.x;
      skeleton.y = baseY;
    });
  }

  // Update AI, handle collisions, and trigger stomp/hurt outcomes.
  function updateSkeletons(dt, pizza, pizzaBody, physicsSettings, playSound, spawnParticle, onPlayerDeath) {
    for (const skeleton of skeletons) {
      if (!skeleton.alive) {
        skeleton.hitTimer -= dt;
        continue;
      }
      skeleton.x += skeleton.dir * skeleton.speed * dt;
      if (skeleton.x < skeleton.minX) {
        skeleton.x = skeleton.minX;
        skeleton.dir = 1;
      } else if (skeleton.x + skeleton.width > skeleton.maxX) {
        skeleton.x = skeleton.maxX - skeleton.width;
        skeleton.dir = -1;
      }

      const contact = circleRectContact(pizza, skeleton);
      if (contact.overlap) {
        const downwardDot = contact.distance > 0 ? -contact.dy / contact.distance : 1;
        const hittingFromAbove = downwardDot > 0.6;
        if (hittingFromAbove && pizza.invincibleTimer <= 0) {
          skeleton.alive = false;
          skeleton.hitTimer = 0.6;
          Body.setVelocity(pizzaBody, {
            x: pizzaBody.velocity.x,
            y: -physicsSettings.jumpSpeed * 0.7
          });
          pizza.score += 100;
          playSound('enemy');
          for (let i = 0; i < 12; i++) {
            spawnParticle(
              skeleton.x + skeleton.width / 2,
              skeleton.y + skeleton.height / 2,
              i % 2 ? '#fbe7c4' : '#dcb48f'
            );
          }
        } else if (pizza.invincibleTimer <= 0) {
          pizza.health -= 1;
          pizza.invincibleTimer = 1.2;
          const hitX = contact.closestX;
          const hitY = contact.closestY;
          const awayX = pizza.x - hitX;
          const awayY = pizza.y - hitY;
          const awayLen = Math.hypot(awayX, awayY) || 1;
          const nx = awayX / awayLen;
          const ny = awayY / awayLen;
          Body.setVelocity(pizzaBody, {
            x: pizzaBody.velocity.x + nx * 6,
            y: pizzaBody.velocity.y + ny * 6 - physicsSettings.jumpSpeed * 0.2
          });
          playSound('hurt');
          if (pizza.health <= 0 && typeof onPlayerDeath === 'function') {
            onPlayerDeath();
          }
        }
      }
    }
  }

  // Draw skeleton sprites relative to the camera offset.
  function drawSkeletons(ctx, cameraX) {
    for (const skeleton of skeletons) {
      const x = skeleton.x - cameraX;
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
        ctx.fillStyle = 'rgba(255,255,255,0.15)';
        ctx.fillRect(x, skeleton.y, skeleton.width, skeleton.height);
      }
    }
  }

  window.EnemySystem = {
    skeletons,
    initSkeletons,
    resetSkeletons,
    updateSkeletons,
    drawSkeletons,
    circleRectContact
  };
})();
