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

    static int32_t last_frame_ticks;
    int32_t start_ticks;

    display_unlock();

    game_handle_sdl_events();

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

/* v0.8 experimental camera-only step control.
 * Default speed 3: exactly one call per input, identical to v0.7.
 * [ and ] decrease/increase speed. SDL preference path persists choice.
 * This is a keyboard prototype, NOT the final options-menu slider.
 */
static int camera_speed_value = 3;
static int camera_speed_loaded = 0;
static int camera_speed_prev_down = 0;
static int camera_speed_prev_up = 0;
static int camera_speed_fraction = 0;
static char *camera_speed_pref_dir = NULL;

static void camera_speed_save(void)
{
    char path[1024];
    FILE *f;
    if (!camera_speed_pref_dir) return;
    if (snprintf(path,sizeof(path), "%scamera-speed.cfg",camera_speed_pref_dir) >= (int)sizeof(path)) return;
    f=fopen(path,"w");
    if (f) { fprintf(f,"%d\n",camera_speed_value); fclose(f); }
}

static void camera_speed_load(void)
{
    char path[1024];
    FILE *f;
    int saved;
    if (camera_speed_loaded) return;
    camera_speed_loaded=1;
    camera_speed_pref_dir=SDL_GetPrefPath("Syndicate2026","Syndicate2026");
    if (!camera_speed_pref_dir) return;
    if (snprintf(path,sizeof(path),"%scamera-speed.cfg",camera_speed_pref_dir) >= (int)sizeof(path)) return;
    f=fopen(path,"r");
    if (!f) return;
    if (fscanf(f,"%d",&saved)==1 && saved>=1 && saved<=5) camera_speed_value=saved;
    fclose(f);
}

int camera_scroll_steps(void)
{
    const Uint8 *keys;
    int down,up;
    int steps;
    camera_speed_load();
    keys=SDL_GetKeyboardState(NULL);
    down=keys && keys[SDL_SCANCODE_LEFTBRACKET];
    up=keys && keys[SDL_SCANCODE_RIGHTBRACKET];
    if (down && !camera_speed_prev_down && camera_speed_value>1) {
        --camera_speed_value; camera_speed_fraction=0; camera_speed_save();
    }
    if (up && !camera_speed_prev_up && camera_speed_value<5) {
        ++camera_speed_value; camera_speed_fraction=0; camera_speed_save();
    }
    camera_speed_prev_down=down;
    camera_speed_prev_up=up;
    if (camera_speed_value == 3) return 1;
    if (camera_speed_value == 5) return 2;
    /* 1: 50%, 2: 75%, 4: 150% of original directional steps. */
    camera_speed_fraction += (camera_speed_value==1) ? 2 :
        (camera_speed_value==2) ? 3 : 6;
    steps = camera_speed_fraction / 4;
    camera_speed_fraction %= 4;
    return steps;
}
