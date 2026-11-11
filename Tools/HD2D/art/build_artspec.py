"""Imports drawn characters (Art/Characters) and builds /Game/ArtSpec/MAP_ArtSpec_Props, a level that artspec_driver.py
streams into the village maps at runtime. Safe to rerun. Touches no shared material, light or post process asset.

Sprites use the spec import settings: Nearest filter, Pixels2D group, no mips, uncompressed, PPUU and pivot from spec.py.
Before saving it traces the ground under every sprite and the line from each camera to every sprite it frames,
so a blocked or floating placement shows up here instead of in a failed game run.

Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file> -unattended -nullrhi
Result lines go to Client/Saved/ArtSpec/build.txt.
"""
import math, os, sys, unreal

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spec

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
ART = os.path.join(ROOT, 'Art', 'Characters')
DEST = '/Game/ArtSpec/Characters'
LEVEL = '/Game/ArtSpec/MAP_ArtSpec_Props'
GEO = '/Game/Maps/Village/MAP_Village_Geo'
SPRITE_MATERIAL = '/Game/HD2D/M_HD2D_SpriteLift'
GROUND_Z = 1001.6
ROW_Y = 12070.0  # the hero's PlayerStart row, so the hero stands in the same shot for scale
AIM_HEIGHT = 75.0
CAMERA_X = 14110.0
# Shot name -> sprites as (x, type, direction). The stone path only spans about x 13834-14351 on this row
# (grass, which has no collision, hides feet outside it), so both shots share it and the driver shows one set at a time.
SHOTS = {
    # Drawn characters stand beside the current hero for scale: (x, '<Name>', 'FR') per character under Art/Characters.
    'chars': [],
}

EAL = unreal.EditorAssetLibrary
ACT = unreal.EditorLevelLibrary


def log_to_file():
    out = os.path.join(unreal.Paths.project_saved_dir(), 'ArtSpec')
    os.makedirs(out, exist_ok=True)
    sys.stdout = open(os.path.join(out, 'build.txt'), 'w', encoding='utf-8', buffering=1)
    sys.stderr = sys.stdout


def import_texture(png, dest):
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', png)
    task.set_editor_property('destination_path', dest)
    task.set_editor_property('replace_existing', True)
    task.set_editor_property('automated', True)
    task.set_editor_property('save', False)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    tex = unreal.load_asset(dest + '/' + os.path.splitext(os.path.basename(png))[0])
    tex.set_editor_property('filter', unreal.TextureFilter.TF_NEAREST)
    tex.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_PIXELS2D)
    tex.set_editor_property('mip_gen_settings', unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
    tex.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_EDITOR_ICON)
    EAL.save_loaded_asset(tex, False)
    return tex


def make_sprite(tex, dest, name):
    path = dest + '/' + name
    # Updated in place on reruns: the props level references these sprites, so deleting them half fails.
    if EAL.does_asset_exist(path):
        sprite = unreal.load_asset(path)
    else:
        sprite = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, dest, unreal.PaperSprite, unreal.PaperSpriteFactory())
    sprite.set_editor_property('source_texture', tex)
    sprite.set_editor_property('source_uv', unreal.Vector2D(0, 0))
    sprite.set_editor_property('source_dimension', unreal.Vector2D(spec.CELL_W, spec.CELL_H))
    sprite.set_editor_property('pixels_per_unreal_unit', spec.PIXELS_PER_UNREAL_UNIT)
    sprite.set_editor_property('pivot_mode', unreal.SpritePivotMode.CUSTOM)
    sprite.set_editor_property('custom_pivot_point', unreal.Vector2D(*spec.PIVOT))
    sprite.set_editor_property('sprite_collision_domain', unreal.SpriteCollisionMode.NONE)
    EAL.save_loaded_asset(sprite, False)
    return sprite


def sprite_png(key, view):
    return os.path.join(ART, key, '%s_Idle_%s.png' % (key, view))


def body_height():
    # The editor's Python has no image library, so trace the whole cell height: a little conservative, never short.
    return (spec.SOLE_ROW + 1) * spec.UU_PER_DOT


