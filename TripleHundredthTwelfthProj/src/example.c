#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
#include "miniphysfs.h"

#define MINIFS_IMPLEMENTATION
#include "minifs.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define OGG_IMPL
#define VORBIS_IMPL
#include "minivorbis.h"

#define MINICORO_IMPL
#include "minicoro.h"

#define DMON_IMPL
#include "dmon.h"
#if DMON_OS_LINUX
#include "dmon_extra.h"
#endif

#define LUA_IMPL
#include "minilua.h"

// #define MINTARO_IMPLEMENTATION
// #include "mintaro.h"
// #define SCREEN_MAIN_MENU    0
// #define SCREEN_IN_GAME      1
// #define SCREEN_OPTIONS      2
// int g_CurrentScreen = SCREEN_MAIN_MENU;
// int g_FocusedMenuItem = 0;
// float g_PlayerPosX = 0;
// float g_PlayerPosY = 0;
// void example1_on_step(mo_context* pContext, double dt)
// {
//     int black = mo_find_closest_color(pContext, mo_make_rgb(0, 0, 0));
//     int white = mo_find_closest_color(pContext, mo_make_rgb(255, 255, 255));
//     int blue  = mo_find_closest_color(pContext, mo_make_rgb(128, 192, 255));
//     mo_clear(pContext, black);
//     switch (g_CurrentScreen)
//     {
//         case SCREEN_MAIN_MENU:
//         {
//             // Input.
//             if (mo_was_button_pressed(pContext, MO_BUTTON_DOWN)) {
//                 g_FocusedMenuItem = (g_FocusedMenuItem + 1) % 3;
//             }
//             if (mo_was_button_pressed(pContext, MO_BUTTON_UP)) {
//                 if (g_FocusedMenuItem == 0) {
//                     g_FocusedMenuItem = 2;
//                 } else {
//                     g_FocusedMenuItem -= 1;
//                 }
//             }
//             if (mo_was_button_pressed(pContext, MO_BUTTON_A) || mo_was_button_pressed(pContext, MO_BUTTON_START)) {
//                 if (g_FocusedMenuItem == 0) {
//                     g_PlayerPosX = (float)(pContext->profile.resolutionX/2 - 16);
//                     g_PlayerPosY = (float)(pContext->profile.resolutionY/2 - 16);
//                     g_CurrentScreen = SCREEN_IN_GAME;
//                 } else if (g_FocusedMenuItem == 1) {
//                     g_CurrentScreen = SCREEN_OPTIONS;
//                 } else if (g_FocusedMenuItem == 2) {
//                     mo_close(pContext);
//                 }
//             }
//             // Graphics.
//             int caretPosX = 4;
//             int caretPosY = 8 + (12*g_FocusedMenuItem);
//             mo_draw_text(pContext, caretPosX, caretPosY, white, ">");
//             mo_draw_text(pContext, 15, 8+12*0, white, "Start Game");
//             mo_draw_text(pContext, 15, 8+12*1, white, "Options");
//             mo_draw_text(pContext, 15, 8+12*2, white, "Quit");
//         } break;
//         case SCREEN_IN_GAME:
//         {
//             // Input.
//             if (mo_is_button_down(pContext, MO_BUTTON_LEFT)) {
//                 g_PlayerPosX -= (float)(100 * dt);
//             }
//             if (mo_is_button_down(pContext, MO_BUTTON_RIGHT)) {
//                 g_PlayerPosX += (float)(100 * dt);
//             }
//             if (mo_is_button_down(pContext, MO_BUTTON_UP)) {
//                 g_PlayerPosY -= (float)(100 * dt);
//             }
//             if (mo_is_button_down(pContext, MO_BUTTON_DOWN)) {
//                 g_PlayerPosY += (float)(100 * dt);
//             }
//             if (mo_was_button_pressed(pContext, MO_BUTTON_START)) {
//                 g_CurrentScreen = SCREEN_MAIN_MENU;
//             }
//             // Graphics.
//             mo_draw_quad(pContext, (int)g_PlayerPosX, (int)g_PlayerPosY, 32, 32, blue);
//             mo_draw_textf(pContext, 4, 4, white, "FPS: %u", (unsigned int)(1/dt));
//         } break;
//         case SCREEN_OPTIONS:
//         {
//             // Input.
//             if (mo_was_button_pressed(pContext, MO_BUTTON_B)) {
//                 g_CurrentScreen = SCREEN_MAIN_MENU;
//             }
//             // Graphics.
//             mo_draw_text(pContext, 8, 8, white, "OPTIONS");
//             mo_draw_text(pContext, 8, 30, white, "Press 'X' to go");
//             mo_draw_text(pContext, 8, 39, white, "back");
//         } break;
//         default:
//         {
//             // Unknown state.
//         } break;
//     }
// }


