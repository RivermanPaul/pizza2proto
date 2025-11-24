// Overworld navigation layer that mirrors Super Mario Bros 3's node-based map.
(() => {
  /**
   * Graph of overworld nodes players can travel between, each with screen coordinates and neighbor links.
   */
  const nodes = [
    { id: 'start', label: 'Start', x: 80, y: 200, links: ['stage1', 'turn'] },
    { id: 'stage1', label: '1', x: 160, y: 200, links: ['start', 'stage2'] },
    { id: 'stage2', label: '2', x: 240, y: 200, links: ['stage1', 'stage3'] },
    { id: 'stage3', label: '3', x: 320, y: 200, links: ['stage2', 'stage4'] },
    { id: 'stage4', label: '4', x: 320, y: 280, links: ['stage3', 'bridge'] },
    { id: 'bridge', label: 'G', x: 440, y: 280, links: ['stage4', 'castleGate', 'riverTurn'] },
    { id: 'castleGate', label: 'Castle', x: 520, y: 280, links: ['bridge', 'keep'] },
    { id: 'keep', label: 'Help', x: 520, y: 200, links: ['castleGate'] },
    { id: 'turn', label: 'Path', x: 80, y: 280, links: ['start', 'bend'] },
    { id: 'bend', label: 'B', x: 80, y: 360, links: ['turn', 'stage5'] },
    { id: 'stage5', label: '5', x: 160, y: 360, links: ['bend', 'stage6'] },
    { id: 'stage6', label: '6', x: 240, y: 360, links: ['stage5', 'ferry'] },
    { id: 'ferry', label: 'Boat', x: 320, y: 360, links: ['riverTurn', 'stage6'] },
    // Node A sits directly above the boat so pressing down sails correctly to the ferry stop.
    { id: 'riverTurn', label: 'A', x: 320, y: 280, links: ['ferry', 'bridge'] }
  ];

  /**
   * Quick lookup from id to full node data for traversal decisions.
   */
  const nodeMap = new Map(nodes.map((node) => [node.id, node]));

  /**
   * Camera that keeps the player centered while respecting map extents.
   */
  const camera = { x: 0, y: 0 };

  /**
   * Bounds of the map derived from node coordinates, expanded for padding.
   */
  const mapBounds = nodes.reduce(
    (bounds, node) => {
      // Spread each node into the running bounding box so the camera clamp stays correct.
      return {
        minX: Math.min(bounds.minX, node.x),
        maxX: Math.max(bounds.maxX, node.x),
        minY: Math.min(bounds.minY, node.y),
        maxY: Math.max(bounds.maxY, node.y)
      };
    },
    { minX: Infinity, maxX: -Infinity, minY: Infinity, maxY: -Infinity }
  );

  // Soft padding around the playable area so the camera is not glued to the nodes.
  const mapPadding = 80;
  mapBounds.minX -= mapPadding;
  mapBounds.maxX += mapPadding;
  mapBounds.minY -= mapPadding;
  mapBounds.maxY += mapPadding;

  // Current location within the overworld graph.
  let currentNodeId = 'start';
  // Remember the last visited node so re-entering the overworld resumes where the player left off.
  let lastVisitedNodeId = currentNodeId;
  // Visual representation of the pizza's position, animated between nodes.
  const pizzaMarker = { x: nodeMap.get(currentNodeId).x, y: nodeMap.get(currentNodeId).y };
  // Cooldown timer to prevent multi-move per button hold.
  let moveCooldown = 0;
  // Track previous frame input so we can only react to transitions.
  const lastInput = { left: false, right: false, up: false, down: false, jump: false };

  /**
   * Stores in-progress travel between nodes for smooth interpolation.
   */
  const travelState = {
    from: null,
    to: null,
    t: 0,
    duration: 0
  };

  /**
   * Sets the overworld to its initial state each time the player enters it.
   */
  function enter(score) {
    // Reuse the previous selection when possible so overworld entries start from the last cleared level.
    currentNodeId = nodeMap.has(lastVisitedNodeId) ? lastVisitedNodeId : 'start';
    // Reset travel state so we snap the marker to the current node on entry.
    travelState.from = null;
    travelState.to = null;
    travelState.t = 0;
    travelState.duration = 0;
    const startNode = nodeMap.get(currentNodeId);
    // Update the marker immediately in case the caller re-enters the overworld mid-run.
    if (startNode) {
      pizzaMarker.x = startNode.x;
      pizzaMarker.y = startNode.y;
    }
    moveCooldown = 0;
    lastInput.left = false;
    lastInput.right = false;
    lastInput.up = false;
    lastInput.down = false;
    lastInput.jump = false;
  }

  /**
   * Attempts to move to the nearest neighbor that matches the intended direction of travel.
   */
  function attemptMove(direction, playSound) {
    // Fetch current node details so we can check connected neighbors.
    const currentNode = nodeMap.get(currentNodeId);
    // Abort movement requests if the current node lookup failed.
    if (!currentNode) {
      return;
    }

    // Ignore input while the pizza is already sliding between nodes.
    if (travelState.to) {
      return;
    }

    // Pick the neighbor that sits in the requested direction, prioritizing closer axis distances.
    let candidate = null;
    for (const neighborId of currentNode.links) {
      // Loop over each neighbor to find the best fit for the chosen direction.
      const neighbor = nodeMap.get(neighborId);
      // Skip links that are misconfigured rather than crash the overworld navigation.
      if (!neighbor) {
        continue;
      }
      // Only allow axis-aligned travel so map lines never slant.
      if (neighbor.x !== currentNode.x && neighbor.y !== currentNode.y) {
        continue;
      }
      // Skip neighbors that are not in the requested direction relative to the player node.
      if (direction === 'left' && neighbor.x >= currentNode.x) {
        continue;
      }
      if (direction === 'right' && neighbor.x <= currentNode.x) {
        continue;
      }
      if (direction === 'up' && neighbor.y >= currentNode.y) {
        continue;
      }
      if (direction === 'down' && neighbor.y <= currentNode.y) {
        continue;
      }
      // Record the closest neighbor so traversal feels predictable along the chosen axis.
      const axisDistance =
        direction === 'left' || direction === 'right'
          ? Math.abs(neighbor.x - currentNode.x)
          : Math.abs(neighbor.y - currentNode.y);
      const currentBestDistance =
        !candidate
          ? Infinity
          : direction === 'left' || direction === 'right'
            ? Math.abs(candidate.x - currentNode.x)
            : Math.abs(candidate.y - currentNode.y);
      if (!candidate || axisDistance < currentBestDistance) {
        candidate = neighbor;
      }
    }

    // Move to the neighbor and play a click if the neighbor existed in the desired direction.
    if (candidate) {
      const distance = Math.hypot(candidate.x - currentNode.x, candidate.y - currentNode.y);
      // Kick off a lerp between the current node and the next one to animate travel.
      travelState.from = currentNode;
      travelState.to = candidate;
      travelState.t = 0;
      travelState.duration = Math.max(0.001, distance / 220);
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

    // Progress the travel animation so node swaps feel smooth.
    if (travelState.to && travelState.from) {
      travelState.t = Math.min(1, travelState.t + dt / travelState.duration);
      const eased = 0.5 - Math.cos(travelState.t * Math.PI) * 0.5;
      // Interpolate the marker between start and target nodes.
      pizzaMarker.x = travelState.from.x + (travelState.to.x - travelState.from.x) * eased;
      pizzaMarker.y = travelState.from.y + (travelState.to.y - travelState.from.y) * eased;
      // Finalize the hop once the interpolation completes.
      if (travelState.t >= 1) {
        currentNodeId = travelState.to.id;
        // Persist the player's latest position so future overworld visits resume here.
        lastVisitedNodeId = currentNodeId;
        travelState.to = null;
        travelState.from = null;
      }
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

    // React to an up press to climb the grid when the cooldown allows movement.
    if (keys.up && !lastInput.up && moveCooldown <= 0) {
      attemptMove('up', playSound);
    }

    // React to a down press to descend the grid when the cooldown allows movement.
    if (keys.down && !lastInput.down && moveCooldown <= 0) {
      attemptMove('down', playSound);
    }

    // Track jump presses to trigger level launch from the current node.
    const launchRequested = keys.jump && !lastInput.jump;
    if (launchRequested) {
      return { type: 'launch-level', from: currentNodeId };
    }

    lastInput.left = keys.left;
    lastInput.right = keys.right;
    lastInput.up = keys.up;
    lastInput.down = keys.down;
    lastInput.jump = keys.jump;

    // Ease the camera toward the pizza while keeping it inside the map.
    const canvas = document.getElementById('game');
    // Ensure the canvas is available before we attempt to read dimensions.
    if (canvas && canvas.width && canvas.height) {
      const targetX = pizzaMarker.x - canvas.width * 0.5;
      const targetY = pizzaMarker.y - canvas.height * 0.5;
      const maxCamX = Math.max(mapBounds.minX, mapBounds.maxX - canvas.width);
      const maxCamY = Math.max(mapBounds.minY, mapBounds.maxY - canvas.height);
      // Clamp the desired camera target to remain inside the defined bounds.
      const desiredX = Math.min(maxCamX, Math.max(mapBounds.minX, targetX));
      const desiredY = Math.min(maxCamY, Math.max(mapBounds.minY, targetY));
      camera.x += (desiredX - camera.x) * 0.18;
      camera.y += (desiredY - camera.y) * 0.18;
    }

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
    ctx.save();
    ctx.translate(-camera.x, -camera.y);
    for (const node of nodes) {
      // Loop over each node's links to draw the outward path segments.
      for (const linkId of node.links) {
        // Only draw each edge once by enforcing an ordering check.
        if (node.id > linkId) {
          continue;
        }
        const target = nodeMap.get(linkId);
        // Ignore missing neighbor references so one bad link does not break the render.
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

    // Draw the pizza marker traveling between nodes.
    ctx.save();
    ctx.translate(pizzaMarker.x, pizzaMarker.y);
    ctx.fillStyle = '#ffdf7f';
    ctx.beginPath();
    ctx.arc(0, 0, 11, 0, Math.PI * 2);
    ctx.fill();
    ctx.strokeStyle = '#bf7f2f';
    ctx.lineWidth = 3;
    ctx.stroke();
    ctx.restore();
    ctx.restore();

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
    // Provide read-only access to the currently selected node for HUD overlays.
    getCurrentNode: () => currentNodeId
  };
})();
