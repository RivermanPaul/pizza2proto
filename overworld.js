// Overworld navigation layer that mirrors Super Mario Bros 3's node-based map.
(() => {
  /**
   * Graph of overworld nodes players can travel between, each with screen coordinates and neighbor links.
   */
  const nodes = [
    { id: 'home', label: 'Home Base', x: 120, y: 190, links: ['grove'] },
    { id: 'grove', label: 'Spooky Grove', x: 220, y: 190, links: ['home', 'cliff'] },
    { id: 'cliff', label: 'Cliff Road', x: 320, y: 160, links: ['grove', 'bridge'] },
    { id: 'bridge', label: 'Moonlit Bridge', x: 420, y: 190, links: ['cliff', 'ruins'] },
    { id: 'ruins', label: 'Ruins', x: 520, y: 150, links: ['bridge', 'gate'] },
    { id: 'gate', label: 'Skeleton Gate', x: 620, y: 190, links: ['ruins'] }
  ];

  /**
   * Quick lookup from id to full node data for traversal decisions.
   */
  const nodeMap = new Map(nodes.map((node) => [node.id, node]));

  // Current location within the overworld graph.
  let currentNodeId = 'home';
  // Cooldown timer to prevent multi-move per button hold.
  let moveCooldown = 0;
  // Track previous frame input so we can only react to transitions.
  const lastInput = { left: false, right: false, jump: false };

  /**
   * Sets the overworld to its initial state each time the player enters it.
   */
  function enter(score) {
    // When the player has picked up some score, start them further along the path to honor progress.
    if (score >= 10) {
      currentNodeId = 'cliff';
    } else {
      currentNodeId = 'home';
    }
    moveCooldown = 0;
    lastInput.left = false;
    lastInput.right = false;
    lastInput.jump = false;
  }

  /**
   * Attempts to move to the nearest neighbor that matches the intended direction of travel.
   */
  function attemptMove(direction, playSound) {
    // Fetch current node details so we can check connected neighbors.
    const currentNode = nodeMap.get(currentNodeId);
    if (!currentNode) {
      return;
    }

    // Pick the neighbor that sits in the requested direction, prioritizing closer x distances.
    let candidate = null;
    for (const neighborId of currentNode.links) {
      // Loop over each neighbor to find the best fit for the chosen direction.
      const neighbor = nodeMap.get(neighborId);
      if (!neighbor) {
        continue;
      }
      // Skip neighbors that are not in the requested direction relative to the player node.
      if (direction === 'left' && neighbor.x >= currentNode.x) {
        continue;
      }
      if (direction === 'right' && neighbor.x <= currentNode.x) {
        continue;
      }
      // Record the closest neighbor so traversal feels predictable.
      if (!candidate || Math.abs(neighbor.x - currentNode.x) < Math.abs(candidate.x - currentNode.x)) {
        candidate = neighbor;
      }
    }

    // Move to the neighbor and play a click if the neighbor existed in the desired direction.
    if (candidate) {
      currentNodeId = candidate.id;
      moveCooldown = 0.22;
      if (typeof playSound === 'function') {
        playSound('coin');
      }
    }
  }

  /**
   * Updates overworld input handling and returns a launch request when the player selects a stage.
   */
  function update(dt, keys, playSound) {
    // Guard against missing input state because the platformer code drives this module.
    if (!keys) {
      return null;
    }

    moveCooldown = Math.max(0, moveCooldown - dt);

    // React to a left press only when we are not repeating the previous frame and cooldown expired.
    if (keys.left && !lastInput.left && moveCooldown <= 0) {
      attemptMove('left', playSound);
    }

    // React to a right press only when we are not repeating the previous frame and cooldown expired.
    if (keys.right && !lastInput.right && moveCooldown <= 0) {
      attemptMove('right', playSound);
    }

    // Track jump presses to trigger level launch from the current node.
    const launchRequested = keys.jump && !lastInput.jump;
    if (launchRequested) {
      return { type: 'launch-level', from: currentNodeId };
    }

    lastInput.left = keys.left;
    lastInput.right = keys.right;
    lastInput.jump = keys.jump;
    return null;
  }

  /**
   * Draws the overworld nodes and links so players can see where they can travel.
   */
  function draw(ctx) {
    // Bail out gracefully if the canvas context is missing.
    if (!ctx) {
      return;
    }

    // Paint a soft night sky gradient behind the map.
    const gradient = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);
    gradient.addColorStop(0, '#0c0a18');
    gradient.addColorStop(1, '#05040e');
    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    // Draw faint stars to keep the overworld lively.
    ctx.fillStyle = 'rgba(255,255,255,0.12)';
    for (let i = 0; i < 90; i++) {
      // Scatter each star across the canvas using pseudo-random math.
      const starX = (i * 37) % ctx.canvas.width;
      const starY = (i * 73) % ctx.canvas.height;
      ctx.fillRect(starX, starY, 2, 2);
    }

    // Render connections between nodes as glowing paths.
    ctx.strokeStyle = '#3a4c9e';
    ctx.lineWidth = 6;
    ctx.lineCap = 'round';
    for (const node of nodes) {
      // Loop over each node's links to draw the outward path segments.
      for (const linkId of node.links) {
        // Only draw each edge once by enforcing an ordering check.
        if (node.id > linkId) {
          continue;
        }
        const target = nodeMap.get(linkId);
        if (!target) {
          continue;
        }
        ctx.beginPath();
        ctx.moveTo(node.x, node.y);
        ctx.lineTo(target.x, target.y);
        ctx.stroke();
      }
    }

    // Render each node with a pulsing highlight on the current selection.
    for (const node of nodes) {
      // Check whether this node is the active location to adjust styling.
      const isCurrent = node.id === currentNodeId;
      ctx.fillStyle = isCurrent ? '#f5c542' : '#9aa5ff';
      ctx.strokeStyle = isCurrent ? '#ffe9a3' : '#d6dcff';
      ctx.lineWidth = 3;
      ctx.beginPath();
      ctx.arc(node.x, node.y, isCurrent ? 14 : 10, 0, Math.PI * 2);
      ctx.fill();
      ctx.stroke();

      ctx.fillStyle = '#ffffff';
      ctx.font = '12px monospace';
      ctx.textAlign = 'center';
      ctx.fillText(node.label, node.x, node.y + 24);
    }

    // Provide a hint banner explaining how to start the selected level.
    ctx.fillStyle = 'rgba(0, 0, 0, 0.45)';
    ctx.fillRect(12, 12, 220, 44);
    ctx.fillStyle = '#ffffff';
    ctx.font = '13px monospace';
    ctx.textAlign = 'left';
    ctx.fillText('Press jump to enter stage', 20, 36);
  }

  /**
   * Exposes the overworld API on the window so the platformer shell can drive it.
   */
  window.Overworld = {
    enter,
    update,
    draw,
    getCurrentNode: () => currentNodeId
  };
})();
