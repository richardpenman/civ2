/* The game, sound, music and video: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00419c8b auto */
void __cdecl snd_menu_end(void)

{
  snd_play_effect(-0x6b,0,1,0);
  snd_play_effect(SND_MENUEND,0,0,0);
  return;
}

/* 0046dff0 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void AviPlayer::set_end_callback(VideoNotifyFn callback)

{
  on_end = callback;
  return;
}

/* 0046e020 auto */
void __cdecl snd_play_effect(int effect,int play_mode,int loop,SoundDoneFn done_callback)

{
  int result;
  char *drive;
  char wav_name[16];
  /* (port) 128 in the original: a game folder path longer than about 110
     characters overran them.  Room for scenario_dir_path (264), SOUND\ and
     the name. */
  char wav_path[0x140];
  i8 local_path[0x140];
  uint snd_flags;
  
  replay_log_sound(effect,play_mode,loop);
  prefs_main_menu_choice_block.snd_last_effect_id = effect;
  prefs_main_menu_choice_block.snd_last_effect_arg = play_mode;
  if (effect < 0) {
    snd_wave_free_by_id(~effect + 1);
  }
  else if ((((byte)game.game_options & OPT_SOUND) != 0) && (effect < 0x85)) {
    crt_strcpy(wav_name,((char *)&str_aircombt) + effect * 9);
    result = _strcmp(wav_name,str_cheers);
    if (result == 0) {
      result = rand_range(0,2);
      str_append_int((iptr)wav_name,result + 1);
    }
    crt_strcat(wav_name,((i8 *)&snd_wav_ext_str));
    str_clear((char *)wav_path);
    str_append((char *)wav_path,(const char *)&str_civ2_sound);
    str_append((char *)wav_path,(const char *)wav_name);
    str_clear((char *)local_path);
    str_append((char *)local_path,(const char *)&net_map_lock_slots_block.scenario_dir_path);
    str_append((char *)local_path,(const char *)&str_sound);
    str_append((char *)local_path,(const char *)wav_name);
    result = file_exists((const char *)local_path);
    if (result == 0) {
      drive = file_get_fallback_drive();
      result = file_resolve_path((const char *)wav_path);
      if (result == 0) {
        return;
      }
      if ((drive != (char *)0x0) && (wav_path[0] == *drive)) {
        return;
      }
    }
    else {
      crt_strcpy(wav_path,local_path);
    }
    snd_flags = 3;
    if (play_mode == 0) {
      snd_flags = 0x13;
    }
    if (loop != 0) {
      snd_flags = snd_flags | 8;
    }
    if (play_mode == 99) {
      sndPlaySoundA(wav_path,snd_flags);
    }
    else {
      snd_wave_play((const char *)wav_path,(loop == 0) - 1 & 8,effect,done_callback);
    }
    win_flush_pending_msgs();
  }
  return;
}

/* 0046e2f4 auto */
void __cdecl snd_cd_on_notify(void)

{
  cd_music.available = 1;
  if (cd_music.keep_playing != 0) {
    snd_sync_music_option();
  }
  return;
}

/* 0046e320 auto */
void __cdecl snd_cd_play_random_track(void)

{
  int rand_offset;
  int rand_offset_alt;
  uint track;
  int track_count;
  int tries;
  
  tries = 0;
  while (tries < 10) {
    tries++;
    track_count = snd_cd_get_track_count();
    if ((((track_count == 0x18) || (track_count == 0x12)) || (track_count == 0xc)) || (track_count == 10)) break;
  }
  if ((track_count == 1) || (track_count == -1)) {
    cd_music.available = 0;
  }
  else {
    while( true ) {
      if (((track_count == 10) || (track_count == 0x12)) || ((track_count == 0x18 || (track_count == 0xc)))) {
        if (track_count == 4 || track_count - 4 < 0) {
          rand_offset = 0;
        }
        else {
          rand_offset = _rand();
          rand_offset = rand_offset % (track_count - 3);
        }
        track = rand_offset + 4;
        cd_music.unknown_disc = 0;
      }
      else {
        if (track_count == 2 || track_count - 2 < 0) {
          rand_offset_alt = 0;
        }
        else {
          rand_offset_alt = _rand();
          rand_offset_alt = rand_offset_alt % (track_count - 1);
        }
        track = rand_offset_alt + 2;
        cd_music.unknown_disc = 1;
      }
      if (track != cd_music.current_track) break;
      cd_music.current_track = 0xffffffff;
    }
    cd_music.available = 0;
    cd_music.current_track = track;
    cd_music.keep_playing = 1;
    snd_cd_play_track(track);
  }
  return;
}

/* 0046e4a9 auto */
void __cdecl snd_cd_detect_layout(void)

{
  int track_count;
  
  snd_cd_set_notify_callback(snd_cd_on_notify);
  track_count = snd_cd_get_track_count();
  if ((track_count == -1) || (track_count == 1)) {
    cd_music.available = 0;
  }
  else if ((track_count == 10) || (((track_count == 0x12 || (track_count == 0x18)) || (track_count == 0xc)))) {
    cd_music.keep_playing = 1;
    cd_music.available = 1;
    cd_music.unknown_disc = 0;
  }
  else {
    cd_music.keep_playing = 1;
    cd_music.available = 1;
    cd_music.unknown_disc = 1;
  }
  return;
}

/* 0046e571 auto */
void __cdecl music_play_track(uint track,int force)

{
  int track_count;
  int tries;
  
  if (((byte)game.game_options & OPT_MUSIC) == 0) {
    if (cd_music.available == 0) {
      snd_cd_stop();
    }
  }
  else if ((((cd_music.available != 0) || (force != 0)) || (cd_music.current_track != track)) &&
          (cd_music.unknown_disc == 0)) {
    tries = 0;
    while (tries < 10) {
      tries++;
      track_count = snd_cd_get_track_count();
      if (((track_count == 0x18) || (track_count == 0x12)) || ((track_count == 0xc || (track_count == 10)))) break;
    }
    if ((track_count == 1) || (track_count == -1)) {
      cd_music.available = 0;
    }
    else {
      cd_music.keep_playing = 0;
      snd_cd_mci_stop();
      snd_cd_set_notify_callback(snd_cd_on_notify);
      snd_cd_play_track(track);
      cd_music.current_track = track;
      cd_music.keep_playing = 1;
      cd_music.available = 0;
    }
  }
  return;
}

/* 0046e6a9 auto */
void __cdecl snd_cd_stop(void)

{
  snd_cd_mci_stop();
  cd_music.available = 1;
  return;
}

/* 0046e6c8 auto */
void __cdecl snd_sync_music_option(void)

{
  if (((byte)game.game_options & OPT_MUSIC) == 0) {
    if (cd_music.available == 0) {
      snd_cd_stop();
    }
  }
  else if (cd_music.available != 0) {
    snd_cd_play_random_track();
  }
  return;
}

/* 00472910 auto */
void AviPlayer::set_frame_callback(VideoNotifyFn callback,u32 callback_data)

{
  on_frame = callback;
  cue_frame_u = callback_data;
  return;
}

/* 00472950: written by hand in src/game/regargs.cpp */

/* 004729ab: written by hand in src/game/regargs.cpp */

/* 004d8b20 auto */
AviPlayer *AviPlayer::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 005d48d0 auto */
void __cdecl snd_shutdown(void)

{
  snd_wave_close_all();
  return;
}

/* 005d48f0 auto */
int __cdecl snd_wave_open(i32 notify_hwnd,i32 buffer_bytes,i32 buffer_count)

{
  int result;
  
  snd_wave.notify_hwnd = notify_hwnd;
  snd_wave.open_buffer_bytes = buffer_bytes;
  (*(i32 *)&snd_wave.max_buffers) = buffer_count;
  (*(i32 *)&snd_wave.buffer_count) = buffer_count;
  result = snd_wave_open_device(notify_hwnd,0,(iptr)&snd_wave.out_handle);
  if (result == 0) {
    snd_wave_alloc_buffers(buffer_bytes,snd_wave.out_handle,buffer_count);
    result = 0;
  }
  return result;
}

/* 005d4965 auto */
void __cdecl snd_wave_close_all(void)

{
  if (snd_wave.out_handle != 0) {
    snd_wave_reset_all(snd_wave.out_handle);
    snd_wave_close_device(snd_wave.out_handle);
  }
  return;
}

/* 005d49a3 auto */
void __cdecl snd_show_error(int error_code)

{
  if (error_code == 10) {
    MessageBoxA((HWND)0x0,str_this_sound_format_is_not_supported,str_sound_error,0x40);
  }
  else if (error_code != 0xb) {
    MessageBoxA((HWND)0x0,str_undefined_sound_error,str_sound_error_2,0x40);
  }
  return;
}

/* 005d4a11 auto */
uint __cdecl snd_wave_open_device(DWORD_PTR callback,i32 *format,iptr out_handle)