#include <stdlib.h>
#include <stdio.h>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_encoder* pEncoder = (ma_encoder*)pDevice->pUserData;
    MA_ASSERT(pEncoder != NULL);
    ma_encoder_write_pcm_frames(pEncoder, pInput, frameCount);
    (void)pOutput;
}

// Coroutine entry function.
void coro_entry(mco_coro* co) {
  printf("coroutine 1\n");
  mco_yield(co);
  printf("coroutine 2\n");
}

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user)
{
    (void)(user);
    (void)(watch_id);
    switch (action) {
    case DMON_ACTION_CREATE:
        printf("CREATE: [%s]%s\n", rootdir, filepath);
        break;
    case DMON_ACTION_DELETE:
        printf("DELETE: [%s]%s\n", rootdir, filepath);
        break;
    case DMON_ACTION_MODIFY:
        printf("MODIFY: [%s]%s\n", rootdir, filepath);
        break;
    case DMON_ACTION_MOVE:
        printf("MOVE: [%s]%s -> [%s]%s\n", rootdir, oldfilepath, rootdir, filepath);
        break;
    }
}

int main(int argc, char **argv) {
  if(!PHYSFS_init(argv[0])) {
    printf("PhysFS initialization failed: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return -1;
  }
  PHYSFS_deinit();
  getchar();
  
  
  mfs_result resultfs;
  mfs_iterator iterator;
  mfs_file_info fileInfo;
  (void)argc;
  (void)argv;
  resultfs = mfs_iterator_init("", &iterator);
  if (resultfs != MFS_SUCCESS) {
      printf("Failed to initialize iterator: %d\n", resultfs);
      return (int)resultfs;
  }
  while (mfs_iterator_next(&iterator, &fileInfo) == MFS_SUCCESS) {
      if (fileInfo.isDirectory) {
          printf("DIR:  %s\n", fileInfo.pFileName);
      } else {
          printf("FILE: %s\n", fileInfo.pFileName);
      }
  }
  getchar();
  
  
  ma_result resultaudio;
  ma_encoder_config encoderConfig;
  ma_encoder encoder;
  ma_device_config deviceConfig;
  ma_device device;
  // if (argc < 2) {
  //     printf("No input file.\n");
  //     return -1;
  // }
  encoderConfig = ma_encoder_config_init(ma_resource_format_wav, ma_format_f32, 2, 44100);
  if (ma_encoder_init_file(argc < 2?"claw.ogg":argv[1], &encoderConfig, &encoder) != MA_SUCCESS) {
      printf("Failed to initialize output file.\n");
      return -1;
  }
  deviceConfig = ma_device_config_init(ma_device_type_capture);
  deviceConfig.capture.format   = encoder.config.format;
  deviceConfig.capture.channels = encoder.config.channels;
  deviceConfig.sampleRate       = encoder.config.sampleRate;
  deviceConfig.dataCallback     = data_callback;
  deviceConfig.pUserData        = &encoder;
  resultaudio = ma_device_init(NULL, &deviceConfig, &device);
  if (resultaudio != MA_SUCCESS) {
      printf("Failed to initialize capture device.\n");
      return -2;
  }
  resultaudio = ma_device_start(&device);
  if (resultaudio != MA_SUCCESS) {
      ma_device_uninit(&device);
      printf("Failed to start device.\n");
      return -3;
  }
  printf("Press Enter to stop recording...\n");
  getchar();
  ma_device_uninit(&device);
  ma_encoder_uninit(&encoder);
  getchar();
  
  
  dmon_init();
  puts("waiting for changes ..");
  dmon_watch("./", watch_callback, DMON_WATCHFLAGS_RECURSIVE, NULL);
  getchar();
  dmon_deinit();
  
  
  // mintaro example
  // mo_context* pContext;
  // if (mo_init(NULL, 160*2, 144*2, "Hello, World!", example1_on_step, NULL, &pContext) != MO_SUCCESS) {
  //   return -1;
  // }
  // int resultmo = mo_run(pContext);
  // mo_uninit(pContext);
        
  
   // First initialize a `desc` object through `mco_desc_init`.
  mco_desc desc = mco_desc_init(coro_entry, 0);
  // Configure `desc` fields when needed (e.g. customize user_data or allocation functions).
  desc.user_data = NULL;
  // Call `mco_create` with the output coroutine pointer and `desc` pointer.
  mco_coro* co;
  mco_result rescoro = mco_create(&co, &desc);
  assert(rescoro == MCO_SUCCESS);
  // The coroutine should be now in suspended state.
  assert(mco_status(co) == MCO_SUSPENDED);
  // Call `mco_resume` to start for the first time, switching to its context.
  rescoro = mco_resume(co); // Should print "coroutine 1".
  assert(rescoro == MCO_SUCCESS);
  // We get back from coroutine context in suspended state (because it's unfinished).
  assert(mco_status(co) == MCO_SUSPENDED);
  // Call `mco_resume` to resume for a second time.
  rescoro = mco_resume(co); // Should print "coroutine 2".
  assert(rescoro == MCO_SUCCESS);
  // The coroutine finished and should be now dead.
  assert(mco_status(co) == MCO_DEAD);
  // Call `mco_destroy` to destroy the coroutine.
  rescoro = mco_destroy(co);
  assert(rescoro == MCO_SUCCESS);
  
  
  lua_State *L = luaL_newstate();
  if(L == NULL)
    return -1;
  luaL_openlibs(L);
  luaL_loadstring(L, "print 'hello world'");
  lua_call(L, 0, 0);
  lua_close(L);
  getchar();
  
  
  // if(argc < 2) {
  //   printf("No input file.\n");
  //   return -1;
  // }
  /* Open sound file. */
  FILE* fp = fopen(argc < 2?"claw.ogg":argv[1], "rb");
  if(!fp) {
    printf("Failed to open file '%s'.", argc < 2?"claw.ogg":argv[1]);
    return -1;
  }
  /* Open sound stream. */
  OggVorbis_File vorbis;
  if(ov_open_callbacks(fp, &vorbis, NULL, 0, OV_CALLBACKS_DEFAULT) != 0) {
    printf("Invalid Ogg file '%s'.", argc < 2?"claw.ogg":argv[1]);
    return -1;
  }
  /* Print sound information. */
  vorbis_info* info = ov_info(&vorbis, -1);
  printf("Ogg file %d Hz, %d channels, %d kbit/s.\n", info->rate, info->channels, info->bitrate_nominal / 1024);
  /* Read the entire sound stream. */
  unsigned char buf[4096];
  while(1) {
    int section = 0;
    long bytes = ov_read(&vorbis, buf, sizeof(buf), 0, 2, 1, &section);
    if(bytes <= 0) /* end of file or error */
        break;
  }
  /* Close sound file */
  ov_clear(&vorbis);
  return 0;
}
