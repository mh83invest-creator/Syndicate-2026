/******************************************************************************/
// SyndicatFX, source port of the classic game from Bullfrog.
/******************************************************************************/
/** @file game.c
 *     Functions for high level game functionalities.
 * @par Purpose:
 *     Routines on game loop level, creating eneral design of the game.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     12 Nov 2008 - 25 May 2022
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bfkeybd.h"
#include "bfmouse.h"
#include "bfdata.h"
#include "bffile.h"
#include "bfmemory.h"
#include "bfmemut.h"
#include "bfpalette.h"
#include "bfscreen.h"
#include "bfsprite.h"
#include "bfwindows.h"
#include "bfaudio.h"
#include "bfmusic.h"
#include "bfsound.h"
#include "bfsvaribl.h"
#include "bfscd.h"
#include "bfutility.h"
#include "ssampply.h"

#include <SDL.h>
#include "applog.h"
#include "game_data.h"
#include "display.h"
#include "dos.h"
#include "game.h"
#include "keyboard.h"
#include "mouse.h"
#include "osunix.h"
#include "oswindws.h"
#include "sound.h"
#include "util.h"
#include "timer.h"

#define SAVEGAME_PATH "save/"

#if defined(SYNDICATE_DIAGNOSTICS)
/* Bounded C-level frame-pacing measurements. Nothing is logged in a hot frame. */
struct SyndicateFrameDiagnostics {
    Uint64 frames;
    Uint64 event_ticks;
    Uint64 wait_ticks;
    Uint64 total_ticks;
};
static struct SyndicateFrameDiagnostics synd_diag[2];

static void syndicate_diagnostics_report(void)
{
    const Uint64 freq = SDL_GetPerformanceFrequency();
    char *pref_path = SDL_GetPrefPath("Syndicate2026", "Diagnostics");
    FILE *out = stderr;
    int mode;
    if (pref_path != NULL) {
        char file_path[1024];
        int len = snprintf(file_path, sizeof(file_path),
          "%ssyndicate-frame-diagnostics.txt", pref_path);
        if (len > 0 && len < (int)sizeof(file_path)) {
            FILE *f = fopen(file_path, "w");
            if (f != NULL)
                out = f;
        }
        SDL_free(pref_path);
    }
    if (freq == 0) {
        if (out != stderr)
            fclose(out);
        return;
    }
    for (mode = 0; mode != 2; ++mode) {
        const struct SyndicateFrameDiagnostics *d = &synd_diag[mode];
        const double factor = 1000.0 / (double)freq;
        if (d->frames == 0)
            continue;
        fprintf(out,
          "SYND_DIAG mode=%s frames=%llu event_ms=%.3f wait_ms=%.3f update_ms=%.3f avg_event_ms=%.3f avg_update_ms=%.3f\n",
          mode == 0 ? "tactical16" : "menu30",
          (unsigned long long)d->frames,
          (double)d->event_ticks * factor,
          (double)d->wait_ticks * factor,
          (double)d->total_ticks * factor,
          (double)d->event_ticks * factor / (double)d->frames,
          (double)d->total_ticks * factor / (double)d->frames);
    }
    fflush(out);
    if (out != stderr)
        fclose(out);
}
#endif


/******************************************************************************/

bool
game_initialise (void)
{
    TbResult ret;

    // Check for level data offsets consistency
    assert((int)((ubyte *)(level__CPObjectives) - (ubyte *)&level__Seed) == 0x9BEB2 - 0x80108);
    assert((int)((ubyte *)(level__CPObjectives + 128) - (ubyte *)&level__Seed) == 0x9C632 - 0x80108);

    ret = LbBaseInitialise();
    if (ret != Lb_SUCCESS)
    {
        LOGERR("Bullfrog Library initialization failed");
        return false;
    }

#ifdef __unix__
    /* clean up after SDL messing around where it shouldn't */
    unix_restore_signal_handlers();
#endif

    LbSetUserResourceMapping(AppResourceMapping);
    LbSetTitle(PACKAGE_NAME);
    LbSetIcon(1);

    // Make sure file names are properly converted before opening
    setup_file_names();

    return true;
}

void
game_handle_sdl_events (void)
{
    TbBool contn;
    contn = LbWindowsControl();
    if (!contn) {
        game_quit();
    }
    mouse_update_scaled_coords();
}

