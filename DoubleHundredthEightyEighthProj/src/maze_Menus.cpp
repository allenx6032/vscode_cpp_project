#include "maze_Menus.h"
#include "maze_Button.h"
#include "maze_Manager.h"
#include "maze_Maze.h"
#include "maze_Player.h"
#include "maze_Timer.h"
#include "raylib.h"

// Menu class constructor
Menu::Menu(Maze *maze, Player *player, Timer *timer) {
  // Initializing maze and player pointers
  this->maze = maze;
  this->player = player;
  this->sessionTimer = timer;

  // Initialize Background Texture
  background = LoadTexture("Resources/gui/background.png");

  // Initializing buttons with images and sizes
  btnStart = {"Resources/gui/button_start.png", 8};
  btnExit = {"Resources/gui/button_exit.png", 8};

  // Buttons for difficulty selection
  btnEasy = {"Resources/gui/button_empty.png", 10};
  btnMedium = {"Resources/gui/button_empty.png", 10};
  btnHard = {"Resources/gui/button_empty.png", 10};

  // Buttons for game options like pause, home, regenerate, and fullscreen
  btnPause = {"Resources/gui/button_pause.png", 5};
  btnHome = {"Resources/gui/button_home.png", 5};
  btnRegenerate = {"Resources/gui/button_regenerate.png", 5};
  btnRestart = {"Resources/gui/button_regenerate.png", 5};
  btnfullScreen = {"Resources/gui/button_expand.png", 5};

  // Button for Player Controlling up, down, left, right
  btnMoveUp = {"Resources/controls/MoveUp_Control.png", 4};
  btnMoveDown = {"Resources/controls/MoveDown_Control.png", 4};
  btnMoveRight = {"Resources/controls/MoveRight_Control.png", 4};
  btnMoveLeft = {"Resources/controls/MoveLeft_Control.png", 4};

  //
  btnYes = {"Resources/gui/button_yes.png", 8};
  btnNo = {"Resources/gui/button_no.png", 8};
  //
  confirmTexture = LoadTexture("Resources/gui/confirm.png");
  //
  winBgTexture = LoadTexture("Resources/gui/win_background.png");
  //
  showCase = LoadTexture("Resources/showcase.png");
  // Load sound effect for button click
  clickSound = LoadSound("Resources/audio/button_clicked.mp3");
}

// Destructor to unload sound when Menu object is destroyed
Menu::~Menu() { UnloadSound(clickSound); }

// Draws the main menu screen
void Menu::DrawMainMenu(Manager &manager) {

  auto setUpStartButton = [&]() {
    btnStart.update(GetMousePosition());
    btnStart.SetPosition({200, 450});
    if (btnStart.isPressed()) {
      manager.showDifficlttyMenu = true; // Show difficulty menu
      PlaySound(clickSound);             // Play button click sound
    }
  };

  auto setUpExitButton = [&]() {
    btnExit.update(GetMousePosition());
    btnExit.SetPosition({700, 450});
    if (btnExit.isPressed()) {
      manager.windowExitRequested = 1;
      // manager.exitGame = 1;  // Exit the game
      PlaySound(clickSound); // Play button click sound
    }
  };

  setUpStartButton();
  setUpExitButton();

  // Drawing the screen
  BeginDrawing();
  ClearBackground(GRAY); // Set background color

  // Draw Background Texture
  DrawTexturePro(background, {0, 0, 270, 160}, {0, 0, 1280, 720}, {0, 0}, 0,
                 RAYWHITE);

  // Draw buttons or show the difficulty menu based on the state
  if (manager.showDifficlttyMenu != 1) {
    if (manager.windowExitRequested) {
      DrawExitConfirmMenu(manager);
    } else {
      DrawText("The Explorer", 300, 200, 100, WHITE);
      btnStart.drawbutton();
      btnExit.drawbutton();
    }
  } else {
    DrawDifficultyMenu(manager); // Draw difficulty menu if active
  }

  EndDrawing();
}

// Draws the game bar (pause, home, regenerate, fullscreen) in the game screen
void Menu::DrawGameBar(Manager &manager) {

  auto setupPauseButton = [&]() {
    btnPause.update(GetMousePosition());
    btnPause.SetPosition({1160, 30});
    if (btnPause.isPressed()) {
      PlaySound(clickSound); // Play button click sound

      manager.isPaused = !manager.isPaused; // Pause the game
      TraceLog(LOG_INFO, "Paused");
    }
  };

  auto setupHomeButton = [&]() {
    btnHome.update(GetMousePosition());
    btnHome.SetPosition({1060, 30});
    if (btnHome.isPressed()) {
      PlaySound(clickSound); // Play button click sound

      manager.setScreen(MAIN_MENU_SCREEN); // Go back to main menu
    }
  };

  auto setupRegenerateButton = [&]() {
    btnRegenerate.update(GetMousePosition());
    btnRegenerate.SetPosition({960, 30});
    if (btnRegenerate.isPressed()) {
      PlaySound(clickSound); // Play button click sound

      maze->generateMaze(); // Regenerate maze
      sessionTimer->startTimer();
      maze->showKey = 1;
      player->resetPosition(); // Reset player position
      TraceLog(LOG_INFO, "Generating New Maze");
    }
  };

  auto setupfullScreenButton = [&]() {
    btnfullScreen.update(GetMousePosition());
    btnfullScreen.SetPosition({860, 30});
    if (btnfullScreen.isPressed()) {
      PlaySound(clickSound); // Play button click sound
      TraceLog(LOG_INFO, "Full Screen Triggered");
      ToggleFullscreen(); // Toggle fullscreen mode
    }
  };

  setupPauseButton();
  setupHomeButton();
  setupRegenerateButton();
  setupfullScreenButton();

  // Always draw the other game control buttons
  btnPause.drawbutton();
  btnHome.drawbutton();
  btnRegenerate.drawbutton();
  btnfullScreen.drawbutton();
}

