/* Sound, music and video.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_SOUND_H
#define CIV2_STATE_SOUND_H

/* CD audio music. */
#pragma pack(push, 1)
typedef struct CdMusic {
    int available;  /* 0062b420 */
    i32 unknown_disc;  /* 0062b424 */
    uint current_track;  /* 0062b428 */
    i32 keep_playing;  /* 0062b42c */
} CdMusic;
#pragma pack(pop)
extern CdMusic cd_music;
extern RODATA i8 snd_wav_ext_str[8];
/* The active child window and the depth of modal windows. */
#pragma pack(push, 1)
typedef struct ModalWindows {
    i32 active_child;  /* 00637ea4 */
    iptr depth;  /* 00637ea8 */
} ModalWindows;
#pragma pack(pop)
extern ModalWindows modal_wnd;
/* Digital sound output: the device, its format, buffers and sources (and video soundtracks fed to it). */
#pragma pack(push, 1)
typedef struct WaveOut {
    int out_handle;  /* 00638578 */
    i32 notify_hwnd;  /* 0063857c */
    i32 open_buffer_bytes;  /* 00638580 */
    uint max_buffers;  /* 00638584 */
    uint buffer_count;  /* 00638588 */
    short source_count;  /* 0063858c */
    u8 source_count_rest[2];
    i32 avi_feed_count;  /* 00638590 */
    i32 avi_fill_count;  /* 00638594 */
    i32 avi_feed_samples;  /* 00638598 */
    u8 avi_feed_samples_rest[4];
    i32 format[1];  /* 006385a0 */
    i32 samples_per_sec;  /* 006385a4 */
    i32 avg_bytes_per_sec;  /* 006385a8 */
    i8 block_align;  /* 006385ac */
    u8 block_align_rest[3];
    int device_open;  /* 006385b0 */
    i32 buffers_ready;  /* 006385b4 */
    i16 stop_pending;  /* 006385b8 */
    u8 stop_pending_rest[2];
    i32 avi_stream_started;  /* 006385bc */
    WaveBuffer *buffer_ring;  /* 006385c0 */
    WaveBuffer *spare_buffers;  /* 006385c4 */
    WaveBuffer *cur_buffer;  /* 006385c8 */
    LONG total_buffer_bytes;  /* 006385cc */
    WaveSource *sources;  /* 006385d0 */
    i32 stop_drained_msg_count;  /* 006385d4 */
} WaveOut;
#pragma pack(pop)
extern WaveOut snd_wave;
/* The video player and the MIDI and CD audio devices open. */
#pragma pack(push, 1)
typedef struct MediaDevices {
    i32 avi_player;  /* 006389d0 */
    uint midi_device_id;  /* 006389d4 */
    MCIDEVICEID cd_device_id;  /* 006389d8 */
    u8 cd_device_id_rest[4];
    i32 cd_track_count;  /* 006389e0 */
} MediaDevices;
#pragma pack(pop)
extern MediaDevices media_devices;
/* The video being played. */
#pragma pack(push, 1)
typedef struct AviPlayback {
    LPVOID frame_buffer;  /* 00638da8 */
    i32 frame_buffer_size;  /* 00638dac */
    i32 decompressor;  /* 00638db0 */
    int frame_width;  /* 00638db4 */
    int frame_height;  /* 00638db8 */
    i32 start_time;  /* 00638dbc */
    i32 file[1];  /* 00638dc0 */
    i32 video_stream[1];  /* 00638dc4 */
    i32 audio_stream[1];  /* 00638dc8 */
    int file_loaded;  /* 00638dcc */
    int playing;  /* 00638dd0 */
    AviPlayer *active_window;  /* 00638dd4 */
} AviPlayback;
#pragma pack(pop)
extern AviPlayback avi_play;
#pragma pack(push, 1)
struct avi_blit_union_left_block {
    i32 avi_blit_union_left[1];  /* 00638dd8 */
    int avi_blit_union_top;  /* 00638ddc */
    int avi_blit_union_right;  /* 00638de0 */
    int avi_blit_union_bottom;  /* 00638de4 */
    i32 avi_frame_needs_blit;  /* 00638de8 */
    i32 g_video_frame_count;  /* 00638dec */
};
#pragma pack(pop)
extern struct avi_blit_union_left_block avi_blit_union_left_block;
/* The Indeo video codec's options and the video view rectangle. */
#pragma pack(push, 1)
typedef struct IndeoOptions {
    i32 frame_opts[1];  /* 006e5028 */
    i32 frame_fourcc;  /* 006e502c */
    i32 frame_version;  /* 006e5030 */
    i32 frame_type;  /* 006e5034 */
    i32 frame_environment;  /* 006e5038 */
    i32 video_flags_b;  /* 006e503c */
    u8 video_flags_b_rest[36];
    i32 view_rect_ptr;  /* 006e5064 */
    u8 view_rect_ptr_rest[48];
    i32 seq_opts[1];  /* 006e5098 */
    i32 seq_fourcc;  /* 006e509c */
    i32 seq_version;  /* 006e50a0 */
    i32 seq_type;  /* 006e50a4 */
    i32 seq_environment;  /* 006e50a8 */
    i32 video_flags_a;  /* 006e50ac */
    u8 video_flags_a_rest[12];
    i32 seq_extra;  /* 006e50bc */
    u8 seq_extra_rest[4];
    int vfw_initialized;  /* 006e50c4 */
    i32 view_rect_x[1];  /* 006e50c8 */
    i32 view_rect_y;  /* 006e50cc */
    i32 view_rect_width;  /* 006e50d0 */
    i32 view_rect_height;  /* 006e50d4 */
    i32 frames_per_second;  /* 006e50d8 */
} IndeoOptions;
#pragma pack(pop)
#pragma pack(push, 1)
struct err_report_ctx_block {
    u8 err_report_ctx[1808];  /* 006e4850 */
    u32 res_dll_modules[35];  /* 006e4f60 HMODULE */
    i32 app_prev_instance;  /* 006e4fec */
    HINSTANCE app_hinstance;  /* 006e4ff0 */
    u8 app_hinstance_rest[4];
    HWND snd_mci_notify_hwnd;  /* 006e4ff8 */
    u8 snd_mci_notify_hwnd_rest[4];
    MusicNotifyFn snd_cd_notify_callback;  /* 006e5000 */
    MusicNotifyFn snd_midi_notify_callback;  /* 006e5004 */
    u8 snd_midi_notify_callback_rest[4];
    int palette_static_half_count;  /* 006e500c */
    u8 palette_static_half_count_rest[8];
    i32 choice_dlg_selection;  /* 006e5018 */
    u8 choice_dlg_selection_rest[4];
    struct SurfaceWindow *choice_dlg_window;  /* 006e5020 the choice dialog's window (FUN_005e0863) */
    u8 choice_dlg_window_rest[4];
    IndeoOptions indeo;  /* 006e5028 */
    i32 avi_indeo_frame_extra_i32;  /* 006e50dc */
    u8 avi_indeo_frame_extra_i32_rest[8];
    DWORD g_ddcaps_driver;  /* 006e50e8 */
    byte ddraw_hw_caps_byte0;  /* 006e50ec */
    u8 ddraw_hw_caps_byte0_rest[1];
    byte ddraw_hw_caps_byte2;  /* 006e50ee */
    byte ddraw_hw_caps_byte3;  /* 006e50ef */
    u8 ddraw_hw_caps_byte3_rest[312];
    DWORD g_ddcaps_hel;  /* 006e5228 */
    u8 g_ddcaps_hel_rest[316];
    i32 menu_search_pos;  /* 006e5368 */
    u8 menu_search_pos_rest[6144];
};
#pragma pack(pop)
extern struct err_report_ctx_block err_report_ctx_block;
inline constexpr IndeoOptions &indeo = err_report_ctx_block.indeo;

#endif