void
game_quit(void)
{
#if defined(SYNDICATE_DIAGNOSTICS)
    syndicate_diagnostics_report();
#endif
    host_reset();
    LbBaseReset();
    exit(0);
}

void
game_transform_path_full(const char *file_name, char *buffer, size_t size)
{
    if (strncasecmp (file_name, SAVEGAME_PATH,
             sizeof (SAVEGAME_PATH) - 1) == 0)
    {
        snprintf (buffer, size, "%s" FS_SEP_STR "%s", GetDirectoryUser(),
          file_name + sizeof (SAVEGAME_PATH) - 1);
        return;
    }

    /* abort on absolute paths */
    if (file_name[0] == '\\' || file_name[0] == '/'
        || (strlen (file_name) >= 2 && file_name[1] == ':'))
    {
        snprintf (buffer, size, "%s", file_name);
        return;
    }

    snprintf (buffer, size, "%s" FS_SEP_STR "%s", GetDirectoryHdd(), file_name);
}

void
game_transform_path(const char *file_name, char *result)
{
    game_transform_path_full (file_name, result, FILENAME_MAX);
}

static void
game_update_full(bool wait, int max_fps)
{
    const int32_t frame_duration = 1000 / max_fps;
#if defined(SYNDICATE_DIAGNOSTICS)
    const Uint64 diag_start = SDL_GetPerformanceCounter();
    Uint64 diag_event_ticks;
    Uint64 diag_wait_start;
    struct SyndicateFrameDiagnostics *diag =
      &synd_diag[max_fps == 30 ? 1 : 0];
#endif

    static int32_t last_frame_ticks;
    int32_t start_ticks;

    display_unlock();

#if defined(SYNDICATE_DIAGNOSTICS)
    {
        Uint64 begin = SDL_GetPerformanceCounter();
        game_handle_sdl_events();
        diag_event_ticks = SDL_GetPerformanceCounter() - begin;
    }
    diag_wait_start = SDL_GetPerformanceCounter();
#else
    game_handle_sdl_events();
#endif

    start_ticks = SDL_GetTicks();

    if (wait && last_frame_ticks != 0)
    {
        int32_t last_frame_duration;

        last_frame_duration = (int32_t)(start_ticks - last_frame_ticks + 2);

        if (last_frame_duration < frame_duration)
        {
            int32_t total_sleep_time = frame_duration - last_frame_duration;
            const int32_t min_sleep_time = 1000 / 40;
            const int32_t max_sleep_time = 1000 / 20;

            total_sleep_time = frame_duration - last_frame_duration;

            if (total_sleep_time > 0)
            {
                float f = (float) total_sleep_time
                    * (min_sleep_time + max_sleep_time)
                    / (2 * min_sleep_time * max_sleep_time);
                int32_t base_sleep_time = (int32_t)(total_sleep_time / f + .5f);

                while (total_sleep_time > 0)
                {
                    int32_t sleep_time = min(base_sleep_time, total_sleep_time);
                    int32_t ticks = SDL_GetTicks();

                    SDL_Delay(sleep_time);

                    display_lock();
                    game_handle_sdl_events();
                    display_unlock();

                    total_sleep_time -= SDL_GetTicks() - ticks;
                }
            }
        }
    }

    last_frame_ticks = SDL_GetTicks();

    display_lock();
#if defined(SYNDICATE_DIAGNOSTICS)
    diag->frames++;
    diag->event_ticks += diag_event_ticks;
    diag->wait_ticks += SDL_GetPerformanceCounter() - diag_wait_start;
    diag->total_ticks += SDL_GetPerformanceCounter() - diag_start;
#endif
}

void reset_input(void)
{
    LbMouseReset();
    LbKeyboardClose();
}

void
game_update(void)
{
    game_update_full(true, 16);
}

void host_reset(void)
{
# if defined(DOS)
    ShutdownMIDI();
    FreeSound();
#else
    FreeAudio();
#endif
    reset_input();
    LbScreenReset();
}
/******************************************************************************/

/* Experimental menu-only cadence. Mission update remains 16 FPS. */
void game_update_menu(void)
{
    game_update_full(true, 30);
}
