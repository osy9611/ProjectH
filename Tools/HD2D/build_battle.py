"""Applies HD-2D to the battle: Village environment, shadows, camera modes and focus. Safe to rerun.

BattleScene stays in place because its level sequences animate PlayerSpawn1-4 by binding,
so the Village geometry moves under the fixed battle coordinates instead.
The Experience, pawn data and CM_Battle link already exist in DA_BattleCharacterPawnData.

Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file> -unattended -nullrhi
Set HD2D_BATTLE_CAMERA to pick an entry of CAMERAS. Result lines go to Client/Saved/HD2D_Explore/build_battle.txt.
"""
import os, sys, unreal

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from build_explore import ACT, EAL, GEO, MAPS, PRESETS, add_environment, log_to_file, set_camera_focus, set_sprite_material

log_to_file('build_battle.txt')

BATTLE_MAP = '/BattleGame/Maps/BattleScene'
DAY = '/Game/Maps/Village/MAP_Village_Day'
# Battle (0, 1060, 0) lands on the Village plaza at (14110, 12070, 1002), with the stairs as a backdrop.
GEO_OFFSET = unreal.Vector(-14110, -11010, -1002)
FOG_ANCHOR = unreal.Vector(0, 1060, 398)
REPLACED_CLASSES = ('DirectionalLight', 'SkyLight', 'SkyAtmosphere', 'VolumetricCloud', 'ExponentialHeightFog', 'BP_HD2DEnvironment_C')
REPLACED_LABELS = ('SM_SkySphere',)
GROUND_LABEL = 'Plane'

CAMERAS = dict(
    current=dict(pitch=-5.0, noise=True, battle_distance=5500.0, skill_distance=5000.0),
    explore=dict(pitch=-18.0, noise=False, battle_distance=5500.0, skill_distance=5000.0),
)
camera = CAMERAS[os.environ.get('HD2D_BATTLE_CAMERA', 'explore')]
SPRITES = ('/Game/Blueprints/Battle/BP_BattleActor', '/Game/Blueprints/Battle/AI/BP_BattleActor_AI')


def components(path):
    bp = unreal.load_asset(path)
    subs = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    lib = unreal.SubobjectDataBlueprintFunctionLibrary
    return bp, [lib.get_object(lib.get_data(h)) for h in subs.k2_gather_subobject_data_for_blueprint(bp)]


def set_sprite_shadows():
    for path in SPRITES:
        bp, comps = components(path)
        for o in comps:
            if isinstance(o, unreal.PaperFlipbookComponent):
                o.set_editor_property('cast_shadow', True)
        print(path, 'saved', EAL.save_loaded_asset(bp, False))


def set_camera_modes():
    for path, distance in (('CM_Battle', camera['battle_distance']), ('CM_BattleSkiil', camera['skill_distance'])):
        cm = unreal.load_asset('/Game/Blueprints/Characters/Camera/' + path)
        cdo = unreal.get_default_object(cm.generated_class())
        cdo.set_editor_property('camera_rotation', unreal.Rotator(roll=0, pitch=camera['pitch'], yaw=-90))
        cdo.set_editor_property('camera_distance', distance)
        cdo.set_editor_property('use_camera_noise', camera['noise'])
        print(path, 'saved', EAL.save_loaded_asset(cm, False))


def set_observer_focus():
    bp, comps = components('/Game/Blueprints/Battle/BP_BattleObserver')
    for o in comps:
        if isinstance(o, unreal.CameraComponent):
            set_camera_focus(o, camera['battle_distance'])
    print('BP_BattleObserver saved', EAL.save_loaded_asset(bp, False))


def scan_scene_data_with_plugin():
    # DefaultGame.ini must not name /BattleGame paths: the asset manager records them as scanned
    # before the plugin mounts and then skips this asset's own scan of the same paths.
    gfd = unreal.load_asset('/BattleGame/BattleGame')
    types = gfd.get_editor_property('primary_asset_types_to_scan')
    if not any('"HDSceneData"' in t.export_text() for t in types):
        t = unreal.PrimaryAssetTypeInfo()
        t.import_text('(PrimaryAssetType="HDSceneData",AssetBaseClass="/Script/ProjectH.HDSceneData",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/BattleGame/Experiences")),Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=Unknown))')
        types.append(t)
        gfd.set_editor_property('primary_asset_types_to_scan', types)
    print('BattleGame saved', EAL.save_loaded_asset(gfd, False), [t.export_text()[:45] for t in types])


def build_map():
    world = MAPS.load_map(BATTLE_MAP)
    for a in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        if a.get_class().get_name() in REPLACED_CLASSES or a.get_actor_label() in REPLACED_LABELS:
            ACT.destroy_actor(a)
        elif a.get_actor_label() == GROUND_LABEL:
            # Battle pawns stand on this plane, so only its visuals go away.
            a.set_actor_hidden_in_game(True)
        elif a.get_class().get_name() == 'PostProcessVolume':
            ACT.destroy_actor(a)
    add_environment(world, PRESETS[DAY], FOG_ANCHOR)
    if not unreal.GameplayStatics.get_streaming_level(world, 'MAP_Village_Geo'):
        print('level', unreal.EditorLevelUtils.add_level_to_world_with_transform(world, GEO, unreal.LevelStreamingAlwaysLoaded, unreal.Transform(location=GEO_OFFSET)))
    print('saved', MAPS.save_map(world, BATTLE_MAP))


scan_scene_data_with_plugin()
set_sprite_shadows()
set_sprite_material(SPRITES)
set_camera_modes()
set_observer_focus()
build_map()
print('done', os.environ.get('HD2D_BATTLE_CAMERA', 'explore'))
