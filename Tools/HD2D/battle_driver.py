"""Runs inside a -game process and calls the same functions the UI buttons call.

Start with: -ExecCmds="py <this file>" and HD2D_STEPS set to a ;-separated list of
"seconds:action[:arg]". Times count from the first tick. Result lines go to the game log with the prefix [Driver].
Actions: travel:<SceneTableNo>, state, click:<x>:<y> (real mouse, in client pixels), shot:<tag>, cvar:<name>, quit.
"""
import os, unreal

STEPS = []
for s in os.environ.get('HD2D_STEPS', '').split(';'):
    if s:
        t, _, rest = s.partition(':')
        STEPS.append((float(t), rest))


def log(*a):
    unreal.log('[Driver] ' + ' '.join(str(x) for x in a))


def world():
    for gi in unreal.ObjectIterator(unreal.GameInstance):
        w = gi.get_world()
        if w:
            return w, gi
    return None, None


def travel(arg):
    w, gi = world()
    sub = next(o for o in unreal.ObjectIterator(unreal.HDCommonSessionSubsystem) if not o.get_name().startswith('Default__'))
    log('travel', arg, sub)
    sub.move_seesion_by_table_id(int(arg))


def state(_):
    w, _gi = world()
    log('world', w.get_name())
    names = [a.get_class().get_name() + ':' + a.get_name() for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Pawn)]
    log('pawns', names)
    log('ids', [str(i.export_text()) for i in unreal.SystemLibrary.get_primary_asset_id_list(unreal.PrimaryAssetType('HDExperienceDefinition'))])


def click(arg):
    x, _, y = arg.partition(':')
    open(os.path.join(os.environ['HD2D_OUT'], 'click_%s_%s.req' % (x, y)), 'w').close()
    log('click', arg)


def cvar(arg):
    log(arg, '=', unreal.SystemLibrary.get_console_variable_float_value(arg))


def shot(arg):
    open(os.path.join(os.environ['HD2D_OUT'], arg + '.req'), 'w').close()
    log('shot', arg)


def quit_game(_):
    unreal.SystemLibrary.quit_game(world()[0], None, unreal.QuitPreference.QUIT, False)


ACTIONS = dict(travel=travel, state=state, click=click, shot=shot, cvar=cvar, quit=quit_game)
elapsed = 0.0


def tick(dt):
    global elapsed
    # Long hitches during load would otherwise fire several steps in the same frame.
    elapsed += min(dt, 0.25)
    while STEPS and STEPS[0][0] <= elapsed:
        _, step = STEPS.pop(0)
        name, _, arg = step.partition(':')
        try:
            ACTIONS[name](arg)
        except Exception as e:
            log('step failed', step, repr(e))


unreal.register_slate_post_tick_callback(tick)
log('armed', len(STEPS))
