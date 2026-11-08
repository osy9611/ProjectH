"""Runs inside one offscreen -game process: for MAP_Village_Day and then MAP_Village_Night it streams in
MAP_ArtSpec_Props, looks through each ArtSpec camera and saves an engine screenshot to ARTSPEC_OUT/<map>_<shot>.png.
Started by run_artspec.ps1. Result lines go to the game log with the prefix [ArtSpec].
"""
import os, shutil, unreal

OUT = os.environ['ARTSPEC_OUT']
SHOT_DIR = os.path.abspath(unreal.Paths.screen_shot_dir())
PROPS = '/Game/ArtSpec/MAP_ArtSpec_Props'
SHOTS = ('chars',)


def log(*a):
    unreal.log('[ArtSpec] ' + ' '.join(str(x) for x in a))


def world():
    for gi in unreal.ObjectIterator(unreal.GameInstance):
        w = gi.get_world()
        if w:
            return w
    return None


def console(cmd):
    unreal.SystemLibrary.execute_console_command(world(), cmd)


def pngs():
    found = set()
    for root, _, files in os.walk(SHOT_DIR):
        found.update(os.path.join(root, f) for f in files if f.lower().endswith('.png'))
    return found


state = dict(map='Day', pending=None)


def setup(_):
    os.makedirs(SHOT_DIR, exist_ok=True)
    console('r.SetRes 1920x1080w')
    _, ok = unreal.LevelStreamingDynamic.load_level_instance(world(), PROPS, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    log(state['map'], 'props loaded', ok, 'world', world().get_name())


def view(shot):
    # Shots share one camera and one stretch of path; only this shot's sprites are shown.
    w = world()
    for other in SHOTS:
        for a in unreal.GameplayStatics.get_all_actors_with_tag(w, 'ArtSpec_sprite_' + other):
            a.set_actor_hidden_in_game(other != shot)
    pc = unreal.GameplayStatics.get_player_controller(w, 0)
    cams = unreal.GameplayStatics.get_all_actors_with_tag(w, 'ArtSpec_camera')
    if not cams:
        log('camera missing')
        return
    pc.set_view_target_with_blend(cams[0], 0.0)
    log('view', shot)


def shot(shot):
    view(shot)  # the game camera may have taken the view back since
    state['pending'] = ('%s_%s' % (state['map'], shot), pngs())
    console('shot')


def travel(name):
    state['map'] = name
    console('open /Game/Maps/Village/MAP_Village_' + name)


def quit_game(_):
    unreal.SystemLibrary.quit_game(world(), None, unreal.QuitPreference.QUIT, False)


def map_steps(t):
    # Load the props, then frame and capture each shot in turn.
    steps = [(t, setup, None)]
    for i, name in enumerate(SHOTS):
        steps += [(t + 6 + i * 7, view, name), (t + 11 + i * 7, shot, name)]
    return steps


STEPS = map_steps(10) + [(38, travel, 'Night')] + map_steps(46) + [(76, quit_game, None)]
elapsed = 0.0


def collect():
    if not state['pending']:
        return
    tag, before = state['pending']
    new = sorted(pngs() - before, key=os.path.getmtime)
    if new:
        try:
            shutil.move(new[0], os.path.join(OUT, tag + '.png'))
            log('saved', tag)
            state['pending'] = None
        except OSError:
            pass


def tick(dt):
    global elapsed
    # A long load hitch must not fire several steps in one frame.
    elapsed += min(dt, 0.25)
    collect()
    while STEPS and STEPS[0][0] <= elapsed:
        _, fn, arg = STEPS.pop(0)
        try:
            fn(arg)
        except Exception as e:
            log('step failed', fn.__name__, repr(e))


unreal.register_slate_post_tick_callback(tick)
log('armed')