{
  bool use_fallback;
  UINT device_count;
  UINT chosen_device;
  UINT_PTR device;
  uint result;
  
  use_fallback = false;
  if (snd_wave.device_open == 0) {
    if (format != (i32 *)0x0) {
      snd_wave.format[0] = *format;
      snd_wave.samples_per_sec = format[1];
      snd_wave.avg_bytes_per_sec = format[2];
      (*(i32 *)&snd_wave.block_align) = format[3];
    }
    device_count = waveOutGetNumDevs();
    for (device = 0; (int)device < (int)device_count; device++) {
      waveOutGetDevCapsA(device,(LPWAVEOUTCAPSA)&default_draw_font_obj_block.snd_wave_dev_caps,0x34);
      if ((default_draw_font_obj_block.snd_wave_dev_formats & 0x20) != 0) {
        use_fallback = false;
        chosen_device = device;
        break;
      }
      if ((default_draw_font_obj_block.snd_wave_dev_formats & 0x10) != 0) {
        chosen_device = device;
        use_fallback = true;
      }
    }
    if ((use_fallback) || (device != device_count)) {
      if (use_fallback) {
        snd_wave.format[0] = CONCAT22(1,(i16)snd_wave.format[0]);
        snd_wave.avg_bytes_per_sec = 0x5622;
        (*(i32 *)&snd_wave.block_align) = ((*(i32 *)&snd_wave.block_align) & 0xffff0000 | (ushort)(1));
      }
      result = waveOutOpen((LPHWAVEOUT)&snd_wave.out_handle,chosen_device,(LPCWAVEFORMATEX)((i32 *)&snd_wave.format),
                            callback,0,0x10000);
      if ((short)result != 0) {
        sndPlaySoundA((LPCSTR)0x0,0);
        result = waveOutOpen((LPHWAVEOUT)&snd_wave.out_handle,chosen_device,(LPCWAVEFORMATEX)((i32 *)&snd_wave.format),
                              callback,0,0x10000);
      }
      result = result & 0xffff;
      if ((result == 0) && (snd_wave.device_open = 1, out_handle != 0)) {
        *(i32 *)out_handle = snd_wave.out_handle;
      }
    }
    else {
      snd_show_error(0xb);
      result = 0xb;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 005d4bcb auto */
uint __cdecl snd_wave_close_device(HWAVEOUT wave_out)

{
  MMRESULT close_result;
  i32 result;
  
  result = 0;
  if (snd_wave.device_open != 0) {
    close_result = waveOutClose(wave_out);
    result = close_result & 0xffff;
  }
  snd_wave.device_open = 0;
  return result;
}

/* 005d4c18 auto */
void __cdecl snd_waveout_error_box(uint error_code)

{
  CHAR error_text[200];
  
  waveOutGetErrorTextA(error_code & 0xffff,error_text,200);
  MessageBoxA((HWND)0x0,error_text,str_wave_out_error,0x40);
  return;
}

/* BF-111/BF-305: only the first 16 fmt bytes are retained. Extensions are
 * consumed, never read into adjacent locals; truncation is still a read error.
 * Return the loaders' existing error codes (10 format, 8 read). */
static int snd_read_pcm_format(HMMIO mmio,DWORD size,WAVEFORMATEX *format)
{
  char discard[256];
  LONG prefix = size < 16 ? (LONG)size : 16;
  if (mmioRead(mmio,(HPSTR)format,prefix) != prefix) {
    return 8;
  }
  if (size < 16) {
    return 10;
  }
  size -= 16;
  while (size != 0) {
    LONG count = size < sizeof discard ? (LONG)size : (LONG)sizeof discard;
    if (mmioRead(mmio,discard,count) != count) {
      return 8;
    }
    size -= count;
  }
  return 0;
}

/* 005d4c5f auto */
i16 __cdecl snd_wave_load_file(LPSTR filename,i32 unused,i32 flags,SoundDoneFn done_callback)

{
  i16 result;
  MMRESULT mmio_result;
  int n;
  _MMIOINFO *p_Var5;
  _MMCKINFO *p_Var6;
  DWORD *dst_flags;
  FOURCC *dst_ckid;
  _MMCKINFO chunk;
  _MMIOINFO mmio_info;
  WaveSource *node2;
  DWORD fmt_size;
  WaveSource *source;
  HGLOBAL mem_handle2;
  WAVEFORMATEX wave_format;
  _MMCKINFO riff_chunk;
  HMMIO mmio;
  
  _memset(&mmio_info,0,0x48);
  mmio_info.cchBuffer = snd_wave.total_buffer_bytes;
  mmio = mmioOpenA(filename,&mmio_info,0x10000);
  if (mmio == (HMMIO)0x0) {
    result = 7;
  }
  else {
    riff_chunk.fccType = 0x45564157;
    mmio_result = mmioDescend(mmio,&riff_chunk,(MMCKINFO *)0x0,0x20);
    if (mmio_result == 0) {
      chunk.ckid = 0x20746d66;
      mmio_result = mmioDescend(mmio,&chunk,&riff_chunk,0x10);
      if (mmio_result == 0) {
        fmt_size = chunk.cksize;
        result = (i16)snd_read_pcm_format(mmio,fmt_size,&wave_format);
        if (result == 0) {
          if ((wave_format.nSamplesPerSec == 0x5622) && (wave_format.wBitsPerSample == 8)) {
            mmioAscend(mmio,&chunk,0);
            chunk.ckid = 0x61746164;
            mmio_result = mmioDescend(mmio,&chunk,&riff_chunk,0x10);
            if (mmio_result == 0) {
              mmioGetInfo(mmio,&mmio_info,0);
              mmioAdvance(mmio,&mmio_info,0);
              mem_handle2 = GlobalAlloc(0x2002,0xbc);
              if (mem_handle2 == (HGLOBAL)0x0) {
                mmioClose(mmio,0);
                result = 2;
              }
              else {
                source = (WaveSource *)GlobalLock(mem_handle2);
                if (source == (WaveSource *)0x0) {
                  mmioClose(mmio,0);
                  GlobalFree(mem_handle2);
                  result = 2;
                }
                else {
                  _memset(source,0,0xbc);
                  source->mem_handle = mem_handle2;
                  source->mmio = mmio;
                  p_Var5 = &mmio_info;
                  dst_flags = &source->io_flags;
                  for (n = 0x12; n != 0; n--) {
                    *dst_flags = p_Var5->dwFlags;
                    p_Var5 = (_MMIOINFO *)&p_Var5->fccIOProc;
                    dst_flags++;
                  }
                  p_Var6 = &chunk;
                  dst_ckid = (FOURCC *)source;
                  for (n = 5; dst_ckid = dst_ckid + 1, n != 0; n--) {
                    *dst_ckid = p_Var6->ckid;
                    p_Var6 = (_MMCKINFO *)&p_Var6->cksize;
                  }
                  source->sound_id = (i16)flags;
                  source->format_tag_channels = wave_format.wFormatTag | (u32)wave_format.nChannels << 16;
                  source->samples_per_sec = wave_format.nSamplesPerSec;
                  source->avg_bytes_per_sec = wave_format.nAvgBytesPerSec;
                  source->block_align_bits = wave_format.nBlockAlign | (u32)wave_format.wBitsPerSample << 16;
                  source->kind = 1;
                  source->done_callback = done_callback;
                  if (snd_wave.sources == 0) {
                    snd_wave.sources = source;
                  }
                  else {
                    for (node2 = snd_wave.sources; node2->next != 0;
                        node2 = node2->next) {
                    }
                    node2->next = source;
                  }
                  result = 0;
                }
              }
            }
            else {
              mmioClose(mmio,0);
              result = 8;
            }
          }
          else {
            snd_show_error(10);
            mmioClose(mmio,0);
            result = 10;
          }
        }
        else {
          if (result == 10) {
            snd_show_error(10);
          }
          mmioClose(mmio,0);
        }
      }
      else {
        mmioClose(mmio,0);
        result = 8;
      }
    }
    else {
      mmioClose(mmio,0);
      result = 8;
    }
  }
  return result;
}

/* 005d4f6a auto */
i16 __cdecl snd_wave_open_stream(LPSTR filename,i32 unused,i32 flags)

{
  i16 result;
  MMRESULT mmio_result;
  int n;
  _MMIOINFO *p_Var5;
  _MMCKINFO *p_Var6;
  DWORD *dst_flags;
  FOURCC *dst_ckid;
  _MMCKINFO chunk;
  _MMIOINFO mmio_info;
  WaveSource *node2;
  DWORD fmt_size;
  WaveSource *source;
  HGLOBAL mem_handle2;
  WAVEFORMATEX wave_format;
  _MMCKINFO riff_chunk;
  HMMIO file_mmio;
  HMMIO mem_mmio;
  
  file_mmio = mmioOpenA(filename,(LPMMIOINFO)0x0,0);
  if (file_mmio == (HMMIO)0x0) {
    result = 7;
  }
  else {
    riff_chunk.fccType = 0x45564157;
    mmio_result = mmioDescend(file_mmio,&riff_chunk,(MMCKINFO *)0x0,0x20);
    if (mmio_result == 0) {
      chunk.ckid = 0x20746d66;
      mmio_result = mmioDescend(file_mmio,&chunk,&riff_chunk,0x10);
      if (mmio_result == 0) {
        fmt_size = chunk.cksize;
        result = (i16)snd_read_pcm_format(file_mmio,fmt_size,&wave_format);
        if (result == 0) {
          if ((wave_format.nSamplesPerSec == 0x5622) && (wave_format.wBitsPerSample == 8)) {
            mmioAscend(file_mmio,&chunk,0);
            chunk.ckid = 0x61746164;
            mmio_result = mmioDescend(file_mmio,&chunk,&riff_chunk,0x10);
            if (mmio_result == 0) {
              _memset(&mmio_info,0,0x48);
              mmio_info.cchBuffer = snd_wave.total_buffer_bytes;
              mmio_info.fccIOProc = 0x204d454d;
              mmio_info.adwInfo[0] = 0;
              mem_mmio = mmioOpenA((LPSTR)0x0,&mmio_info,0x11002);
              if (mem_mmio == (HMMIO)0x0) {
                result = 7;
              }
              else {
                mmioGetInfo(mem_mmio,&mmio_info,0);
                mem_handle2 = GlobalAlloc(0x2002,0xbc);
                if (mem_handle2 == (HGLOBAL)0x0) {
                  result = 2;
                }
                else {
                  source = (WaveSource *)GlobalLock(mem_handle2);
                  if (source == (WaveSource *)0x0) {
                    GlobalFree(mem_handle2);
                    result = 2;
                  }
                  else {
                    _memset(source,0,0xbc);
                    source->mem_handle = mem_handle2;
                    source->kind = 1;
                    source->format_tag_channels = wave_format.wFormatTag | (u32)wave_format.nChannels << 16;
                    source->samples_per_sec = wave_format.nSamplesPerSec;
                    source->avg_bytes_per_sec = wave_format.nAvgBytesPerSec;
                    source->block_align_bits = wave_format.nBlockAlign | (u32)wave_format.wBitsPerSample << 16;
                    source->mmio = mem_mmio;
                    source->file_mmio = file_mmio;
                    p_Var5 = &mmio_info;
                    dst_flags = &source->io_flags;
                    for (n = 0x12; n != 0; n--) {
                      *dst_flags = p_Var5->dwFlags;
                      p_Var5 = (_MMIOINFO *)&p_Var5->fccIOProc;
                      dst_flags++;
                    }
                    p_Var6 = &chunk;
                    dst_ckid = (FOURCC *)source;
                    for (n = 5; dst_ckid = dst_ckid + 1, n != 0; n--) {
                      *dst_ckid = p_Var6->ckid;
                      p_Var6 = (_MMCKINFO *)&p_Var6->cksize;
                    }
                    source->flags = source->flags | 4;
                    source->sound_id = (i16)flags;
                    snd_wave_stream_refill(source);
                    if (snd_wave.sources == 0) {
                      snd_wave.sources = source;
                    }
                    else {
                      for (node2 = snd_wave.sources; node2->next != 0;
                          node2 = node2->next) {
                      }
                      node2->next = source;
                    }
                    result = 0;
                  }
                }
              }
            }
            else {
              mmioClose(file_mmio,0);
              result = 8;
            }
          }
          else {
            snd_show_error(10);
            mmioClose(file_mmio,0);
            result = 10;
          }
        }
        else {
          if (result == 10) {
            snd_show_error(10);
          }
          mmioClose(file_mmio,0);
        }
      }
      else {
        mmioClose(file_mmio,0);
        result = 8;
      }
    }
    else {
      mmioClose(file_mmio,0);
      result = 8;
    }
  }
  return result;
}

/* 005d52a2 auto */
i32 __cdecl snd_wave_add_avi_stream(int avi_stream)

{
  i32 result;
  int n;
  _MMIOINFO *p_Var3;
  DWORD *dst_flags;
  _MMIOINFO mmio_info;
  WAVEFORMATEX wave_format;
  WaveSource *source;
  HGLOBAL mem_handle2;
  i8 sample_buf[4];
  uint bytes_per_buffer;
  int total_bytes;
  i32 format_size;
  uint size_bits;
  AVISTREAMINFOA stream_info;
  HMMIO mmio;
  i32 rate_mode;
  
  format_size = 0xe;
  if (snd_wave.out_handle == 0) {
    result = 3;
  }
  else if (avi_stream == 0) {
    result = 0xc;
  }
  else if ((snd_wave.sources == 0) || (snd_wave.sources->kind != 4)) {
    AVIStreamInfoA(avi_stream,&stream_info,0x8c);
    if (stream_info.fccType == 0x73647561) {
      AVIStreamRead(avi_stream,0,1,0,0,(LONG *)sample_buf,0);
      total_bytes = stream_info.dwSampleSize * stream_info.dwLength;
      AVIStreamReadFormat(avi_stream,0,&wave_format,&format_size);
      if ((*(i16 *)((i8 *)&snd_wave.format + 2)) == 1) {
        if (wave_format.nChannels == 2) {
          rate_mode = (rate_mode & 0xffff0000 | (ushort)(1));
        }
        else {
          rate_mode = (rate_mode & 0xffff0000 | (ushort)(2));
        }
      }
      else if (wave_format.nChannels == 2) {
        rate_mode = rate_mode & 0xffff0000;
      }
      else {
        rate_mode = (rate_mode & 0xffff0000 | (ushort)(3));
      }
      _memset(&mmio_info,0,0x48);
      if ((rate_mode & 0xffff) == 1) {
        mmio_info.cchBuffer = (*(uint *)&snd_wave.total_buffer_bytes) * 2;
      }
      else if ((rate_mode & 0xffff) == 3) {
        mmio_info.cchBuffer = (*(uint *)&snd_wave.total_buffer_bytes) >> 1;
      }
      else {
        mmio_info.cchBuffer = (*(uint *)&snd_wave.total_buffer_bytes);
      }
      bytes_per_buffer = (uint)mmio_info.cchBuffer / snd_wave.max_buffers;
      mmio_info.fccIOProc = 0x204d454d;
      mmio_info.adwInfo[0] = 0;
      mmio = mmioOpenA((LPSTR)0x0,&mmio_info,0x11002);
      if (mmio == (HMMIO)0x0) {
        result = 7;
      }
      else {
        mmioGetInfo(mmio,&mmio_info,0);
        mem_handle2 = GlobalAlloc(0x2002,0xbc);
        if (mem_handle2 == (HGLOBAL)0x0) {
          result = 2;
        }
        else {
          source = (WaveSource *)GlobalLock(mem_handle2);
          if (source == (WaveSource *)0x0) {
            GlobalFree(mem_handle2);
            result = 2;
          }
          else {
            _memset(source,0,0xbc);
            source->mem_handle = mem_handle2;
            source->kind = 4;
            source->format_tag_channels = wave_format.wFormatTag | (u32)wave_format.nChannels << 16;
            source->samples_per_sec = wave_format.nSamplesPerSec;
            source->avg_bytes_per_sec = wave_format.nAvgBytesPerSec;
            source->block_align_bits = wave_format.nBlockAlign | (u32)wave_format.wBitsPerSample << 16;
            source->mmio = mmio;
            p_Var3 = &mmio_info;
            dst_flags = &source->io_flags;
            for (n = 0x12; n != 0; n--) {
              *dst_flags = p_Var3->dwFlags;
              p_Var3 = (_MMIOINFO *)&p_Var3->fccIOProc;
              dst_flags++;
            }
            source->data_ckid = stream_info.fccType;
            source->data_size = total_bytes;
            source->avi_stream = avi_stream;
            source->flags = source->flags | 2;
            source->buffer_bytes = bytes_per_buffer;
            for (size_bits = mmio_info.cchBuffer; 1 < size_bits; size_bits = size_bits >> 1) {
              source->size_mask = source->size_mask << 1;
              source->size_mask++;
            }
            source->next = snd_wave.sources;
            snd_wave.sources = source;
            snd_wave.source_count++;
            result = 0;
          }
        }
      }
    }
    else {
      result = 9;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 005d5643: written by hand in src/game/sound_position.cpp */

/* 005d5706: written by hand in src/game/sound_position.cpp */

/* 005d57b1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl snd_avi_stream_feed(uint sample_count)

{
  i32 result;
  uint limit;
  WaveSource *source;
  uint byte_count;
  uint i;
  WaveBuffer *buffer;
  int bytes_read;
  
  source = snd_wave.sources;
  if (snd_wave.out_handle == 0) {
    result = 3;
  }
  else if (snd_wave.avi_stream_started == 0) {
    result = 0xe;
  }
  else if ((snd_wave.sources == 0) || (snd_wave.sources->kind != 4)) {
    result = 5;
  }
  else if (((u8)snd_wave.sources->flags & 1) == 0) {
    if ((snd_wave.sources->fill_buffer->flags >> 3 & 1) == 0) {
      if (((snd_wave.sources->fill_buffer->flags >> 7) -
           (snd_wave.cur_buffer->flags >> 7) & 7) == 0) {
        sample_count = (uint)(snd_wave.sources->buffer_bytes - snd_wave.sources->buffer_fill) /
                  (uint)snd_wave.sources->block_align;
        snd_wave.avi_fill_count++;
      }
      buffer = snd_wave.sources->fill_buffer;
      limit = (snd_wave.sources->buf_end - snd_wave.sources->buf_next) /
              (int)(uint)snd_wave.sources->block_align;
      if (limit < sample_count) {
        sample_count = limit;
      }
      byte_count = snd_wave.sources->block_align * sample_count;
      limit = snd_wave.sources->buffer_bytes - snd_wave.sources->buffer_fill;
      snd_wave.avi_feed_samples = sample_count;
      if (limit < byte_count) {
        byte_count = limit;
      }
      limit = snd_wave.sources->data_size -
              (snd_wave.sources->stream_pos - snd_wave.sources->stream_start);
      if (limit < byte_count) {
        byte_count = limit;
      }
      bytes_read = 0;
      AVIStreamRead(snd_wave.sources->avi_stream,
                    (uint)snd_wave.sources->stream_pos / (uint)snd_wave.sources->block_align,
                    byte_count / snd_wave.sources->block_align,
                    (LPVOID)snd_wave.sources->buf_next,byte_count,&bytes_read,0);
      snd_wave.sources->stream_pos = snd_wave.sources->stream_pos + bytes_read;
      source->buf_next = source->buf_next + bytes_read;
      if (source->buf_end == source->buf_next) {
        source->buf_next = source->buf_start;
      }
      snd_wave.sources->buffer_fill = snd_wave.sources->buffer_fill + bytes_read;
      if ((uint)snd_wave.sources->buffer_bytes <= (uint)snd_wave.sources->buffer_fill) {
        buffer->flags = buffer->flags | 8;
        snd_wave.sources->fill_buffer = buffer->next;
        buffer = snd_wave.sources->fill_buffer;
        snd_wave.sources->buffer_fill = 0;
      }
      if ((uint)snd_wave.sources->data_size <=
          (uint)(snd_wave.sources->stream_pos - snd_wave.sources->stream_start)) {
        for (i = 0;
            i < (uint)(snd_wave.sources->buffer_bytes - snd_wave.sources->buffer_fill);
            i = i + 1) {
          *(i8 *)source->buf_next = 0x80;
          source->buf_next = source->buf_next + 1;
        }
        if (source->buf_end == source->buf_next) {
          source->buf_next = source->buf_start;
        }
        buffer->flags = buffer->flags | 8;
        buffer->flags = buffer->flags | 2;
        snd_wave.sources->fill_buffer = buffer->next;
        snd_wave.sources->buffer_fill = 0;
        snd_wave.sources->flags = snd_wave.sources->flags | 1;
      }
      snd_wave.avi_feed_count++;
      result = 0;
    }
    else {
      result = 0;
    }
  }
  else {
    result = 0xe;
  }
  return result;
}

/* 005d5b88 auto */
i32 __cdecl snd_avi_stream_stop(void)

{
  i32 result;
  
  snd_wave.avi_stream_started = 0;
  if ((snd_wave.sources == 0) || (snd_wave.sources->kind != 4)) {
    result = 5;
  }
  else {
    snd_wave.sources = snd_wave_free(0,snd_wave.sources);
    result = 0;
  }
  return result;
}

/* 005d5bec auto */
void __cdecl snd_avi_stream_refill(WaveSource *source)

{
  uint remaining;
  int shortfall;
  uint byte_count;
  int bytes_read;
  
  bytes_read = 0;
  if ((source->buf_end == source->buf_next) ||
     (source->buf_start == source->buf_next)) {
    remaining = snd_wave.sources->data_size -
            (snd_wave.sources->stream_pos - snd_wave.sources->stream_start);
    byte_count = source->buf_size;
    if (remaining < source->buf_size) {
      byte_count = remaining;
    }
    source->buf_next = source->buf_start;
    AVIStreamRead(source->avi_stream,
                  (uint)source->stream_pos / (uint)source->block_align,
                  byte_count / source->block_align,(LPVOID)source->buf_next,byte_count,
                  &bytes_read,0);
    source->stream_pos = source->stream_pos + bytes_read;
    shortfall = (int)source->buf_size - bytes_read;
    if (shortfall != 0) {
      snd_fill_silence_u8(source->buf_next + bytes_read,shortfall);
    }
  }
  return;
}

/* 005d5d11 auto */
i32 __cdecl snd_avi_stream_prepare(i32 start_time,i32 end_time,iptr status_ptr)

{
  i32 result;
  int start_byte;
  int byte_length;
  uint i;
  WaveBuffer *buffer;
  
  if (snd_wave.out_handle == 0) {
    result = 3;
  }
  else if ((snd_wave.sources == 0) || (snd_wave.sources->kind != 4)) {
    result = 5;
  }
  else {
    if (snd_wave.buffer_count != snd_wave.max_buffers) {
      snd_wave_set_buffer_count(snd_wave.max_buffers);
    }
    start_byte = AVIStreamTimeToSample(snd_wave.sources->avi_stream,start_time);
    byte_length = AVIStreamTimeToSample(snd_wave.sources->avi_stream,end_time);
    byte_length = (uint)snd_wave.sources->block_align * byte_length;
    start_byte = (uint)snd_wave.sources->block_align * start_byte;
    if (byte_length == 0) {
      byte_length = snd_wave.sources->data_size - start_byte;
    }
    else {
      byte_length -= start_byte;
    }
    if (byte_length < 0) {
      result = 0xc;
    }
    else {
      snd_wave_stop_all();
      snd_wave.sources->data_size = byte_length;
      snd_wave.sources->stream_pos = start_byte;
      snd_wave.sources->stream_start = start_byte;
      snd_wave.sources->play_pos = 0;
      snd_wave.sources->buffer_fill = 0;
      snd_wave.sources->buf_next = snd_wave.sources->buf_end;
      snd_avi_stream_refill(snd_wave.sources);
      snd_wave.sources->flags = snd_wave.sources->flags & 0xfffffffd;
      snd_wave.sources->flags = snd_wave.sources->flags & 0xfffffffe;
      snd_wave.sources->status_ptr = status_ptr;
      snd_wave.avi_stream_started = 0;
      buffer = snd_wave.buffer_ring;
      for (i = 0; i < snd_wave.max_buffers; i++) {
        buffer->flags = buffer->flags & 0xffffffdf;
        buffer->flags = buffer->flags & 0xfffffffe;
        buffer->flags = buffer->flags & 0xfffffffd;
        buffer->flags = buffer->flags & 0xffffffef;
        buffer->flags = buffer->flags & 0xffffffdf;
        buffer->flags = buffer->flags & 0xffffffbf;
        buffer->data = buffer->hdr->data;
        start_byte = snd_wave_mix_buffer(buffer->hdr);
        if (start_byte == 1) break;
        buffer = buffer->next;
      }
      snd_avi_stream_refill(snd_wave.sources);
      buffer = snd_wave.buffer_ring;
      for (i = 0; i < snd_wave.max_buffers; i++) {
        buffer->flags = buffer->flags | 8;
        buffer = buffer->next;
      }
      snd_wave.cur_buffer = snd_wave.buffer_ring;
      result = 0;
    }
  }
  return result;
}

/* 005d5f91 auto */
i16 __cdecl snd_avi_stream_start(void)

{
  iptr ring;
  
  if ((snd_wave.sources != 0) && (snd_wave.sources->kind == 4)) {
    snd_wave_write_pending(snd_wave.buffer_ring);
    snd_wave.sources->flags = snd_wave.sources->flags | 0x10;
    if (snd_wave.sources->status_ptr != 0) {
      *(ushort *)snd_wave.sources->status_ptr = *(ushort *)snd_wave.sources->status_ptr | 4;
    }
  }
  ring = (iptr)snd_wave.buffer_ring;
  snd_wave.avi_stream_started = 1;
  snd_wave.sources->fill_buffer = snd_wave.buffer_ring;
  return (short)ring;
}

/* 005d6038 auto */
short __cdecl snd_wave_play(const char *filename,uint flags,i32 sound_id,SoundDoneFn done_callback)

{
  short result;
  int status;
  uint i;
  WaveBuffer *buffer;
  
  if (snd_wave.out_handle == 0) {
    result = 3;
  }
  else if ((snd_wave.device_open == 0) &&
          (status = snd_wave_open(snd_wave.notify_hwnd,snd_wave.open_buffer_bytes,(*(i32 *)&snd_wave.max_buffers)), status != 0)) {
    result = 0;
  }
  else if ((*(ushort *)&snd_wave.source_count) < 5) {
    if ((flags & 8) == 0) {
      result = snd_wave_load_file((LPSTR)filename,flags,sound_id,done_callback);
    }
    else {
      result = snd_wave_open_stream((LPSTR)filename,flags,sound_id);
    }
    if ((result == 0) &&
       ((*(ushort *)&snd_wave.source_count) = (*(ushort *)&snd_wave.source_count) + 1, (snd_wave.cur_buffer->flags >> 5 & 1) == 0)) {
      i = 0;
      buffer = snd_wave.cur_buffer;
      while ((i < snd_wave.buffer_count && (status = snd_wave_mix_buffer(buffer->hdr), status != 1))) {
        buffer = buffer->next;
        i++;
      }
      snd_wave_write_pending(snd_wave.cur_buffer);
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 005d61ab auto */
void __cdecl snd_wave_free_by_id(i32 sound_id)

{
  WaveSource *source;
  WaveSource *prev;
  
  prev = 0;
  source = snd_wave.sources;
  while (source != 0) {
    if (source->sound_id == (short)sound_id) {
      source = snd_wave_free(prev,source);
    }
    else {
      prev = source;
      source = source->next;
    }
  }
  return;
}

/* 005d6283 auto */
void __cdecl snd_wave_stream_refill(WaveSource *source)

{
  int n;
  i32 *src;
  i32 *dst;
  MMIOINFO mmio_info;
  int done;
  int remaining;
  int leftover;
  int space;
  
  remaining = 0;
  done = 0;
  src = (i32 *)&source->io_flags;
  dst = (i32 *)&mmio_info;
  for (n = 0x12; n != 0; n--) {
    *dst = *src;
    src++;
    dst++;
  }
  if (mmio_info.pchBuffer == mmio_info.pchNext) {
    leftover = 0;
    space = mmio_info.cchBuffer;
  }
  else {
    leftover = (int)mmio_info.pchEndWrite - (int)mmio_info.pchNext;
    space = mmio_info.cchBuffer - leftover;
  }
  if (leftover == 0) {
    mmio_info.pchNext = mmio_info.pchBuffer;
  }
  else {
    crt_memmove(mmio_info.pchBuffer,mmio_info.pchNext,leftover);
    mmio_info.pchNext = mmio_info.pchBuffer + leftover;
  }
  while (done == 0) {
    remaining = source->data_size - source->buffer_fill;
    if (space < remaining) {
      mmioRead(source->file_mmio,mmio_info.pchNext,space);
      source->buffer_fill = source->buffer_fill + space;
      source->buf_next = (i32)mmio_info.pchBuffer;
      done = 1;
    }
    else {
      mmioRead(source->file_mmio,mmio_info.pchNext,remaining);
      source->buffer_fill = 0;
      mmio_info.pchNext += remaining;
      space = (int)mmio_info.pchEndWrite - (int)mmio_info.pchNext;
      mmioSeek(source->file_mmio,source->data_offset,0);
    }
  }
  return;
}

/* 005d63c5 auto */
LPVOID __cdecl snd_global_alloc_lock(SIZE_T size,iptr handle_out)

{
  HGLOBAL handle;
  LPVOID ptr;
  
  handle = GlobalAlloc(0x2002,size);
  *(HGLOBAL *)handle_out = handle;
  if (*(int *)handle_out == 0) {
    ptr = (LPVOID)0x0;
  }
  else {
    ptr = GlobalLock(*(HGLOBAL *)handle_out);
    if (ptr == (LPVOID)0x0) {
      GlobalFree(*(HGLOBAL *)handle_out);
      *(i32 *)handle_out = 0;
    }
  }
  return ptr;
}

/* 005d6430 auto */
void __cdecl snd_global_free(HGLOBAL handle)

{
  if (handle != (HGLOBAL)0x0) {
    GlobalUnlock(handle);
    GlobalFree(handle);
  }
  return;
}

/* 005d645e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

MMRESULT __cdecl snd_wave_alloc_buffers(uint total_bytes,HWAVEOUT wave_out,uint buffer_count)

{
  int data;
  int mem_handle2;
  WaveBuffer *prev;
  uint i;
  MMRESULT result;
  uint bytes_per_buffer;
  WaveBuffer *buffer;
  
  buffer = (WaveBuffer *)0x0;
  bytes_per_buffer = total_bytes / buffer_count;
  if (snd_wave.device_open == 0) {
    result = 3;
  }
  else {
    for (i = 0; i < buffer_count; i++) {
      prev = buffer;
      buffer = (WaveBuffer *)snd_global_alloc_lock(0x20,(iptr)&mem_handle2);
      if (buffer == (WaveBuffer *)0x0) {
        if (i != 0) {
          snd_wave_reset_all(wave_out);
        }
        return 4;
      }
      if (i == 0) {
        snd_wave.buffer_ring = buffer;
        snd_wave.cur_buffer = buffer;
      }
      _memset(buffer,0,0x20);
      buffer->handle = mem_handle2;
      data = (int)snd_global_alloc_lock(bytes_per_buffer + 0x20,(iptr)&mem_handle2);
      buffer->hdr = (WaveHeader *)data;
      if (buffer->hdr == 0) {
        if (i != 0) {
          snd_wave_reset_all(wave_out);
        }
        return 4;
      }
      _memset(buffer->hdr,0,0x20);
      buffer->hdr->data = (iptr)buffer->hdr + 0x20;
      buffer->data = buffer->hdr->data;
      buffer->hdr->length = bytes_per_buffer;
      buffer->length = buffer->hdr->length;
      buffer->data_end = buffer->hdr->data + buffer->length;
      buffer->hdr->owner = buffer;
      buffer->data_handle = mem_handle2;
      buffer->flags = buffer->flags & 0xffff807fU | (i & 0xff) << 7;
      result = waveOutPrepareHeader(wave_out,(LPWAVEHDR)buffer->hdr,0x20);
      if (result != 0) {
        snd_waveout_error_box(result);
        snd_wave_reset_all(wave_out);
        return 5;
      }
      buffer->hdr->flags = buffer->hdr->flags | 1;
      if (prev != (WaveBuffer *)0x0) {
        prev->next = buffer;
      }
      result = 0;
    }
    buffer->next = snd_wave.buffer_ring;
    snd_wave.buffers_ready = 1;
    (*(uint *)&snd_wave.total_buffer_bytes) = total_bytes;
  }
  return result;
}

/* 005d6685 auto */
void __cdecl snd_wave_add_buffers(int count)

{
  WaveBuffer *next_spare;
  uint i;
  WaveBuffer *tail2;
  WaveBuffer *buffer;
  
  buffer = snd_wave.spare_buffers;
  snd_wave.spare_buffers->flags = snd_wave.spare_buffers->flags & 0xfffffffe;
  for (i = 0; (int)i < count - 1; i++) {
    buffer = buffer->next;
    buffer->flags = buffer->flags & 0xfffffffe;
  }
  tail2 = snd_wave.buffer_ring;
  for (i = 0; i < (*(int *)&snd_wave.buffer_count) - 1U; i++) {
    tail2 = tail2->next;
  }
  next_spare = buffer->next;
  tail2->next = snd_wave.spare_buffers;
  snd_wave.spare_buffers = next_spare;
  buffer->next = snd_wave.buffer_ring;
  (*(int *)&snd_wave.buffer_count) += count;
  return;
}

/* 005d673a auto */
void __cdecl snd_wave_remove_buffers(int count)

{
  WaveBuffer *removed;
  int i;
  WaveBuffer *buffer;
  
  buffer = snd_wave.buffer_ring;
  if ((uint)((*(int *)&snd_wave.buffer_count) - count) <= (snd_wave.cur_buffer->flags >> 7 & 0xff)) {
    snd_wave.cur_buffer = snd_wave.buffer_ring;
  }
  for (i = 0; i < (int)(((*(int *)&snd_wave.buffer_count) - count) - 1U); i++) {
    buffer = buffer->next;
  }
  removed = buffer->next;
  buffer->next = snd_wave.buffer_ring;
  buffer = removed;
  for (i = 0; i < count; i++) {
    buffer->flags = buffer->flags & 0xfffffffd;
    buffer->flags = buffer->flags & 0xfffffffb;
    buffer->flags = buffer->flags | 1;
    buffer->flags = buffer->flags & 0xfffffff7;
    buffer->flags = buffer->flags & 0xffffffef;
    buffer->flags = buffer->flags & 0xffffffdf;
    buffer->flags = buffer->flags & 0xffffffbf;
    buffer = buffer->next;
  }
  buffer = removed;
  for (i = 0; i < count - 1; i++) {
    buffer = buffer->next;
  }
  buffer->next = snd_wave.spare_buffers;
  snd_wave.spare_buffers = removed;
  (*(int *)&snd_wave.buffer_count) -= count;
  return;
}

/* 005d687b auto */
void __cdecl snd_wave_set_buffer_count(uint count)

{
  if ((snd_wave.out_handle != 0) &&
     (((snd_wave.sources == 0 || (snd_wave.sources->kind != 4)) ||
      ((snd_wave.sources->flags >> 1 & 1) != 0)))) {
    if (snd_wave.max_buffers < count) {
      count = snd_wave.max_buffers;
    }
    if ((int)count < 2) {
      count = 2;
    }
    if (snd_wave.buffer_count < count) {
      snd_wave_add_buffers(count - snd_wave.buffer_count);
    }
    else if (count < snd_wave.buffer_count) {
      snd_wave_remove_buffers(snd_wave.buffer_count - count);
    }
  }
  return;
}

/* 005d6947 auto */
i32 __cdecl snd_wave_write_pending(WaveBuffer *buffer)

{
  MMRESULT write_result;
  uint i;
  uint result;
  byte *sample;
  
  result = result & 0xffff0000;
  while ((buffer->flags >> 6 & 1) != 0) {
    sample = (byte *)buffer->hdr->data;
    for (i = 0; i < buffer->hdr->length; i++) {
      *sample = *sample ^ 0x80;
      sample++;
    }
    write_result = waveOutWrite((*(HWAVEOUT *)&snd_wave.out_handle),(LPWAVEHDR)buffer->hdr,0x20);
    result = (result & 0xffff0000 | (ushort)((short)write_result));
    if ((short)write_result == 0) {
      buffer->flags = buffer->flags | 0x20;
      buffer->flags = buffer->flags & 0xffffffbf;
    }
    else {
      if ((write_result & 0xffff) != 0x21) {
        snd_waveout_error_box(result);
      }
      buffer->flags = buffer->flags & 0xffffffdf;
    }
    buffer = buffer->next;
  }
  return 0;
}

/* 005d6a2c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl snd_wave_stop_all(void)

{
  BOOL got_msg;
  tagMSG msg;
  WaveSource *source;
  uint i;
  WaveSource *prev;
  WaveBuffer *buffer;
  
  prev = 0;
  snd_wave.stop_pending = 0;
  source = snd_wave.sources;
  if ((*(HWAVEOUT *)&snd_wave.out_handle) != (HWAVEOUT)0x0) {
    waveOutReset((*(HWAVEOUT *)&snd_wave.out_handle));
    while (got_msg = PeekMessageA(&msg,(*(HWND *)&snd_wave.notify_hwnd),0x3bd,0x3bd,1), got_msg != 0) {
      snd_wave.stop_drained_msg_count++;
    }
    while (source != 0) {
      if (source->kind == 4) {
        prev = source;
        source = source->next;
      }
      else {
        source = snd_wave_free(prev,source);
      }
    }
    buffer = snd_wave.buffer_ring;
    if (snd_wave.buffer_ring != 0) {
      for (i = 0; i < snd_wave.buffer_count; i++) {
        buffer->flags = buffer->flags & 0xffffffdf;
        buffer->flags = buffer->flags & 0xffffffbf;
        buffer = buffer->next;
      }
    }
  }
  return;
}

/* 005d6b4c auto */
void __cdecl snd_fill_silence_u8(iptr dest,uint length)

{
  uptr i;
  
  for (i = 0; i < length; i++) {
    *(i8 *)(i + dest) = 0x80;
  }
  return;
}

/* 005d6b89 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl snd_wave_reset_all(HWAVEOUT wave_out)

{
  WaveBuffer *next;
  WaveBuffer *ring;
  uint i;
  i32 result;
  WaveBuffer *buffer;
  
  ring = snd_wave.buffer_ring;
  buffer = snd_wave.buffer_ring;
  snd_wave.stop_pending = 0;
  if (snd_wave.device_open != 0) {
    waveOutReset(wave_out);
  }
  if (snd_wave.buffer_count != snd_wave.max_buffers) {
    snd_wave_set_buffer_count(snd_wave.max_buffers);
  }
  if (ring != 0) {
    for (i = 0; i < snd_wave.max_buffers; i++) {
      next = buffer->next;
      if (buffer->hdr != 0) {
        waveOutUnprepareHeader(wave_out,(LPWAVEHDR)buffer->hdr,0x20);
        snd_global_free(buffer->data_handle);
      }
      snd_global_free(buffer->handle);
      buffer = next;
    }
  }
  snd_wave.buffer_ring = 0;
  do {
    snd_wave.sources = snd_wave_free(0,snd_wave.sources);
  } while (snd_wave.sources != 0);
  snd_wave.sources = 0;
  return result;
}

/* 005d6c99 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl snd_wave_on_buffer_done(HWAVEOUT wave_out,WaveHeader *wave_hdr)

{
  i16 sound_id;
  WaveHeader *hdr;
  int src;
  MMRESULT write_result;
  WaveSource *first_source;
  WaveSource *source;
  uint i;
  byte *sample;
  WaveBuffer *buffer;
  WaveSource *prev;
  int active_count;
  uint result;
  SoundDoneFn done_callback;
  
  first_source = snd_wave.sources;
  result = result & 0xffff0000;
  done_callback = 0;
  sound_id = 0;
  if (snd_wave.device_open != 0) {
    buffer = wave_hdr->owner;
    i = 0;
    while ((i < snd_wave.buffer_count &&
           (snd_wave.cur_buffer = snd_wave.cur_buffer->next,
           (snd_wave.cur_buffer->flags >> 5 & 1) == 0))) {
      i++;
    }
    if (((u8)buffer->flags & 1) == 0) {
      source = snd_wave.sources;
      if ((((snd_wave.sources != 0) && (snd_wave.sources->kind == 4)) &&
          ((buffer->flags >> 1 & 1) != 0)) && ((buffer->flags >> 4 & 1) != 0)) {
        *(ushort *)snd_wave.sources->status_ptr = *(ushort *)snd_wave.sources->status_ptr & 0xfffb;
        first_source->flags = first_source->flags | 2;
      }
      do {
        hdr = buffer->hdr;
        sample = (byte *)buffer->hdr->data;
        while ((source != (WaveSource *)0x0 && ((source->flags >> 1 & 1) != 0)))
        {
          source = source->next;
        }
        if (source == (WaveSource *)0x0) {
          snd_fill_silence_u8(buffer->hdr->data,buffer->hdr->length);
          buffer->flags = buffer->flags & 0xffffffdf;
        }
        else {
          if ((buffer->flags >> 6 & 1) == 0) {
            if (source->kind == 4) {
              if ((buffer->flags >> 3 & 1) == 0) {
                snd_avi_fill_buffer(buffer,source);
                snd_wave.avi_fill_count++;
              }
              src = (buffer->flags >> 7 & 0xff) * source->buffer_bytes +
                      source->buf_start;
              snd_resample_8bit((byte *)buffer->data,(byte *)src,buffer->length / (int)(uint)(ushort)snd_wave.block_align,
                           CONCAT22((short)((uint)source >> 0x10),
                                    (i16)source->block_align),
                           CONCAT22((short)((uint)src >> 0x10),(ushort)snd_wave.block_align));
              if ((buffer->flags >> 1 & 1) != 0) {
                buffer->flags = buffer->flags | 0x10;
              }
            }
            else {
              snd_wave_copy_source(source,hdr->data,hdr->length);
            }
            for (source = source->next; source != (WaveSource *)0x0;
                source = source->next) {
              if ((source->flags >> 1 & 1) == 0) {
                snd_wave_mix_source(source,hdr->data,hdr->length);
              }
            }
            buffer->flags = buffer->flags | 0x40;
            buffer->hdr->flags = buffer->hdr->flags & 0xfffffffe;
          }
          for (i = 0; i < hdr->length; i++) {
            *sample = *sample ^ 0x80;
            sample++;
          }
          write_result = waveOutWrite(wave_out,(LPWAVEHDR)buffer->hdr,0x20);
          result = (result & 0xffff0000 | (ushort)((short)write_result));
          if ((short)write_result == 0) {
            buffer->flags = buffer->flags & 0xffffffbf;
            buffer->flags = buffer->flags | 0x20;
            buffer->flags = buffer->flags & 0xfffffff7;
          }
          else {
            snd_waveout_error_box(result);
            buffer->flags = buffer->flags & 0xffffffdf;
          }
        }
        source = snd_wave.sources;
        prev = (WaveSource *)0x0;
        active_count = 0;
        while (source != (WaveSource *)0x0) {
          if (((u8)source->flags & 1) == 0) {
            if ((((source->flags >> 1 & 1) == 0) &&
                (active_count = active_count + 1, (source->flags >> 3 & 1) != 0)) &&
               (source->kind != 4)) {
              if ((source->flags >> 2 & 1) == 0) {
                mmioAdvance(source->mmio,(LPMMIOINFO)&source->io_flags,0);
              }
              else {
                snd_wave_stream_refill(source);
              }
            }
            prev = source;
            source = source->next;
          }
          else if (source->kind == 4) {
            prev = source;
            source = source->next;
          }
          else {
            if (source->done_callback != 0) {
              done_callback = source->done_callback;
              source->done_callback = 0;
              sound_id = source->sound_id;
            }
            source = snd_wave_free(prev,source);
          }
        }
        source = snd_wave.sources;
        buffer = buffer->next;
      } while ((active_count != 0) && ((buffer->flags >> 5 & 1) == 0));
      if (done_callback != 0) {
        done_callback(sound_id);
      }
    }
  }
  return 0;
}

/* 005d717f auto */
i32 __cdecl snd_wave_mix_buffer(WaveHeader *hdr)

{
  bool finished;
  i32 result;
  int src;
  WaveBuffer *buffer;
  WaveSource *source;
  WaveSource *prev;
  
  finished = false;
  if (snd_wave.device_open == 0) {
    result = 0;
  }
  else {
    buffer = hdr->owner;
    buffer->flags = buffer->flags & 0xfffffffd;
    source = snd_wave.sources;
    while ((source != (WaveSource *)0x0 && ((source->flags >> 1 & 1) != 0))) {
      source = source->next;
    }
    if (source == (WaveSource *)0x0) {
      buffer->flags = buffer->flags & 0xffffffdf;
    }
    else {
      if ((buffer->flags >> 6 & 1) == 0) {
        if (source->kind == 4) {
          src = (buffer->flags >> 7 & 0xff) * source->buffer_bytes +
                  source->buf_start;
          snd_resample_8bit((byte *)buffer->data,(byte *)src,
                       buffer->length / (int)(uint)(ushort)snd_wave.block_align,
                       CONCAT22((short)((uint)source >> 0x10),
                                (i16)source->block_align),
                       CONCAT22((short)((uint)src >> 0x10),(ushort)snd_wave.block_align));
        }
        else {
          src = snd_wave_copy_source(source,hdr->data,hdr->length);
          if (src == 1) {
            finished = true;
          }
        }
        buffer->flags = buffer->flags | 0x40;
        source = source->next;
      }
      for (; source != (WaveSource *)0x0; source = source->next) {
        if (((source->flags >> 1 & 1) == 0) &&
           (src = snd_wave_mix_source(source,hdr->data,hdr->length), src == 0)) {
          finished = false;
        }
      }
      hdr->flags = hdr->flags & 0xfffffffe;
    }
    source = snd_wave.sources;
    prev = (WaveSource *)0x0;
    while (source != (WaveSource *)0x0) {
      if (((u8)source->flags & 1) == 0) {
        if (((source->flags >> 1 & 1) == 0) && ((source->flags >> 3 & 1) != 0)) {
          if (source->kind == 4) {
            snd_avi_stream_refill(source);
          }
          if (source->kind != 4) {
            if ((source->flags >> 2 & 1) == 0) {
              mmioAdvance(source->mmio,(LPMMIOINFO)&source->io_flags,0);
            }
            else {
              snd_wave_stream_refill(source);
            }
          }
        }
        prev = source;
        source = source->next;
      }
      else if (source->kind == 4) {
        buffer->flags = buffer->flags | 2;
        source->flags = source->flags | 2;
        prev = source;
        source = source->next;
      }
      else {
        source = snd_wave_free(prev,source);
      }
    }
    if ((snd_wave.sources == 0) || (finished)) {
      result = 1;
    }
    else {
      result = 0;
    }
  }
  return result;
}

/* 005d7494 auto */
WaveSource * __cdecl snd_wave_free(WaveSource *prev,WaveSource *source)

{
  WaveSource *next;
  WaveSource *new_head;
  
  if (source == 0) {
    next = 0;
  }
  else {
    next = source->next;
    new_head = next;
    if (prev != 0) {
      prev->next = next;
      new_head = snd_wave.sources;
    }
    snd_wave.sources = new_head;
    mmioClose(source->mmio,0);
    if ((source->flags >> 2 & 1) != 0) {
      mmioClose(source->file_mmio,0);
    }
    GlobalUnlock(source->mem_handle);
    GlobalFree(source->mem_handle);
    snd_wave.source_count--;
  }
  return next;
}

/* 005d753e auto */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

i32 __cdecl snd_wave_copy_source(WaveSource *source,int dst,uint length)

{
  ushort consumed;
  uint want;
  uint avail;
  i32 result;
  uint count;
  uint remaining;
  
  want = length / (ushort)snd_wave.block_align;
  if ((source == 0) || (((u8)source->flags & 1) != 0)) {
    count = 0;
  }
  else {
    if ((source->flags >> 2 & 1) == 0) {
      remaining = source->data_size - source->play_pos;
    }
    else {
      remaining = source->buf_end - source->buf_next;
    }
    avail = remaining / source->block_align;
    count = want;
    if (avail < want) {
      source->flags = source->flags | 1;
      count = avail;
    }
    if ((uint)(source->buf_end - source->buf_next) /
        (uint)source->block_align < want * 2) {
      source->flags = source->flags | 8;
    }
    else {
      source->flags = source->flags & 0xfffffff7;
    }
    want -= count;
  }
  if (count != 0) {
    consumed = snd_resample_8bit((byte *)dst,(byte *)source->buf_next,count,
                         CONCAT22((short)((uint)source >> 0x10),(i16)source->block_align),
                         CONCAT22((short)(want >> 0x10),(ushort)snd_wave.block_align));
    source->buf_next = source->buf_next + (uint)consumed;
    if ((source->kind == 4) && (source->buf_end == source->buf_next))
    {
      source->buf_next = source->buf_start;
    }
    source->flags = source->flags & 0xffffffef;
  }
  if (want == 0) {
    if ((source->flags >> 2 & 1) == 0) {
      source->play_pos = source->play_pos + source->block_align * count;
    }
    else {
      source->play_pos = 0;
    }
    result = 0;
  }
  else {
    _memset((void *)(dst + (ushort)snd_wave.block_align * count),0,(ushort)snd_wave.block_align * want);
    if ((source != 0) && (remaining != 0)) {
      source->play_pos = source->play_pos + source->block_align * count;
    }
    result = 1;
  }
  return result;
}

/* 005d778c auto */
i32 __cdecl snd_wave_mix_source(WaveSource *source,i32 dst,uint length)

{
  ushort consumed;
  uint avail;
  uint want;
  uptr flags;
  i32 result;
  i32 advance;
  i32 count;
  uint remaining;
  
  if ((source->flags >> 2 & 1) == 0) {
    remaining = source->data_size - source->play_pos;
  }
  else {
    remaining = source->buf_end - source->buf_next;
  }
  avail = remaining / source->block_align;
  want = length / (ushort)snd_wave.block_align;
  count = want;
  if (avail < want) {
    source->flags = source->flags | 1;
    count = avail;
  }
  if ((uint)(source->buf_end - source->buf_next) /
      (uint)source->block_align < want * 2) {
    flags = source->flags | 8;
    source->flags = flags;
  }
  else {
    source->flags = source->flags & 0xfffffff7;
    flags = (uptr)source;
  }
  if (count != 0) {
    consumed = snd_mix_8bit_resample((char *)dst,(byte *)source->buf_next,count,
                         CONCAT22((short)((uptr)source >> 0x10),(i16)source->block_align),
                         CONCAT22((short)(flags >> 0x10),(ushort)snd_wave.block_align));
    advance = (uint)consumed;
    source->buf_next = source->buf_next + advance;
  }
  if ((source->flags >> 2 & 1) == 0) {
    source->play_pos = source->play_pos + source->block_align * count;
  }
  else {
    source->play_pos = 0;
  }
  if ((((u8)source->flags & 1) == 0) || ((source->flags >> 2 & 1) != 0)) {
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 005d791b auto */
i32 __cdecl snd_avi_fill_buffer(WaveBuffer *buffer,WaveSource *source)

{
  i32 result;
  uint limit;
  WaveSource *head;
  uint byte_count;
  uint i;
  int bytes_read;
  
  head = snd_wave.sources;
  if (source->fill_buffer == buffer) {
    byte_count = snd_wave.sources->buffer_bytes - snd_wave.sources->buffer_fill;
    bytes_read = 0;
    limit = snd_wave.sources->data_size -
            (snd_wave.sources->stream_pos - snd_wave.sources->stream_start);
    if (limit < byte_count) {
      byte_count = limit;
    }
    AVIStreamRead(snd_wave.sources->avi_stream,
                  (uint)snd_wave.sources->stream_pos / (uint)snd_wave.sources->block_align,
                  byte_count / snd_wave.sources->block_align,(LPVOID)snd_wave.sources->buf_next
                  ,byte_count,&bytes_read,0);
    snd_wave.sources->stream_pos = snd_wave.sources->stream_pos + bytes_read;
    head->buf_next = head->buf_next + bytes_read;
    snd_wave.sources->buffer_fill = snd_wave.sources->buffer_fill + bytes_read;
    if (head->buf_end == head->buf_next) {
      head->buf_next = head->buf_start;
    }
    if ((uint)snd_wave.sources->buffer_bytes <= (uint)snd_wave.sources->buffer_fill) {
      buffer->flags = buffer->flags | 8;
      snd_wave.sources->fill_buffer = buffer->next;
      buffer = snd_wave.sources->fill_buffer;
      snd_wave.sources->buffer_fill = 0;
    }
    if ((uint)snd_wave.sources->data_size <=
        (uint)(snd_wave.sources->stream_pos - snd_wave.sources->stream_start)) {
      i = 0;
      while ((i < (uint)(snd_wave.sources->buffer_bytes - snd_wave.sources->buffer_fill) &&
             ((uint)head->buf_next < (uint)head->buf_end))) {
        *(i8 *)head->buf_next = 0x80;
        head->buf_next = head->buf_next + 1;
        i++;
      }
      if (head->buf_end == head->buf_next) {
        head->buf_next = head->buf_start;
      }
      buffer->flags = buffer->flags & 0xfffffff7;
      buffer->flags = buffer->flags | 2;
      snd_wave.sources->flags = snd_wave.sources->flags | 1;
      snd_wave.sources->fill_buffer = buffer->next;
      snd_wave.sources->buffer_fill = 0;
    }
    result = 0;
  }
  else {
    result = 0xd;
  }
  return result;
}

/* 005dd010 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

AviPlayer *AviPlayer::construct()

{
  i32 eh_next;
  
  this->SurfaceWindow::construct();
  audio.construct();
  palette.init();
  surface_ctor(&backdrop);
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_6);
  on_end = 0;
  double_size_u = 0;
  transparent_u = 0;
  target_surface_uptr = 0;
  has_audio_u = 1;
  audio_playing_u = 0;
  target_surface_uptr = 0;
  start_frame_u = 0;
  end_frame_u = 0;
  cur_frame_u = 0;
  field_a1c = 0;
  cue_frame_u = 0;
  paused_u = 1;
  loop_u = 0;
  media_devices.avi_player = (i32)this;
  return this;
}

/* 005dd1a0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 AviPlayer::destruct()

{
  i32 eh_next;
  int eh_state;
  
  *(i8 ***)&vtable = (i8 **)((pointer *)&leader_anim_frame_starts_block.ptr_fun_6);
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 3;
  avi_play_pause(this);
  avi_close(this);
  media_devices.avi_player = 0;
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 005dd230 */ { surface_dtor((DibSurface *)(((int)this) + 0x558)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 005dd23f */ { ((GamePalette *)(((int)this) + 0x124))->destroy(); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 005dd24e */ { ((AviAudio *)(((int)this) + 0x11c))->release(); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 005dd25d */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return eh_next;
}

/* 005dd27e auto */
void AviPlayer::create(iptr parent,i32 style,u32 x,u32 y)

{
  wnd_create(parent,style,x,y,0x140,0xf0,(iptr)&palette_u
            );
  fill(0);
  pos_x_u = x;
  pos_y_u = y;
  return;
}

/* 005dd2e3 auto */
void AviPlayer::create_child(iptr parent,i32 style,u32 x,u32 y,iptr target)

{
  iptr target_dib;
  
  if (target == 0) {
    target_dib = 0;
  }
  else {
    target_dib = target + 0x48;
  }
  wnd_create_with_surface((const char *)parent,style,x,y,0x140,0xf0,(iptr)&palette_u
             ,target_dib);
  fill(0);
  target_surface_uptr = target;
  pos_x_u = x;
  pos_y_u = y;
  return;
}

/* 005dd377 auto */
int AviPlayer::open(const char *filename)

{
  int result;
  
  avi_close(this);
  result = avi_open_file(this,filename);
  if (result == 0) {
    result = 0;
  }
  return result;
}

/* 005dd3c2 auto */
void AviPlayer::play()

{
  loop_bytes[0] = '\0';
  loop_bytes[1] = '\0';
  loop_bytes[2] = '\0';
  loop_bytes[3] = '\0';
  avi_play_start(this);
  return;
}

/* 005dd3f1 auto */
void AviPlayer::play_range(u32 first_frame,u32 last_frame)

{
  if ((int)first_frame <= (int)last_frame) {
    loop_u = 0;
    avi_seek_frame(this,first_frame,0);
    end_frame_u = last_frame;
    start_frame_u = first_frame;
    avi_play_start(this);
  }
  return;
}

/* 005dd45d auto */
void AviPlayer::show_frame(i32 frame)

{
  avi_seek_frame(this,frame,1);
  return;
}

/* 005dd4c2 auto */
void AviPlayer::loop_range(u32 first_frame,u32 last_frame)

{
  start_frame_u = first_frame;
  avi_seek_frame(this,first_frame,0);
  end_frame_u = last_frame;
  loop_u = 1;
  avi_play_start(this);
  return;
}

/* 005dd51d auto */
void AviPlayer::pause()

{
  avi_play_pause(this);
  return;
}

/* 005dd561 auto */
void DibSurface::avi_player_set_palette(iptr palette)

{
  avi_apply_palette((AviPlayer *)this,(GamePalette *)palette);
  set_palette(palette);
  ((WndBase *)(this + 1))->set_palette(palette);
  return;
}

/* 005dd64c auto */
void DibSurface::avi_player_draw_to(iptr dest,int x,int y)

{
  int dest_size;
  int src_size;
  tagRECT src_rect;
  tagRECT dst_rect;
  
  if ((-1 < x) && (-1 < y)) {
    dest_size = ((DibSurface *)dest)->surface_width();
    src_size = get_width();
    if (src_size <= dest_size - x) {
      dest_size = ((DibSurface *)dest)->surface_height();
      src_size = get_height();
      if (src_size <= dest_size - y) {
        avi_decode_frame((AviPlayer *)this,0);
        dest_size = get_height();
        src_size = get_width();
        SetRect(&src_rect,0,0,src_size,dest_size);
        CopyRect(&dst_rect,&src_rect);
        OffsetRect(&dst_rect,x,y);
        blit((DibSurface *)dest,&src_rect,&dst_rect);
      }
    }
  }
  return;
}

/* 005dd71e auto */
void AviPlayer::set_double_size(i32 enable)

{
  int height;
  int width;
  
  height = get_height();
  height = height * 2;
  width = get_width();
  resize_clamped(width * 2,height);
  double_size_u = 1;
  return;
}

/* 005dd761 auto */
void AviPlayer::set_transparent(int enable)

{
  if (enable == 0) {
    if (transparent_u != 0) {
      transparent_u = 0;
      avi_restart_decompress(this,0);
    }
  }
  else if (transparent_u == 0) {
    transparent_u = 1;
    avi_restart_decompress(this,1);
  }
  return;
}

/* 005dd8a0 auto */
void __cdecl snd_system_init(void)

{
  WNDCLASSA wnd_class;
  
  wnd_class.style = 0;
  wnd_class.lpfnWndProc = (WNDPROC)snd_mci_notify_wndproc;
  wnd_class.cbClsExtra = 0;
  wnd_class.cbWndExtra = 0;
  wnd_class.hInstance = err_report_ctx_block.app_hinstance;
  wnd_class.hIcon = (HICON)0x0;
  wnd_class.hCursor = (HCURSOR)0x0;
  wnd_class.hbrBackground = (HBRUSH)0x0;
  wnd_class.lpszMenuName = (LPCSTR)0x0;
  wnd_class.lpszClassName = str_msmmwindow;
  RegisterClassA(&wnd_class);
  err_report_ctx_block.snd_mci_notify_hwnd = CreateWindowExA(0,str_msmmwindow_2,(LPCSTR)0x0,0,-0x80000000,-0x80000000,
                                 -0x80000000,-0x80000000,(HWND)0x0,(HMENU)0x0,err_report_ctx_block.app_hinstance,
                                 (LPVOID)0x0);
  snd_cd_open();
  return;
}

/* 005dd93f auto */
LRESULT __stdcall snd_mci_notify_wndproc(HWND hwnd,UINT msg,WPARAM wparam,uint lparam)

{
  LRESULT ret;
  
  if (msg == 0x3b9) {
    if (wparam == 1) {
      if ((lparam & 0xffff) == media_devices.midi_device_id) {
        snd_midi_fire_notify_callback();
      }
      else if ((lparam & 0xffff) == media_devices.cd_device_id) {
        snd_cd_fire_notify_callback();
      }
    }
    ret = 0;
  }
  else {
    ret = DefWindowProcA(hwnd,msg,wparam,lparam);
  }
  return ret;
}

/* 005dda11 auto */
void __cdecl snd_stop_sndplaysound(void)

{
  sndPlaySoundA((LPCSTR)0x0,1);
  return;
}

/* 005ddbc7 auto */
i32 __cdecl snd_cd_play_track(uint track)

{
  MCIERROR mci_result;
  i32 result;
  MCI_PLAY_PARMS play_parms;
  DWORD_PTR play_flags;
  MCI_SEEK_PARMS seek_parms;
  MCIDEVICEID device_id;
  
  if (media_devices.cd_device_id == 0) {
    snd_cd_open();
    if (media_devices.cd_device_id == 0) {
      return 0;
    }
    device_id = media_devices.cd_device_id;
  }
  else {
    device_id = media_devices.cd_device_id;
    mciSendCommandA(media_devices.cd_device_id,0x808,0,0);
  }
  seek_parms.dwCallback = 0;
  seek_parms.dwTo = track & 0xff;
  mci_result = mciSendCommandA(device_id,0x807,8,(DWORD_PTR)&seek_parms);
  if (mci_result == 0) {
    if ((track & 0xff) == (*(uint *)&media_devices.cd_track_count)) {
      play_flags = 5;
    }
    else {
      play_flags = 0xd;
    }
    play_parms.dwFrom = track & 0xff;
    play_parms.dwTo = (uint)(byte)((char)track + 1);
    play_parms.dwCallback = (*(i32 *)&err_report_ctx_block.snd_mci_notify_hwnd);
    mci_result = mciSendCommandA(device_id,0x806,play_flags,(DWORD_PTR)&play_parms);
    if (mci_result == 0) {
      result = 1;
    }
    else {
      snd_cd_close();
      dbg_logf((const char *)&str_failed_to_play_requested_cd_track_2,mci_result);
      result = 0;
    }
  }
  else {
    snd_cd_close();
    dbg_logf((const char *)&str_failed_to_play_requested_cd_track,mci_result);
    result = 0;
  }
  return result;
}

/* 005ddd12 auto */
void __cdecl snd_midi_close(void)

{
  if (media_devices.midi_device_id != 0) {
    mciSendCommandA(media_devices.midi_device_id,0x808,0,0);
    media_devices.midi_device_id = 0;
  }
  return;
}

/* 005ddd4e auto */
void __cdecl snd_cd_open(void)

{
  MCIERROR mci_result;
  MCI_OPEN_PARMSA open_parms;
  MCI_SET_PARMS set_parms;
  MCI_STATUS_PARMS status_parms;
  
  if (media_devices.cd_device_id == 0) {
    open_parms.lpstrDeviceType = str_cdaudio;
    mci_result = mciSendCommandA(0,0x803,0x2000,(DWORD_PTR)&open_parms);
    if (mci_result == 0) {
      media_devices.cd_device_id = open_parms.wDeviceID;
      set_parms.dwTimeFormat = 10;
      mci_result = mciSendCommandA(open_parms.wDeviceID,0x80d,0x400,(DWORD_PTR)&set_parms);
      if (mci_result == 0) {
        status_parms.dwCallback = 0;
        status_parms.dwReturn = 0;
        status_parms.dwTrack = 0;
        status_parms.dwItem = 3;
        mciSendCommandA(media_devices.cd_device_id,0x814,0x100,(DWORD_PTR)&status_parms);
        media_devices.cd_track_count = status_parms.dwReturn;
      }
      else {
        mciSendCommandA(media_devices.cd_device_id,0x804,0,0);
        media_devices.cd_device_id = 0;
        dbg_logf((const char *)&str_cdaudio_could_not_set_time_format,mci_result);
      }
    }
    else {
      dbg_logf((const char *)&str_failed_to_open_cdaudio_device,mci_result);
    }
  }
  return;
}

/* 005dde57 auto */
void __cdecl snd_cd_close(void)

{
  if (media_devices.cd_device_id != 0) {
    mciSendCommandA(media_devices.cd_device_id,0x804,0,0);
    media_devices.cd_device_id = 0;
    media_devices.cd_track_count = 0;
  }
  return;
}

/* 005dde9d auto */
void __cdecl snd_cd_mci_stop(void)

{
  if (media_devices.cd_device_id != 0) {
    mciSendCommandA(media_devices.cd_device_id,0x808,0,0);
  }
  return;
}

/* 005ddecf auto */
void __cdecl snd_system_shutdown(void)

{
  snd_stop_sndplaysound();
  snd_cd_mci_stop();
  snd_cd_close();
  snd_midi_close();
  DestroyWindow(err_report_ctx_block.snd_mci_notify_hwnd);
  return;
}

/* 005ddeff auto */
i32 __cdecl snd_cd_get_track_count(void)

{
  MCIERROR mci_result;
  MCI_OPEN_PARMSA open_parms;
  MCIDEVICEID device_id;
  MCI_SET_PARMS set_parms;
  MCI_STATUS_PARMS status_parms;
  i32 track_count;
  
  if (media_devices.cd_device_id == 0) {
    open_parms.lpstrDeviceType = str_cdaudio_2;
    mci_result = mciSendCommandA(0,0x803,0x2000,(DWORD_PTR)&open_parms);
    if (mci_result != 0) {
      return 0xffffffff;
    }
    device_id = open_parms.wDeviceID;
    media_devices.cd_device_id = open_parms.wDeviceID;
    set_parms.dwTimeFormat = 10;
    mci_result = mciSendCommandA(open_parms.wDeviceID,0x80d,0x400,(DWORD_PTR)&set_parms);
    if (mci_result != 0) {
      mciSendCommandA(device_id,0x804,0,0);
      media_devices.cd_device_id = 0;
      return 0xffffffff;
    }
  }
  else {
    device_id = media_devices.cd_device_id;
  }
  status_parms.dwItem = 3;
  mci_result = mciSendCommandA(device_id,0x814,0x100,(DWORD_PTR)&status_parms);
  if (mci_result == 0) {
    track_count = status_parms.dwReturn;
    media_devices.cd_track_count = track_count;
  }
  else {
    snd_cd_close();
    track_count = 0xffffffff;
  }
  return track_count;
}

/* 005de6bd auto */
void __cdecl snd_midi_fire_notify_callback(void)

{
  if (err_report_ctx_block.snd_midi_notify_callback != 0) {
    err_report_ctx_block.snd_midi_notify_callback();
  }
  return;
}

/* 005de6fc auto */
void __cdecl snd_cd_set_notify_callback(MusicNotifyFn callback)

{
  err_report_ctx_block.snd_cd_notify_callback = callback;
  return;
}

/* 005de714 auto */
void __cdecl snd_cd_fire_notify_callback(void)

{
  if (err_report_ctx_block.snd_cd_notify_callback != 0) {
    err_report_ctx_block.snd_cd_notify_callback();
  }
  return;
}

/* 005e1c70 auto */
void __cdecl avi_update_current_frame(void)

{
  avi_update_frame(avi_play.active_window);
  return;
}

/* 005e1c8e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl avi_open_file(AviPlayer *player,const char *filename)

{
  int attach_result;
  i32 start_frame;
  int value;
  GdiDib *dib;
  HGLOBAL mem_handle;
  i8 palette_rgb[708];
  AVISTREAMINFOA stream_info;
  iptr i;
  i32 format_size;
  int result;
  
  if (indeo.vfw_initialized == 0) {
    AVIFileInit();
    indeo.vfw_initialized = 1;
  }
  avi_play.active_window = player;
  indeo.seq_opts[0] = 0x2c;
  indeo.seq_fourcc = 0x31345649;
  indeo.seq_environment = 2;
  indeo.seq_version = 0x10002;
  indeo.seq_type = 4;
  indeo.video_flags_a = 0;
  indeo.seq_extra = 0;
  indeo.frame_opts[0] = 0x70;
  indeo.frame_fourcc = 0x31345649;
  indeo.frame_environment = 2;
  indeo.frame_version = 0x10002;
  indeo.frame_type = 2;
  indeo.view_rect_ptr = (i32)((i32 *)&indeo.view_rect_x);
  indeo.video_flags_b = 0x80000008;
  indeo.view_rect_x[0] = 0;
  indeo.view_rect_y = 0;
  indeo.view_rect_width = 0;
  indeo.view_rect_height = 0;
  result = AVIFileOpenA(((PAVIFILE *)&avi_play.file),filename,0,0);
  if (result == 0) {
    result = AVIFileGetStream(avi_play.file[0],((PAVISTREAM *)&avi_play.video_stream),0x73646976,0);
    if (result == 0) {
      value = 0;
      if (player->has_audio != 0) {
        result = AVIFileGetStream(avi_play.file[0],((PAVISTREAM *)&avi_play.audio_stream),0x73647561,0);
        if (result == 0) {
          player->audio_playing = 0;
          attach_result = player->audio.attach_stream(avi_play.audio_stream[0]);
          value = result;
          if (attach_result == 0) {
            player->has_audio = 1;
          }
          else {
            player->has_audio = 0;
          }
        }
        else {
          player->has_audio = 0;
          value = result;
        }
      }
      result = value;
      format_size = 0x28;
      AVIStreamReadFormat(avi_play.video_stream[0],1,(LPVOID)&player->src_format,&format_size);
      AVIStreamInfoA(avi_play.video_stream[0],&stream_info,0x8c);
      player->src_format.size_image = stream_info.dwSuggestedBufferSize;
      indeo.frames_per_second = stream_info.dwRate / stream_info.dwScale;
      avi_play.decompressor = ICLocate(0x63646976,stream_info.fccHandler,(LPBITMAPINFOHEADER)&player->src_format,0,2);
      if (avi_play.decompressor == 0) {
        avi_close(player);
        result = 0;
      }
      else {
        start_frame = AVIStreamStart(avi_play.video_stream[0]);
        player->start_frame = start_frame;
        value = AVIStreamLength(avi_play.video_stream[0]);
        player->end_frame = player->start_frame + value - 1;
        player->cur_frame = player->start_frame;
        crt_memcpy((void *)&player->dst_format,(const void *)&player->src_format,0x28);
        player->dst_format.compression = 0;
        player->dst_format.bit_count = 8;
        player->dst_format.size_image = 0;
        player->dst_format.clr_used = 0x100;
        avi_play.frame_width = player->src_format.width;
        avi_play.frame_height = player->src_format.height;
        dib = player->get_dib();
        value = dib_is_top_down(dib);
        if (value != 0) {
          player->dst_format.height = -player->dst_format.height;
        }
        if (player->double_size == 0) {
          player->resize_clamped(avi_play.frame_width,avi_play.frame_height);
        }
        else {
          player->resize_clamped(avi_play.frame_width * 2,avi_play.frame_height * 2);
        }
        if (stream_info.dwSuggestedBufferSize == 0) {
          value = (uint)player->src_format.bit_count * player->src_format.width *
                  player->src_format.height;
          mem_handle = GlobalAlloc(0x40,(int)(value + (value >> 0x1f & 7U)) >> 3);
          avi_play.frame_buffer = GlobalLock(mem_handle);
        }
        else {
          mem_handle = GlobalAlloc(0x40,stream_info.dwSuggestedBufferSize);
          avi_play.frame_buffer = GlobalLock(mem_handle);
        }
        if (avi_play.frame_buffer == (LPVOID)0x0) {
          avi_close(player);
          result = 0;
        }
        else {
          result = avi_ic_decompress_ex_query(avi_play.decompressor,0,(iptr)&player->src_format,0,0,0,
                                 player->src_format.width,player->src_format.height,
                                 (iptr)&player->dst_format,0,0,0,player->src_format.width,
                                 player->src_format.height);
          if (result == 0) {
            ICSendMessage(avi_play.decompressor,0x401e,(iptr)&player->src_format,(iptr)&player->dst_format);
            for (i = 0; i < 0xec; i++) {
              palette_rgb[i * 3] = (i8)player->dst_colors[0x2a + i * 4];
              palette_rgb[i * 3 + 1] = (i8)player->dst_colors[0x29 + i * 4];
              palette_rgb[i * 3 + 2] = (i8)player->dst_colors[0x28 + i * 4];
            }
            player->palette.set_rgb(10,0xec,(iptr)palette_rgb);
            ICSendMessage(avi_play.decompressor,0x401d,(iptr)&player->dst_format,0);
            result = avi_ic_decompress_ex_begin(avi_play.decompressor,0,(iptr)&player->src_format,0,0,0,
                                   player->src_format.width,player->src_format.height
                                   ,(iptr)&player->dst_format,0,0,0,player->src_format.width,
                                   player->src_format.height);
            if (result == 0) {
              avi_play.file_loaded = 1;
              avi_play.playing = 0;
              if (player->transparent == 0) {
                avi_restart_decompress(player,0);
              }
              else {
                avi_restart_decompress(player,1);
              }
              value = (uint)player->src_format.bit_count * player->src_format.width *
                      player->src_format.height;
              avi_play.frame_buffer_size = (int)(value + (value >> 0x1f & 7U)) >> 3;
              avi_blit_union_left_block.g_video_frame_count = 0;
              err_report_ctx_block.avi_indeo_frame_extra_i32 = 0;
              result = 0;
            }
            else {
              avi_close(player);
            }
          }
          else {
            avi_close(player);
          }
        }
      }
    }
    else {
      avi_close(player);
    }
  }
  else {
    avi_close(player);
  }
  return result;
}

/* 005e22ed: written by hand in src/game/fixups.cpp */

/* 005e250c auto */
i32
__stdcall avi_ic_decompress_ex_begin(i32 hic,i32 flags,iptr bi_src,i32 src_bits,
            i32 x_src,i32 y_src,i32 width_src,i32 height_src,iptr bi_dst
            ,i32 dst_bits,i32 x_dst,i32 y_dst,i32 width_dst,
            i32 height_dst)

{
  i32 result;
  ICDECOMPRESSEX ex;
  
  ex.dwFlags = flags;
  ex.lpbiSrc = (BITMAPINFOHEADER *)bi_src;
  ex.lpSrc = (void *)src_bits;
  ex.xSrc = x_src;
  ex.ySrc = y_src;
  ex.dxSrc = width_src;
  ex.dySrc = height_src;
  ex.lpbiDst = (BITMAPINFOHEADER *)bi_dst;
  ex.lpDst = (void *)dst_bits;
  ex.xDst = x_dst;
  ex.yDst = y_dst;
  ex.dxDst = width_dst;
  ex.dyDst = height_dst;
  result = ICSendMessage(hic,0x403c,(DWORD_PTR)&ex,0x34);
  return result;
}

/* 005e2583 auto */
i32
__stdcall avi_ic_decompress_ex_query(i32 hic,i32 flags,iptr bi_src,i32 src_bits,
            i32 x_src,i32 y_src,i32 width_src,i32 height_src,iptr bi_dst
            ,i32 dst_bits,i32 x_dst,i32 y_dst,i32 width_dst,
            i32 height_dst)

{
  i32 result;
  ICDECOMPRESSEX ex;
  
  ex.dwFlags = flags;
  ex.lpbiSrc = (BITMAPINFOHEADER *)bi_src;
  ex.lpSrc = (void *)src_bits;
  ex.xSrc = x_src;
  ex.ySrc = y_src;
  ex.dxSrc = width_src;
  ex.dySrc = height_src;
  ex.lpbiDst = (BITMAPINFOHEADER *)bi_dst;
  ex.lpDst = (void *)dst_bits;
  ex.xDst = x_dst;
  ex.yDst = y_dst;
  ex.dxDst = width_dst;
  ex.dyDst = height_dst;
  result = ICSendMessage(hic,0x403d,(DWORD_PTR)&ex,0x34);
  return result;
}

/* 005e2675 auto */
void __cdecl avi_play_pause(AviPlayer *player)

{
  if ((player != 0) && (avi_play.file_loaded != 0)) {
    if (player->has_audio != 0) {
      player->audio_playing = 0;
    }
    player->paused = 1;
    if (avi_play.playing != 0) {
      wnd_modal_stack_below_block.avi_idle_update_enabled = 0;
      avi_blit_union_left_block.avi_frame_needs_blit = 0;
      avi_play.playing = 0;
    }
  }
  return;
}

/* 005e2799 auto */
void __cdecl avi_close(AviPlayer *player)

{
  HGLOBAL mem_handle;
  
  if (player != 0) {
    avi_play_pause(player);
    if (avi_play.decompressor != 0) {
      ICClose(avi_play.decompressor);
      avi_play.decompressor = 0;
    }
    if (avi_play.audio_stream[0] != 0) {
      AVIStreamRelease(avi_play.audio_stream[0]);
      avi_play.audio_stream[0] = 0;
    }
    if (avi_play.video_stream[0] != 0) {
      AVIStreamRelease(avi_play.video_stream[0]);
      avi_play.video_stream[0] = 0;
    }
    if (avi_play.file[0] != 0) {
      AVIFileRelease(avi_play.file[0]);
      avi_play.file[0] = 0;
    }
    if (indeo.vfw_initialized != 0) {
      AVIFileExit();
      indeo.vfw_initialized = 0;
    }
    if ((*(LPCVOID *)&avi_play.frame_buffer) != (LPCVOID)0x0) {
      mem_handle = GlobalHandle((*(LPCVOID *)&avi_play.frame_buffer));
      GlobalUnlock(mem_handle);
      mem_handle = GlobalHandle((*(LPCVOID *)&avi_play.frame_buffer));
      GlobalFree(mem_handle);
      (*(LPCVOID *)&avi_play.frame_buffer) = (LPCVOID)0x0;
    }
    avi_play.frame_buffer_size = 0;
    avi_play.frame_width = 0;
    avi_play.frame_height = 0;
    avi_play.active_window = 0;
  }
  return;
}

/* 005e28cd auto */
void __cdecl avi_seek_frame(AviPlayer *player,uint frame_index,int redraw)

{
  i32 key_frame;
  uint target_frame;
  
  if ((player != 0) && (avi_play.file_loaded != 0)) {
    if (avi_play.playing != 0) {
      avi_play_pause(player);
    }
    key_frame = AVIStreamFindSample(avi_play.video_stream[0],frame_index,0x14);
    player->cur_frame = key_frame;
    while( true ) {
      if ((int)frame_index < 0) {
        target_frame = 0;
      }
      else {
        target_frame = frame_index;
      }
      if (target_frame <= (uint)player->cur_frame) break;
      avi_decode_frame(player,0x80000000);
    }
    avi_decode_frame(player,0);
    if (redraw != 0) {
      avi_update_frame(player);
    }
  }
  return;
}

/* 005e2997 auto */
int __cdecl avi_decode_frame(AviPlayer *player,int decode_flags)

{
  i32 dst_bits;
  i32 x_dst;
  i32 y_dst;
  int dst_dim_a;
  int dst_dim_b;
  CHAR message[80];
  i32 bytes_read;
  int result;
  
  result = 0;
  bytes_read = 0;
  if ((player == 0) || (avi_play.file_loaded == 0)) {
    result = 1;
  }
  else {
    if ((player->transparent != 0) && (decode_flags == 0)) {
      player->backdrop.DibSurface::blit(player,(RECT *)&avi_blit_union_left_block.avi_blit_union_left,(RECT *)&avi_blit_union_left_block.avi_blit_union_left);
    }
    result = AVIStreamRead(avi_play.video_stream[0],player->cur_frame,1,avi_play.frame_buffer,
                            avi_play.frame_buffer_size,&bytes_read,0);
    if (result == 0) {
      player->src_format.size_image = bytes_read;
      if (player->double_size == 0) {
        y_dst = 0;
        x_dst = 0;
        dst_dim_b = avi_play.frame_width;
        dst_dim_a = avi_play.frame_height;
        dst_bits = player->get_bits();
        result = avi_ic_decompress_ex(avi_play.decompressor,decode_flags,(iptr)&player->src_format,(*(i32 *)&avi_play.frame_buffer),0,0,avi_play.frame_width,
                               avi_play.frame_height,(iptr)&player->dst_format,dst_bits,x_dst,y_dst,dst_dim_b,dst_dim_a);
      }
      else {
        player->dst_format.height = player->dst_format.height << 1;
        player->dst_format.width = player->dst_format.width << 1;
        dst_dim_b = avi_play.frame_height * 2;
        dst_dim_a = avi_play.frame_width * 2;
        y_dst = 0;
        x_dst = 0;
        dst_bits = player->get_bits();
        result = avi_ic_decompress_ex(avi_play.decompressor,decode_flags,(iptr)&player->src_format,(*(i32 *)&avi_play.frame_buffer),0,0,avi_play.frame_width,
                               avi_play.frame_height,(iptr)&player->dst_format,dst_bits,x_dst,y_dst,dst_dim_a,dst_dim_b);
        player->dst_format.height = player->dst_format.height >> 1;
        player->dst_format.width = player->dst_format.width >> 1;
      }
      if ((result != 0) && (result != 1)) {
        wsprintfA(message,str_icdecompressex_returned_ld,result);
        MessageBoxA((HWND)0x0,message,str_get_frame,0);
      }
      if ((player->cue_frame != 0) &&
         (player->cue_frame == player->cur_frame)) {
        player->cue_frame = 0;
        player->call_cue_callback();
      }
      if (((uint)player->end_frame < (uint)player->cur_frame) && (avi_play.playing != 0)) {
        if (player->loop == 0) {
          avi_play_pause(player);
          player->call_end_callback();
          result = 0;
        }
        else {
          avi_seek_frame(player,player->start_frame,0);
          avi_play_start(player);
          result = 0;
        }
      }
      else {
        player->cur_frame = player->cur_frame + 1;
        if (decode_flags != 0) {
          result = 1;
        }
      }
    }
  }
  return result;
}

/* 005e2c5a auto */
i32
__stdcall avi_ic_decompress_ex(i32 hic,i32 flags,iptr bi_src,i32 src_bits,
            i32 x_src,i32 y_src,i32 width_src,i32 height_src,iptr bi_dst
            ,i32 dst_bits,i32 x_dst,i32 y_dst,i32 width_dst,
            i32 height_dst)

{
  i32 result;
  ICDECOMPRESSEX ex;
  
  ex.dwFlags = flags;
  ex.lpbiSrc = (BITMAPINFOHEADER *)bi_src;
  ex.lpSrc = (void *)src_bits;
  ex.xSrc = x_src;
  ex.ySrc = y_src;
  ex.dxSrc = width_src;
  ex.dySrc = height_src;
  ex.lpbiDst = (BITMAPINFOHEADER *)bi_dst;
  ex.lpDst = (void *)dst_bits;
  ex.xDst = x_dst;
  ex.yDst = y_dst;
  ex.dxDst = width_dst;
  ex.dyDst = height_dst;
  result = ICSendMessage(hic,0x403e,(DWORD_PTR)&ex,0x34);
  return result;
}

/* 005e2cd1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl avi_update_frame(AviPlayer *player)

{
  int sample;
  i32 audio_pos_ms;
  uint target_frame;
  RECT dirty_rect;
  int key_frame;
  DWORD now;
  
  if (player == 0) {
    return;
  }
  if (avi_play.file_loaded == 0) {
    return;
  }
  if (avi_play.playing != 0) {
    if (player->has_audio == 0) {
      now = timeGetTime();
      sample = AVIStreamTimeToSample(avi_play.video_stream[0],now - avi_play.start_time);
      target_frame = player->start_frame + sample;
      if (target_frame < (uint)player->cur_frame) {
        return;
      }
      if ((uint)player->end_frame < target_frame) {
        if (player->loop == 0) {
          avi_play_pause(player);
          player->call_end_callback();
          return;
        }
        avi_seek_frame(player,player->start_frame,0);
        avi_play_start(player);
        return;
      }
    }
    else {
      sample = player->audio.is_playing();
      if (sample == 0) {
        if (player->loop == 0) {
          avi_play_pause(player);
          player->call_end_callback();
          return;
        }
        avi_seek_frame(player,player->start_frame,0);
        avi_play_start(player);
        return;
      }
      audio_pos_ms = player->audio.get_position_ms();
      sample = AVIStreamTimeToSample(avi_play.video_stream[0],audio_pos_ms);
      target_frame = player->start_frame + sample;
      if ((player->paused == 0) || (target_frame < (uint)player->cur_frame)) {
        snd_avi_stream_feed(0x40);
        avi_blit_union_left_block.g_video_frame_count++;
        return;
      }
    }
    if ((*(uint *)&avi_blit_union_left_block.avi_frame_needs_blit) != 0) {
      if (player->transparent == 0) {
        player->wnd_blit_surface();
      }
      else {
        dirty_rect.left = indeo.view_rect_x[0];
        dirty_rect.top = indeo.view_rect_y;
        dirty_rect.right = indeo.view_rect_x[0] + indeo.view_rect_width;
        dirty_rect.bottom = indeo.view_rect_y + indeo.view_rect_height;
        UnionRect(&dirty_rect,&dirty_rect,(RECT *)((i32 *)&avi_blit_union_left_block.avi_blit_union_left));
        player->wnd_blit_surface_rect((iptr)&dirty_rect);
        avi_blit_union_left_block.avi_blit_union_left[0] = dirty_rect.left;
        avi_blit_union_left_block.avi_blit_union_top = dirty_rect.top;
        avi_blit_union_left_block.avi_blit_union_right = dirty_rect.right;
        avi_blit_union_left_block.avi_blit_union_bottom = dirty_rect.bottom;
      }
    }
    if ((uint)player->cur_frame < target_frame) {
      key_frame = player->cur_frame;
      do {
        sample = key_frame;
        key_frame++;
        key_frame = AVIStreamFindSample(avi_play.video_stream[0],key_frame,0x11);
        if ((int)target_frame <= key_frame) break;
      } while (key_frame != -1);
      if (player->cur_frame == sample) {
        if ((key_frame != -1) && (key_frame - target_frame < target_frame - player->cur_frame)) {
          player->cur_frame = key_frame;
        }
      }
      else if ((key_frame == -1) || ((int)(target_frame - sample) <= (int)(key_frame - target_frame))) {
        if (sample != -1) {
          player->cur_frame = sample;
        }
      }
      else {
        player->cur_frame = key_frame;
      }
      while (((uint)player->cur_frame < target_frame &&
             ((uint)player->cur_frame < player->end_frame - 1U))) {
        avi_decode_frame(player,0x80000000);
      }
    }
  }
  if ((avi_play.playing != 0) && (player->has_audio != 0)) {
    snd_avi_stream_feed(0x40);
    avi_blit_union_left_block.g_video_frame_count++;
  }
  sample = avi_decode_frame(player,0);
  (*(uint *)&avi_blit_union_left_block.avi_frame_needs_blit) = (uint)(sample == 0);
  return;
}

/* 005e30a1 auto */
void __cdecl avi_apply_palette(AviPlayer *player,GamePalette *palette)

{
  i8 palette_rgb[708];
  int i;
  int result;
  
  if ((player != 0) && (avi_play.file_loaded != 0)) {
    ICSendMessage(avi_play.decompressor,0x403f,0,0);
    palette->get_rgb((iptr)palette_rgb,0,0xec);
    for (i = 0; i < 0xec; i++) {
      player->dst_colors[i * 4] = palette_rgb[i * 3 + 2];
      player->dst_colors[1 + i * 4] = palette_rgb[i * 3 + 1];
      player->dst_colors[2 + i * 4] = palette_rgb[i * 3];
      player->dst_colors[3 + i * 4] = 0;
    }
    ICSendMessage(avi_play.decompressor,0x401d,(iptr)&player->dst_format,0);
    if (player->double_size == 0) {
      result = avi_ic_decompress_ex_begin(avi_play.decompressor,0,(iptr)&player->src_format,0,0,0,player->src_format.width,
                             player->src_format.height,(iptr)&player->dst_format,0,0,0,
                             player->src_format.width,player->src_format.height);
    }
    else {
      player->dst_format.height = player->dst_format.height << 1;
      player->dst_format.width = player->dst_format.width << 1;
      result = avi_ic_decompress_ex_begin(avi_play.decompressor,0,(iptr)&player->src_format,0,0,0,player->src_format.width,
                             player->src_format.height,(iptr)&player->dst_format,0,0,0,
                             player->src_format.width * 2,player->src_format.height * 2);
      player->dst_format.height = player->dst_format.height / 2;
      player->dst_format.width = player->dst_format.width / 2;
    }
    if (result != 0) {
      avi_play.file_loaded = 0;
    }
  }
  return;
}

/* 005e32b2 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl avi_restart_decompress(AviPlayer *player,int transparent)

{
  int yBottom;
  int xRight;
  tagRECT dst_rect;
  tagRECT src_rect;
  i32 cur_frame;
  int result;
  
  if ((player != 0) && (avi_play.file_loaded != 0)) {
    ICSendMessage(avi_play.decompressor,0x403f,0,0);
    if (transparent == 0) {
      indeo.video_flags_a = 0;
    }
    else {
      indeo.video_flags_a = 0x80000004;
    }
    indeo.video_flags_b = 0x80000008;
    ICSendMessage(avi_play.decompressor,0x5001,(DWORD_PTR)((i8 *)&indeo.seq_opts),0x2c);
    ICSendMessage(avi_play.decompressor,0x5001,(DWORD_PTR)((i8 *)&indeo.frame_opts),0x70);
    if (player->double_size == 0) {
      result = avi_ic_decompress_ex_begin(avi_play.decompressor,0,(iptr)&player->src_format,0,0,0,avi_play.frame_width,avi_play.frame_height,
                             (iptr)&player->dst_format,0,0,0,avi_play.frame_width,avi_play.frame_height);
    }
    else {
      player->dst_format.height = player->dst_format.height << 1;
      player->dst_format.width = player->dst_format.width << 1;
      result = avi_ic_decompress_ex_begin(avi_play.decompressor,0,(iptr)&player->src_format,0,0,0,avi_play.frame_width,avi_play.frame_height,
                             (iptr)&player->dst_format,0,0,0,avi_play.frame_width * 2,avi_play.frame_height * 2);
      player->dst_format.height = player->dst_format.height >> 1;
      player->dst_format.width = player->dst_format.width >> 1;
    }
    if (result != 0) {
      avi_play.file_loaded = 0;
    }
    if (transparent == 0) {
      player->backdrop.DibSurface::create(0,0);
    }
    else {
      player->backdrop.DibSurface::create(avi_play.frame_width,avi_play.frame_height);
      yBottom = player->get_height();
      xRight = player->get_width();
      SetRect(&src_rect,0,0,xRight,yBottom);
      dst_rect.left = src_rect.left;
      dst_rect.top = src_rect.top;
      dst_rect.right = src_rect.right;
      dst_rect.bottom = src_rect.bottom;
      OffsetRect(&dst_rect,player->pos_x,player->pos_y);
      if (player->target_surface != 0) {
        ((DibSurface *)player->target_surface)->DibSurface::blit(&player->backdrop,&dst_rect,
                   &src_rect);
      }
      if (avi_play.file_loaded != 0) {
        player->backdrop.DibSurface::blit(player,&src_rect,&src_rect);
        cur_frame = player->cur_frame;
        avi_seek_frame(player,cur_frame,0);
        player->wnd_blit_surface();
      }
    }
  }
  return;
}

/* 005e3550 auto */
void AviPlayer::call_end_callback()

{
  if (on_end != 0) {
    on_end();
  }
  return;
}

/* 005e3580 auto */
void AviPlayer::call_cue_callback()

{
  if (on_frame != 0) {
    on_frame();
  }
  return;
}

/* Part of moviewnd_wndproc (005ec317): WM_GETMINMAXINFO: the window's min and max
   client sizes plus the frame. */
static void moviewnd_on_getminmaxinfo(WndOnMciDoneView *wnd2,uint lparam)
{
  NativeWindow *window;
  int frame_height;
  MINMAXINFO *minmax_info;
  int frame_width;
  int min_height2;
  int min_width2;
  int max_height2;
  int max_width2;

  minmax_info = (MINMAXINFO *)lparam;
  window = ((WndBase *)wnd2)->get_window();
  frame_height = wnd_obj_frame_height(window);
  window = ((WndBase *)wnd2)->get_window();
  frame_width = wnd_obj_frame_width(window);
  ((ModalMenuWindow *)wnd2)->get_min_size((iptr)&min_width2,(iptr)&min_height2);
  ((ModalMenuWindow *)wnd2)->get_max_size((iptr)&max_width2,(iptr)&max_height2);
  minmax_info->ptMaxTrackSize.x = frame_width + max_width2;
  minmax_info->ptMaxTrackSize.y = frame_height + max_height2;
  minmax_info->ptMinTrackSize.x = frame_width + min_width2;
  minmax_info->ptMinTrackSize.y = frame_height + min_height2;
}

/* Part of moviewnd_wndproc (005ec317): WM_HSCROLL: moves the horizontal scroll bar
   (line, page, thumb) and tells the window. */
static void moviewnd_on_hscroll(HWND hwnd,WndOnMciDoneView *wnd2,uint wparam,short thumb_pos)
{
  int value;
  int scroll_pos2;

  scroll_pos2 = ((ModalMenuWindow *)wnd2)->get_hscroll_pos();
  switch(wparam) {
  case 0:
    scroll_pos2--;
    SetScrollPos(hwnd,0,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,0);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->hscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_hscroll(scroll_pos2);
    return;
  case 1:
    scroll_pos2++;
    SetScrollPos(hwnd,0,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,0);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->hscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_hscroll(scroll_pos2);
    return;
  case 2:
    value = ((ModalMenuWindow *)wnd2)->get_hscroll_page();
    scroll_pos2 -= value;
    SetScrollPos(hwnd,0,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,0);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->hscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_hscroll(scroll_pos2);
    return;
  case 3:
    value = ((ModalMenuWindow *)wnd2)->get_hscroll_page();
    scroll_pos2 += value;
    SetScrollPos(hwnd,0,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,0);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->hscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_hscroll(scroll_pos2);
    return;
  case 4:
    scroll_pos2 = (int)thumb_pos;
    SetScrollPos(hwnd,0,scroll_pos2,1);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->hscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_hscroll(*(i32 *)wnd2->hscroll_pos);
    return;
  case 5:
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    ((WndHandlers *)wnd2->handlers)->call_hscroll_track((int)thumb_pos);
    return;
  default:
    return;
  }
}

/* Part of moviewnd_wndproc (005ec317): WM_VSCROLL: moves the vertical scroll bar
   (line, page, thumb) and tells the window. */
static void moviewnd_on_vscroll(HWND hwnd,WndOnMciDoneView *wnd2,uint wparam,short thumb_pos)
{
  int value;
  int scroll_pos2;

  scroll_pos2 = ((ModalMenuWindow *)wnd2)->get_vscroll_pos();
  switch(wparam) {
  case 0:
    scroll_pos2--;
    SetScrollPos(hwnd,1,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,1);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->vscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_vscroll(scroll_pos2);
    return;
  case 1:
    scroll_pos2++;
    SetScrollPos(hwnd,1,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,1);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->vscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_vscroll(scroll_pos2);
    return;
  case 2:
    value = ((ModalMenuWindow *)wnd2)->get_vscroll_page();
    scroll_pos2 -= value;
    SetScrollPos(hwnd,1,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,1);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->vscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_vscroll(scroll_pos2);
    return;
  case 3:
    value = ((ModalMenuWindow *)wnd2)->get_vscroll_page();
    scroll_pos2 += value;
    SetScrollPos(hwnd,1,scroll_pos2,1);
    scroll_pos2 = GetScrollPos(hwnd,1);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->vscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_vscroll(scroll_pos2);
    return;
  case 4:
    scroll_pos2 = (int)thumb_pos;
    SetScrollPos(hwnd,1,scroll_pos2,1);
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    *(int *)wnd2->vscroll_pos = scroll_pos2;
    ((WndHandlers *)wnd2->handlers)->call_vscroll(*(i32 *)wnd2->vscroll_pos);
    return;
  case 5:
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    ((WndHandlers *)wnd2->handlers)->call_vscroll_track((int)thumb_pos);
    return;
  default:
    return;
  }
}

/* 005ec317 auto */
i32 __stdcall moviewnd_wndproc(HWND hwnd,uint msg,uint wparam,uint lparam)

{
  short thumb_pos;
  i32 result;
  WndBase *wnd_obj;
  BOOL is_iconic;
  int value;
  i8 generic_parms[4];
  NativeWindow *parent_window;
  WndOnMciDoneView *wnd2;
  iptr mci_device2;
  
  wnd2 = (WndOnMciDoneView *)GetWindowLongA(hwnd,4);
  if (wnd2 == (WndOnMciDoneView *)0x0) {
    result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
    return result;
  }
  if (msg < 0x11) {
    if (msg != 0x10) {
      switch(msg) {
      case 3:
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_move(lparam & 0xffff,lparam >> 0x10);
        result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
        return result;
      default:
        result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
        return result;
      case 5:
handle_size:
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        is_iconic = IsIconic(hwnd);
        if (is_iconic == 0) {
          ((ModalMenuWindow *)wnd2)->on_size_nop(lparam & 0xffff,lparam >> 0x10);
          ((WndHandlers *)wnd2->handlers)->call_size(wparam);
        }
        result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
        return result;
      case 6:
        goto handle_mci;
      case 7:
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_setfocus();
        return 0;
      }
    }
    (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
    value = ((WndHandlers *)wnd2->handlers)->call_close();
    if (value != 0) {
      result = wnd_base_wndproc(hwnd,0x10,wparam,lparam);
      return result;
    }
  }
  else {
    if (msg < 0x45) {
      if (msg == 0x44) {
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_msg44(lparam & 0xffff);
        return 1;
      }
      if (msg == 0x24) {
        moviewnd_on_getminmaxinfo(wnd2,lparam);
        return 0;
      }
      result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    if (msg < 0x112) {
      if (msg == 0x111) {
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        return 0;
      }
      if (msg == 0x100) {
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        result = input_map_vkey(wparam);
        ((WndHandlers *)wnd2->handlers)->call_keydown_pre(result);
        result = input_map_vkey(wparam);
        ((WndHandlers *)wnd2->handlers)->call_keydown(result);
        return 0;
      }
      if (msg == 0x101) {
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        result = input_map_vkey(wparam);
        ((WndHandlers *)wnd2->handlers)->call_keyup(result);
        return 0;
      }
      if (msg == 0x102) {
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_char(wparam & 0xff);
        return 0;
      }
      result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    if (msg < 0x201) {
      if (msg == 0x200) {
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_mousemove(lparam & 0xffff,lparam >> 0x10);
        return 0;
      }
      if (msg == 0x112) {
        if (wparam == 0xf020) {
          (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
          ((WndHandlers *)wnd2->handlers)->call_minimize();
        }
        else {
          (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
          if (wparam == 0xf120) {
            ((WndHandlers *)wnd2->handlers)->call_restore();
          }
        }
        result = wnd_base_wndproc(hwnd,0x112,wparam,lparam);
        return result;
      }
      thumb_pos = (short)(wparam >> 0x10);
      if (msg == 0x114) {
        moviewnd_on_hscroll(hwnd,wnd2,wparam,thumb_pos);
        return 0;
      }
      if (msg == 0x115) {
        moviewnd_on_vscroll(hwnd,wnd2,wparam,thumb_pos);
        return 0;
      }
      result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    if (msg < 0x211) {
      if (msg == 0x210) {
        if (wparam == 0x201) {
          (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
          ((WndHandlers *)wnd2->handlers)->call_lbuttondown(lparam & 0xffff,lparam >> 0x10);
          return 0;
        }
        if (wparam != 0x204) {
          return 0;
        }
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_rbuttondown(lparam & 0xffff,lparam >> 0x10);
        return 0;
      }
      switch(msg) {
      case 0x201:
        wnd_obj = (WndBase *)GetWindowLongA(hwnd,0);
        parent_window = wnd_obj->get_window();
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        if ((parent_window->flags_bytes[1] & 2) != 0) {
          SetFocus(parent_window->hwnd);
          BringWindowToTop(hwnd);
        }
        wnd2->wnd_header_area[0xa4] = 1;
        wnd2->wnd_header_area[0xa5] = '\0';
        wnd2->wnd_header_area[0xa6] = '\0';
        wnd2->wnd_header_area[0xa7] = '\0';
        ((WndHandlers *)wnd2->handlers)->call_lbuttondown(lparam & 0xffff,lparam >> 0x10);
        return 0;
      case 0x202:
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        if (*(int *)wnd2->lbutton_held == 1) {
          wnd2->wnd_header_area[0xa4] = '\0';
          wnd2->wnd_header_area[0xa5] = '\0';
          wnd2->wnd_header_area[0xa6] = '\0';
          wnd2->wnd_header_area[0xa7] = '\0';
          ((WndHandlers *)wnd2->handlers)->call_lbuttonup(lparam & 0xffff,lparam >> 0x10);
        }
        ((WndHandlers *)wnd2->handlers)->call_lbuttonup_raw(lparam & 0xffff,lparam >> 0x10);
        return 0;
      case 0x203:
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        ((WndHandlers *)wnd2->handlers)->call_dblclick(lparam & 0xffff,lparam >> 0x10);
        return 0;
      case 0x204:
        wnd_obj = (WndBase *)GetWindowLongA(hwnd,0);
        parent_window = wnd_obj->get_window();
        if ((parent_window->flags_bytes[1] & 2) != 0) {
          SetFocus(parent_window->hwnd);
          BringWindowToTop(hwnd);
        }
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        wnd2->wnd_header_area[0xa8] = 1;
        wnd2->wnd_header_area[0xa9] = '\0';
        wnd2->wnd_header_area[0xaa] = '\0';
        wnd2->wnd_header_area[0xab] = '\0';
        ((WndHandlers *)wnd2->handlers)->call_rbuttondown(lparam & 0xffff,lparam >> 0x10);
        return 0;
      case 0x205:
        (*(WndOnMciDoneView * *)&modal_wnd.active_child) = wnd2;
        if (*(int *)wnd2->rbutton_held == 1) {
          wnd2->wnd_header_area[0xa8] = '\0';
          wnd2->wnd_header_area[0xa9] = '\0';
          wnd2->wnd_header_area[0xaa] = '\0';
          wnd2->wnd_header_area[0xab] = '\0';
          ((WndHandlers *)wnd2->handlers)->call_rbuttonup(lparam & 0xffff,lparam >> 0x10);
        }
        ((WndHandlers *)wnd2->handlers)->call_rbuttonup_raw(lparam & 0xffff,lparam >> 0x10);
        return 0;
      }
      result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    if ((msg != 0x30f) && (msg != 0x311)) {
      if (msg == 0x3b9) {
        if (wparam == 1) {
          wnd_call_mci_done(wnd2);
          return 0;
        }
        goto handle_size;
      }
      result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
handle_mci:
    mci_device2 = wnd_get_mci_device((WndMciDeviceView *)wnd2);
    if (mci_device2 == 0) {
      result = wnd_base_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    mciSendCommandA(*(MCIDEVICEID *)(mci_device2 + 8),0x840,*(DWORD_PTR *)(mci_device2 + 0x10),
                    (DWORD_PTR)generic_parms);
  }
  return 0;
}

/* 005eeca0 auto */
AviAudio *AviAudio::construct()

{
  audio_stream = 0;
  return this;
}

/* 005eed1b auto */
void AviAudio::release()

{
  if (audio_stream != 0) {
    snd_avi_stream_stop();
  }
  return;
}

/* 005eed43 auto */
uint AviAudio::is_playing()

{
  return (uint)((flags & 4) != 0);
}

/* 005eed76 auto */
int AviAudio::attach_stream(u32 stream)

{
  int result;
  
  result = snd_wave_add_avi_stream(stream);
  if (result == 0) {
    audio_stream = stream;
    flags = flags | 1;
  }
  else {
    flags = 0;
  }
  return result;
}

/* 005eedd1 auto */
i32 AviAudio::get_position_ms()

{
  i32 position_ms;
  
  position_ms = snd_wave_get_position_ms();
  return position_ms;
}

/* 005eedec auto */
int AviAudio::prepare_range(i32 start_time,i32 end_time)

{
  int result;
  
  result = snd_avi_stream_prepare(start_time,end_time,(iptr)this);
  if (result == 0) {
    flags = flags | 2;
  }
  else {
    flags = flags & 0xfffd;
  }
  return result;
}

/* 005eee4f auto */
i16 AviAudio::start()

{
  snd_avi_stream_start();
  return 0;
}
