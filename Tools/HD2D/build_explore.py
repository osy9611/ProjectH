"""Rebuilds the HD-2D exploration maps and the hero camera values. Safe to rerun.

MAP_Village_Geo is a copy of Asian_Village_Demo without its lights, fog and post process.
MAP_Village_Day and MAP_Village_Night hold one preset each and stream Geo in,
so opening a map picks the time of day and the village exists only once.

Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file> -unattended -nullrhi
Result lines go to Client/Saved/HD2D_Explore/build.txt.
"""
import os, sys, unreal


def log_to_file(name):
    out = os.path.join(unreal.Paths.project_saved_dir(), 'HD2D_Explore')
    os.makedirs(out, exist_ok=True)
    sys.stdout = open(os.path.join(out, name), 'w', encoding='utf-8', buffering=1)
    sys.stderr = sys.stdout


EAL = unreal.EditorAssetLibrary
ACT = unreal.EditorLevelLibrary
MAPS = unreal.EditorLoadingAndSavingUtils

SOURCE = '/Game/Asian_Village/maps/Asian_Village_Demo'
GEO = '/Game/Maps/Village/MAP_Village_Geo'
ENVIRONMENT_CLASSES = ('DirectionalLight', 'SkyLight', 'ExponentialHeightFog', 'PostProcessVolume')
SPAWN = unreal.Vector(14110, 12070, 1400)

CAMERA_DISTANCE = 1500.0

# Values of the source map's volume. The sensor width sets the blur strength, so it
# moves to the hero camera together with the focus.
POST_PROCESS = dict(
    ambient_occlusion_fade_distance=50000.0, ambient_occlusion_intensity=0.3,
    ambient_occlusion_quality=13.0, ambient_occlusion_radius=60.0,
    auto_exposure_bias=1.263, auto_exposure_min_brightness=1.0, auto_exposure_max_brightness=1.0,
    bloom_intensity=0.8, vignette_intensity=0.35,
    color_contrast=unreal.Vector4(1, 1, 1, 1.1), color_gain=unreal.Vector4(1, 0.98, 1.19, 1),
    color_gain_midtones=unreal.Vector4(1, 1, 1.03, 1), color_saturation=unreal.Vector4(1.07, 1.07, 1.07, 1),
    dynamic_global_illumination_method=unreal.DynamicGlobalIlluminationMethod.LUMEN,
    film_shoulder=1.0, indirect_lighting_intensity=4.0,
)

PRESETS = {
    '/Game/Maps/Village/MAP_Village_Day': dict(
        sun_rotation=unreal.Rotator(roll=0, pitch=-40, yaw=-50), sun_intensity=6.0,
        sun_color=unreal.Color(r=255, g=240, b=215), sky_intensity=1.0, fill_lights=[]),
    # The camera looks toward -Y, so a fill light must sit on the +Y side to reach the sprite's face.
    '/Game/Maps/Village/MAP_Village_Night': dict(
        sun_rotation=unreal.Rotator(roll=0, pitch=-35, yaw=-50), sun_intensity=1.6,
        sun_color=unreal.Color(r=150, g=190, b=255), sky_intensity=0.25,
        fill_lights=[unreal.Vector(SPAWN.x - 350, SPAWN.y + 350, 1300), unreal.Vector(SPAWN.x + 350, SPAWN.y + 350, 1300)]),
}


def set_camera_mode():
    cm = unreal.load_asset('/Game/Blueprints/Characters/Camera/CM_Player')
    cdo = unreal.get_default_object(cm.generated_class())
    cdo.set_editor_property('field_of_view', 35.0)
    cdo.set_editor_property('camera_rotation', unreal.Rotator(roll=0, pitch=-18, yaw=-90))
    cdo.set_editor_property('camera_distance', CAMERA_DISTANCE)
    print('CM_Player saved', EAL.save_loaded_asset(cm, False))


def set_hero_focus():
    bp = unreal.load_asset('/Game/Blueprints/Characters/BP_Hero')
    subs = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    lib = unreal.SubobjectDataBlueprintFunctionLibrary
    for h in subs.k2_gather_subobject_data_for_blueprint(bp):
        o = lib.get_object(lib.get_data(h))
        if isinstance(o, unreal.CameraComponent):
            st = o.get_editor_property('post_process_settings')
            for k, v in dict(depth_of_field_focal_distance=CAMERA_DISTANCE, depth_of_field_fstop=2.8, depth_of_field_sensor_width=600.0).items():
                st.set_editor_property('override_' + k, True)
                st.set_editor_property(k, v)
            o.set_editor_property('post_process_settings', st)
        if isinstance(o, unreal.BoxComponent):
            o.set_editor_property('hidden_in_game', True)
    print('BP_Hero saved', EAL.save_loaded_asset(bp, False))


