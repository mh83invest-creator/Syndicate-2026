#!/usr/bin/env python3
"""Apply experimental camera-only speed stepping to the confirmed v0.7 source."""
from pathlib import Path
p=Path("engine/src/game.c")
s=p.read_text()
assert "int camera_scroll_steps(void)" not in s
s += r"""
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
"""
p.write_text(s)
p=Path("engine/src/game.h")
s=p.read_text()
assert "camera_scroll_steps" not in s
s += "\n/* v0.8 experimental camera-only steps; normal gameplay tick unchanged. */\nint camera_scroll_steps(void);\n"
p.write_text(s)
p=Path("engine/conf/wrappers_game.conf")
s=p.read_text()
assert "camera_scroll_steps" not in s
assert "game_update_menu" in s
s=s.replace("game_update_menu", "camera_scroll_steps\t\t\tC\ti\ngame_update_menu",1)
p.write_text(s)
p=Path("engine/src/syndre.sx")
s=p.read_text()
anchor="scroll_map:\t/* 0x1e400 */"
endanchor="vtable_1e4b0:"
assert s.count(anchor)==1 and s.count(endanchor)==1
a=s.index(anchor); b=s.index(endanchor,a)
part=s[a:b]
assert part.count("call   scroll")==7,part.count("call   scroll")
part=part.replace("call   scroll","call   camera_scroll_controlled")
wrapper="""/* v0.8: helper controls number of *camera-only* scroll steps.
 * Default one step, exactly as v0.7. No change to engine update pacing.
 */
camera_scroll_controlled:
\t\tpush   %ebx
\t\tcall   ac_camera_scroll_steps
\t\tmov    %eax,%ebx
\t\ttest   %ebx,%ebx
\t\tjle    camera_scroll_controlled_done
camera_scroll_controlled_repeat:
\t\tmov    0x8(%esp),%eax
\t\tpush   %eax
\t\tcall   scroll
\t\tadd    $0x4,%esp
\t\tdec    %ebx
\t\tjg     camera_scroll_controlled_repeat
camera_scroll_controlled_done:
\t\tpop    %ebx
\t\tret

"""
s=s[:a]+part+wrapper+s[b:]
assert s.count("call   camera_scroll_controlled")==7
assert s.count("camera_scroll_controlled_repeat:")==1
p.write_text(s)
print("Applied camera-only five-step controller prototype with v0.7 default")