def import_sprites():
    # Idle frame 0 of every (character, direction) the shots use.
    sprites = {}
    for key, view in sorted({(k, v) for row in SHOTS.values() for _, k, v in row}):
        tex = import_texture(sprite_png(key, view), '%s/%s' % (DEST, key))
        sprites[(key, view)] = make_sprite(tex, '%s/%s' % (DEST, key), 'SPR_%s_Idle_%s' % (key, view))
    print('sprites', len(sprites), 'ppuu', round(spec.PIXELS_PER_UNREAL_UNIT, 5), 'uu per dot', round(spec.UU_PER_DOT, 4))
    return sprites


def camera_location(x):
    pitch = math.radians(18)
    return unreal.Vector(x, ROW_Y + spec.CAMERA_DISTANCE * math.cos(pitch), GROUND_Z + AIM_HEIGHT + spec.CAMERA_DISTANCE * math.sin(pitch))


def trace(world, a, b):
    hit = unreal.SystemLibrary.line_trace_single(world, a, b, unreal.TraceTypeQuery.ECC_VISIBILITY, True, [], unreal.DrawDebugTrace.NONE, True)
    if not hit:
        return None
    t = hit.to_tuple()
    return t[4], (t[9].get_actor_label() if t[9] else '?')


def check_placement():
    world = unreal.EditorLoadingAndSavingUtils.load_map(GEO)
    ok = True
    cam = camera_location(CAMERA_X)
    for shot, row in SHOTS.items():
        for x, key, view in row:
            ground = trace(world, unreal.Vector(x, ROW_Y, GROUND_Z + 500), unreal.Vector(x, ROW_Y, GROUND_Z - 500))
            gz = ground[0].z if ground else None
            height = body_height()
            blocked = [trace(world, cam, unreal.Vector(x, ROW_Y + 5, GROUND_Z + z)) for z in (5, height / 2, height - 5)]
            blocked = [b[1] for b in blocked if b]
            flat = gz is not None and abs(gz - GROUND_Z) < 2
            ok &= flat and not blocked
            print('place', shot, key, view, 'x', x, 'ground', round(gz, 1) if gz is not None else None, 'blocked', blocked)
    return ok


def remove_level():
    # delete_asset reports success on a map but leaves the file (see build_explore.py), so remove the file and rescan.
    f = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_content_dir() + LEVEL[len('/Game/'):] + '.umap')
    if os.path.exists(f):
        os.remove(f)
    unreal.AssetRegistryHelpers.get_asset_registry().scan_modified_asset_files([f])


def build_level(sprites):
    unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
    material = unreal.load_asset(SPRITE_MATERIAL)
    for shot, row in SHOTS.items():
        for x, key, view in row:
            actor = ACT.spawn_actor_from_class(unreal.PaperSpriteActor, unreal.Vector(x, ROW_Y, GROUND_Z))
            actor.set_actor_label('%s_%s_%s' % (key, view, shot))
            actor.tags = ['ArtSpec_sprite_' + shot]
            c = actor.get_component_by_class(unreal.PaperSpriteComponent)
            c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
            c.set_sprite(sprites[(key, view)])
            c.set_material(0, material)
            c.set_editor_property('lighting_channels', unreal.LightingChannels(channel0=True, channel1=True, channel2=False))
            c.set_collision_enabled(unreal.CollisionEnabled.NO_COLLISION)
    cam = ACT.spawn_actor_from_class(unreal.CameraActor, camera_location(CAMERA_X), unreal.Rotator(roll=0, pitch=-18, yaw=-90))
    cam.set_actor_label('Cam_ArtSpec')
    cam.tags = ['ArtSpec_camera']
    cc = cam.get_component_by_class(unreal.CameraComponent)
    cc.set_editor_property('field_of_view', spec.CAMERA_FOV)
    cc.set_editor_property('constrain_aspect_ratio', False)
    st = cc.get_editor_property('post_process_settings')
    st.set_editor_property('override_depth_of_field_focal_distance', True)
    st.set_editor_property('depth_of_field_focal_distance', spec.CAMERA_DISTANCE)
    cc.set_editor_property('post_process_settings', st)
    print('level saved', unreal.EditorLoadingAndSavingUtils.save_map(unreal.EditorLevelLibrary.get_editor_world(), LEVEL))


if __name__ == '__main__':
    log_to_file()
    try:
        remove_level()
        print('placement ok', check_placement())
        build_level(import_sprites())
    except Exception:
        import traceback
        print('ERR', traceback.format_exc())
    print('done')
