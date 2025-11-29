// PauseMenu builds and controls the global pause button and dropdown.
(function () {
  // Track whether the pause UI has already been built to avoid duplicate controls.
  let initialized = false;
  // Track whether the game is currently paused so the loop can gate updates.
  let paused = false;
  // Keep references to key DOM nodes so labels and visibility can be toggled.
  let pauseButton;
  let pauseMenu;
  let pauseResumeButton;
  let pauseMuteButton;
  let pauseExitButton;

  // Adds a stylesheet that scopes pause menu visuals to match the game's HUD styling.
  function injectStyles() {
    const style = document.createElement('style');
    style.textContent = `
      #pause-button {
        position: fixed;
        top: 12px;
        right: 12px;
        padding: 10px 14px;
        border-radius: 12px;
        border: 2px solid #42405f;
        background: #1e1b34;
        color: #f4f4fb;
        font-weight: 700;
        letter-spacing: 0.4px;
        cursor: pointer;
        z-index: 20;
        box-shadow: 0 8px 24px rgba(0, 0, 0, 0.4);
      }
      #pause-menu {
        position: fixed;
        top: 60px;
        right: 12px;
        display: none;
        flex-direction: column;
        gap: 10px;
        padding: 14px;
        border-radius: 12px;
        background: rgba(12, 11, 24, 0.94);
        border: 1px solid #35334f;
        z-index: 25;
        min-width: 180px;
        box-shadow: 0 16px 48px rgba(0, 0, 0, 0.55);
      }
      #pause-menu.visible {
        display: flex;
      }
      #pause-menu h3 {
        margin: 0 0 6px 0;
        font-size: 16px;
        color: #f4f4fb;
        letter-spacing: 0.5px;
      }
      #pause-menu button {
        height: 40px;
        border-radius: 10px;
        border: 2px solid #42405f;
        background: #24213b;
        color: #f4f4fb;
        font-weight: 600;
        cursor: pointer;
        transition: background 0.1s ease, transform 0.1s ease;
      }
      #pause-menu button:active {
        background: #2f2951;
        transform: translateY(1px);
      }
    `;
    document.head.appendChild(style);
  }

  // Builds the pause menu DOM tree and attaches it to the document body.
  function buildMenu() {
    pauseButton = document.createElement('button');
    pauseButton.id = 'pause-button';
    pauseButton.ariaLabel = 'Pause Game';
    pauseButton.textContent = 'Pause';

    pauseMenu = document.createElement('div');
    pauseMenu.id = 'pause-menu';
    pauseMenu.ariaLabel = 'Pause Menu';

    const title = document.createElement('h3');
    title.textContent = 'Paused';

    pauseResumeButton = document.createElement('button');
    pauseResumeButton.id = 'pause-resume';
    pauseResumeButton.ariaLabel = 'Return to Game';
    pauseResumeButton.textContent = 'Return to Game';

    pauseMuteButton = document.createElement('button');
    pauseMuteButton.id = 'pause-mute';
    pauseMuteButton.ariaLabel = 'Toggle Mute';

    pauseExitButton = document.createElement('button');
    pauseExitButton.id = 'pause-exit';
    pauseExitButton.ariaLabel = 'Exit to Overworld';
    pauseExitButton.textContent = 'Exit to Overworld';

    pauseMenu.appendChild(title);
    pauseMenu.appendChild(pauseResumeButton);
    pauseMenu.appendChild(pauseMuteButton);
    pauseMenu.appendChild(pauseExitButton);

    document.body.appendChild(pauseButton);
    document.body.appendChild(pauseMenu);
  }

  // Shows the pause menu UI and flips the paused flag so the loop halts progression.
  function openMenu() {
    paused = true;
    pauseMenu.classList.add('visible');
    pauseButton.textContent = 'Resume';
  }

  // Hides the pause menu UI and lowers the paused flag so the loop can resume.
  function closeMenu() {
    paused = false;
    pauseMenu.classList.remove('visible');
    pauseButton.textContent = 'Pause';
  }

  // Updates the mute label to reflect whether audio is currently silenced.
  function updateMuteLabel(isMuted) {
    pauseMuteButton.textContent = isMuted ? 'Unmute' : 'Mute';
  }

  // Connects button interactions to the provided callbacks so outer code can react.
  function wireEvents(handlers) {
    // Toggle the pause state when the player clicks the fixed pause button.
    pauseButton.addEventListener('click', () => {
      // If already paused, close the menu before notifying resume callbacks.
      if (paused) {
        closeMenu();
        // Invoke the resume handler so the caller can reset timing when unpausing.
        if (handlers.onResume) {
          handlers.onResume();
        }
      } else {
        // If active, open the menu before signaling a pause.
        openMenu();
        // Invoke the pause handler so callers can react to the stop state.
        if (handlers.onPause) {
          handlers.onPause();
        }
      }
    });

    // Close the menu when the player chooses to return to the game.
    pauseResumeButton.addEventListener('click', () => {
      closeMenu();
      // Invoke the resume handler so the caller can reset timing when resuming.
      if (handlers.onResume) {
        handlers.onResume();
      }
    });

    // Toggle mute status and reflect the new label when tapped from the pause UI.
    pauseMuteButton.addEventListener('click', () => {
      // Apply the provided mute toggle callback when supplied by the caller.
      if (handlers.onToggleMute) {
        handlers.onToggleMute();
      }
      // Reflect the updated mute state using the supplied getter when available.
      updateMuteLabel(handlers.isMuted ? handlers.isMuted() : false);
    });

    // Exit to the overworld and close the menu in response to the exit button.
    pauseExitButton.addEventListener('click', () => {
      // Invoke the exit handler so callers can swap modes on exit.
      if (handlers.onExit) {
        handlers.onExit();
      }
      closeMenu();
      // Invoke the resume handler so the caller can reset timing after exiting.
      if (handlers.onResume) {
        handlers.onResume();
      }
    });
  }

  // Initializes the pause menu once and returns a controller for the game loop.
  function init(handlers = {}) {
    // Avoid reconstructing the UI if init is invoked multiple times.
    if (initialized) {
      // Return the existing controller when initialization already occurred.
      return {
        isPaused: () => paused,
        open: openMenu,
        close: closeMenu,
        updateMuteLabel
      };
    }

    injectStyles();
    buildMenu();
    wireEvents(handlers);
    // Seed the mute label using the provided getter, defaulting to an unmuted state.
    updateMuteLabel(handlers.isMuted ? handlers.isMuted() : false);

    initialized = true;

    return {
      // Expose the paused flag so the main loop can short-circuit updates.
      isPaused: () => paused,
      // Provide an open helper so other scripts can force a pause state.
      open: openMenu,
      // Provide a close helper so other scripts can dismiss the menu.
      close: closeMenu,
      // Allow external callers to refresh the mute label when audio changes.
      updateMuteLabel
    };
  }

  window.PauseMenu = { init };
})();