def mirror_hero_right_slots():
    # PaperZD rewrites the sprite's X scale every frame from these flags, which overrides
    # UHDPawnExtensionComponent::SetFlipX. Slots run clockwise from up; 1 is right and reuses the left art.
    for name in ('Run', 'Idle'):
        seq = unreal.load_asset('/Game/Blueprints/Characters/Primrose/AnimSequences/' + name)
        data = seq.get_editor_property('anim_data')
        right = data[1]
        right.set_editor_property('mirror_mode', unreal.PaperZDFlipbookMirrorMode.HORIZONTAL)
        data[1] = right
        seq.set_editor_property('anim_data', data)
        print(name, 'saved', EAL.save_loaded_asset(seq, False))


def build_geo():
    for path in [GEO] + list(PRESETS):
        if EAL.does_asset_exist(path):
            print('deleted', path, EAL.delete_asset(path))
    print('duplicated', EAL.duplicate_asset(SOURCE, GEO))
    world = MAPS.load_map(GEO)
    npcs = []
    for a in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        name = a.get_class().get_name()
        if name == 'BP_NPC_C':
            # NPCs belong to exploration; the battle map streams Geo too and must not show them.
            npcs.append((a.get_class(), a.get_actor_location(), a.get_actor_rotation(), a.get_component_by_class(unreal.HDNPCComponent).get_editor_property('table_id')))
        if name in ENVIRONMENT_CLASSES or name == 'BP_NPC_C':
            ACT.destroy_actor(a)
    print('Geo saved', MAPS.save_map(world, GEO))
    return npcs


def add_environment(world, preset, anchor):
    world.get_world_settings().set_editor_property('force_no_precomputed_lighting', True)

    sun = ACT.spawn_actor_from_class(unreal.DirectionalLight, anchor, preset['sun_rotation'])
    c = sun.get_component_by_class(unreal.DirectionalLightComponent)
    c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity', preset['sun_intensity'])
    c.set_editor_property('light_color', preset['sun_color'])

    sky = ACT.spawn_actor_from_class(unreal.SkyLight, anchor)
    c = sky.get_component_by_class(unreal.SkyLightComponent)
    c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity', preset['sky_intensity'])
    c.set_editor_property('light_color', unreal.Color(r=255, g=251, b=223))

    fog = ACT.spawn_actor_from_class(unreal.ExponentialHeightFog, anchor)
    c = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    c.set_volumetric_fog(True)
    for k, v in dict(
        fog_density=0.06, fog_height_falloff=0.2, volumetric_fog_extinction_scale=1.0,
        fog_inscattering_luminance=unreal.LinearColor(0.337, 0.397, 1.0, 1.0),
        directional_inscattering_luminance=unreal.LinearColor(0.25, 0.25, 0.125, 1.0),
        volumetric_fog_albedo=unreal.Color(r=255, g=183, b=142),
        volumetric_fog_scattering_distribution=0.9, volumetric_fog_distance=6000.0,
    ).items():
        c.set_editor_property(k, v)

    pp = ACT.spawn_actor_from_class(unreal.PostProcessVolume, anchor)
    pp.set_editor_property('unbound', True)
    st = pp.get_editor_property('settings')
    for k, v in POST_PROCESS.items():
        st.set_editor_property('override_' + k, True)
        st.set_editor_property(k, v)
    pp.set_editor_property('settings', st)

    for location in preset['fill_lights']:
        light = ACT.spawn_actor_from_class(unreal.PointLight, location)
        c = light.get_component_by_class(unreal.PointLightComponent)
        c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
        c.set_editor_property('intensity', 60.0)
        c.set_editor_property('attenuation_radius', 900.0)
        c.set_editor_property('light_color', unreal.Color(r=255, g=190, b=120))
        c.set_editor_property('volumetric_scattering_intensity', 0.2)

def build_preset(path, preset, npcs):
    world = MAPS.new_blank_map(False)
    add_environment(world, preset, SPAWN)
    for cls, location, rotation, table_id in npcs:
        ACT.spawn_actor_from_class(cls, location, rotation).get_component_by_class(unreal.HDNPCComponent).set_editor_property('table_id', table_id)
    print('level', unreal.EditorLevelUtils.add_level_to_world(world, GEO, unreal.LevelStreamingAlwaysLoaded))
    print('saved', path, MAPS.save_map(world, path))


if __name__ == '__main__':
    log_to_file('build.txt')
    set_camera_mode()
    set_hero_focus()
    mirror_hero_right_slots()
    npcs = build_geo()
    for path, preset in PRESETS.items():
        build_preset(path, preset, npcs)
    print('done')