// Draws the difficulty selection menu when the user is setting the difficulty
void Menu::DrawDifficultyMenu(Manager &manager) {
  // Update button states for difficulty selection
  btnEasy.update(GetMousePosition());
  btnMedium.update(GetMousePosition());
  btnHard.update(GetMousePosition());

  auto changeDifficulty = [&](int diff) {
    maze->setDifficulty(diff); // Set difficulty to easy
    maze->resetMaze();         // Resize maze to fit the difficulty
    player->resetPosition();
    PlaySound(clickSound); // Play button click sound
    maze->generateMaze();  // Generate maze based on selected difficulty

    sessionTimer->startTimer();
    manager.setScreen(GAME_SCREEN);     // Switch to game screen
    manager.showDifficlttyMenu = false; // Close difficulty menu
  };

  // Set positions for difficulty buttons
  btnEasy.SetPosition({635 - 48 * 5, 100});
  btnMedium.SetPosition({635 - 48 * 5, 280});
  btnHard.SetPosition({635 - 48 * 5, 460});

  // Check if a difficulty button is pressed and perform respective actions
  if (btnEasy.isPressed()) {
    changeDifficulty(EASY_DIFF);
  }

  if (btnMedium.isPressed()) {
    changeDifficulty(MEDIUM_DIFF);
  }

  if (btnHard.isPressed()) {
    changeDifficulty(HARD_DIFF);
  }

  // Draw the difficulty buttons and labels
  btnEasy.drawbutton();
  DrawText("Easy", btnEasy.position.x + (5 * 33), btnEasy.position.y + 50, 60,
           GREEN);
  btnMedium.drawbutton();
  DrawText("Medium", btnMedium.position.x + (5 * 28), btnMedium.position.y + 50,
           60, WHITE);
  btnHard.drawbutton();
  DrawText("Hard", btnHard.position.x + (5 * 33), btnHard.position.y + 50, 60,
           RED);
}

// Draws Controls for the Player
void Menu::DrawPlayerControls(Player &player, Maze &maze, Camera2D &camera) {

  btnMoveUp.update(GetMousePosition());
  btnMoveDown.update(GetMousePosition());
  btnMoveLeft.update(GetMousePosition());
  btnMoveRight.update(GetMousePosition());

  btnMoveUp.SetPosition({150, 460});
  btnMoveDown.SetPosition({150, 570});
  btnMoveLeft.SetPosition({960, 510});
  btnMoveRight.SetPosition({1100, 510});

  player.updatePlayerControls(maze, camera, btnMoveUp.isPressed(),
                              btnMoveDown.isPressed(), btnMoveLeft.isPressed(),
                              btnMoveRight.isPressed());

  btnMoveUp.drawbutton();
  btnMoveDown.drawbutton();
  btnMoveLeft.drawbutton();
  btnMoveRight.drawbutton();
}

void Menu::DrawExitConfirmMenu(Manager &manager) {

  btnYes.update(GetMousePosition());
  btnNo.update(GetMousePosition());

  btnYes.SetPosition({460, 300});
  btnNo.SetPosition({650, 300});

  if (btnYes.isPressed() || IsKeyPressed(KEY_Y)) {
    manager.exitGame = 1;
    PlaySound(clickSound); // Play button click sound
  }

  if (btnNo.isPressed() || IsKeyPressed(KEY_N)) {
    manager.windowExitRequested = 0;
    PlaySound(clickSound); // Play button click sound
  }

  DrawTexturePro(confirmTexture, {0, 0, 124, 20},
                 {(1280 / 2) - 124 * 4, (720 / 2) - 60 * 4, 124 * 8, 20 * 8},
                 {0, 0}, 0, RAYWHITE);

  btnYes.drawbutton();
  btnNo.drawbutton();
};

void Menu::DrawWinMenu(Manager &manager) {
  auto setupRestartButton = [&]() {
    btnRestart.update(GetMousePosition());
    btnRestart.SetPosition({600, 400});
    if (btnRestart.isPressed() || IsKeyPressed(KEY_R)) {
      PlaySound(clickSound); // Play button click sound
      maze->generateMaze();  // Regenerate maze
      sessionTimer->startTimer();
      maze->showKey = 1;
      player->resetPosition(); // Reset player position
      manager.resetWinState();
      TraceLog(LOG_INFO, "Generating New Maze");
    }
  };

  // DrawRectangle(300, 200, 680, 320, BLACK);

  DrawTexturePro(winBgTexture, {0, 0, 52, 35},
                 {640 - (52 * 6), 360 - (35 * 6), 52 * 12, 35 * 12}, {0, 0}, 0,
                 RAYWHITE);

  DrawText("You Win", 560, 250, 50, BLACK);

  char timeText[50];
  sprintf(timeText, "Finished in : %.0f seconds",
          sessionTimer->getElapsedTime());
  DrawText(timeText, 460, 350, 30, WHITE);
  setupRestartButton();

  btnRestart.drawbutton();
}
