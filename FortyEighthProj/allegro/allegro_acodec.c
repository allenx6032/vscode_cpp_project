#include "allegro5/allegro_acodec.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/internal/aintern_acodec_cfg.h"
#include "allegro_acodec.h"

/* Function: al_get_allegro_acodec_version
 */
uint32_t al_get_allegro_acodec_version(void)
{
   return ALLEGRO_VERSION_INT;
}


/* Function: al_init_acodec_addon
 */
bool al_init_acodec_addon(void)
{
   bool ret = true;

   ret &= al_register_sample_loader(".wav", _al_load_wav);
   ret &= al_register_sample_saver(".wav", _al_save_wav);
   ret &= al_register_audio_stream_loader(".wav", _al_load_wav_audio_stream);

   ret &= al_register_sample_loader_f(".wav", _al_load_wav_f);
   ret &= al_register_sample_saver_f(".wav", _al_save_wav_f);
   ret &= al_register_audio_stream_loader_f(".wav", _al_load_wav_audio_stream_f);

   /* buil-in VOC loader */
   ret &= al_register_sample_loader(".voc", _al_load_voc);
   ret &= al_register_sample_loader_f(".voc", _al_load_voc_f);

#ifdef ALLEGRO_CFG_ACODEC_FLAC
   ret &= al_register_sample_loader(".flac", _al_load_flac);
   ret &= al_register_audio_stream_loader(".flac", _al_load_flac_audio_stream);
   ret &= al_register_sample_loader_f(".flac", _al_load_flac_f);
   ret &= al_register_audio_stream_loader_f(".flac", _al_load_flac_audio_stream_f);
#endif

#ifdef ALLEGRO_CFG_ACODEC_MODAUDIO
   ret &= _al_register_dumb_loaders();
#endif

#ifdef ALLEGRO_CFG_ACODEC_VORBIS
   ret &= al_register_sample_loader(".ogg", _al_load_ogg_vorbis);
   ret &= al_register_audio_stream_loader(".ogg", _al_load_ogg_vorbis_audio_stream);
   ret &= al_register_sample_loader_f(".ogg", _al_load_ogg_vorbis_f);
   ret &= al_register_audio_stream_loader_f(".ogg", _al_load_ogg_vorbis_audio_stream_f);
#endif

#ifdef ALLEGRO_CFG_ACODEC_OPUS
   ret &= al_register_sample_loader(".opus", _al_load_ogg_opus);
   ret &= al_register_audio_stream_loader(".opus", _al_load_ogg_opus_audio_stream);
   ret &= al_register_sample_loader_f(".opus", _al_load_ogg_opus_f);
   ret &= al_register_audio_stream_loader_f(".opus", _al_load_ogg_opus_audio_stream_f);
#endif

   return ret;
}


/* vim: set sts=3 sw=3 et: */
