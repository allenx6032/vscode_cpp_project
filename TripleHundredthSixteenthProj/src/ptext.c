#include <ptext_buffer.h>
#include <ptext_commands.h>
#include <ptext_config.h>
#include <ptext_files.h>
#include <ptext_input.h>
#include <ptext_output.h>
#include <ptext.h>
#include <ptext_stdafx.h>
#include <stdlib.h>
#include <string.h>
#include <ptext_termio.h>
#include <ptext_utils.h>

struct config conf;

void init(void);
void freeall(void);
void freeall(void);
void done(void);
void printVersion(void);

/*
TODO: output is not working properly

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
_In_ char* pCmdLine, _In_ int nCmdShow)
{
    if (!AllocConsole())
    {
        return EXIT_FAILURE;
    }

    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    main(__argc, __argv);
}
*/

int main(int argc, char *argv[]) {
#ifdef _INC_CRTDBG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif // _INC_CRTDBG

  // TODO(Hoorad): Manage flags correctly
  if (argc >= 2 &&
      (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
    printVersion();
    return EXIT_SUCCESS;
  }

  enableRawMode();
  init();
  if (argc >= 2) {
    openFile(argv[1]);
  }
  while (1) {
    refresh();
    procKey();
  }

  return EXIT_SUCCESS;
}

void printVersion(void) {
  // clang-format off
  printf("\n");
  printf("@@@@@@@@@@@@@@@\n");
  printf("@@@   @@@   @@@    @@@@@@@@@@@@@@@\n");
  printf("@@@   @@@   @@@    @@@@@@@@@@@@@@@\n");
  printf("      @@@                      @@@\n");
  printf("      @@@                      @@@\n");
  printf("      @@@                      @@@\n");
  printf("   @@@@@@@@@                   @@@\n");
  printf("   @@@@@@@@@                   @@@\n");
  printf("                               @@@               Name:           %s\n",NAME);
  printf("                               @@@               Version:        %s\n",VERSION);
  printf("@@@                            @@@               Maintainer:     %s\n",MAINTAINER);
  printf("@@@                            @@@               Licensed Under: %s\n",LICENSE);
  printf("@@@                            @@@\n");
  printf("@@@                            @@@\n");
  printf("@@@                            @@@\n");
  printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  // clang-format on
}

void die(const char *s) {
  perror(s);
  exit(1);
}

void init(void) {
  conf.current_buffer = 0;

  createBuffer(&conf.buffers[0], 1);
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);

  ShowScrollBar(GetConsoleWindow(), SB_VERT, 0);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  COORD newSize = {csbi.srWindow.Right + 1, csbi.srWindow.Bottom + 1};

  conf.width = newSize.X;
  conf.height = newSize.Y;

  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), newSize);
#else
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  conf.width = w.ws_col;
  conf.height = w.ws_row;
#endif // _WIN32

  setStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
  char filename[256];
  strcpy(filename, getenv("HOME"));
  strcat(filename, RCFILE);
  doFile(filename);
  atexit(done);
}

void freeall(void) {
#if FREE_EVERYTHING == 1
  int i;
  for (i = 0; i <= conf.num_buffers; i++) {
    freeBuffer(&conf.buffers[i]);
  }
#endif // FREE_EVERYTHING
}

void done(void) {
#ifdef _INC_CRTDBG
  _CrtDumpMemoryLeaks();
#endif // _INC_CRTDBG

#ifdef _WIN32
  FreeConsole();
#endif // _WIN32
  freeall();
  disableRawMode();
}
