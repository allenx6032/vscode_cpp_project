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

#define LUA_IMPL
#include "minilua.h"

#include <stdlib.h>
#include <stdio.h>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_encoder* pEncoder = (ma_encoder*)pDevice->pUserData;
    MA_ASSERT(pEncoder != NULL);
    ma_encoder_write_pcm_frames(pEncoder, pInput, frameCount);
    (void)pOutput;
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
