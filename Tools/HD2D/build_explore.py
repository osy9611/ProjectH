"""Rebuilds the HD-2D exploration maps and the hero camera values. Safe to rerun.

MAP_Village_Geo is a copy of Asian_Village_Demo without its lights, fog and post process.
MAP_Village_Day and MAP_Village_Night hold one preset each and stream Geo in,
so opening a map picks the time of day and the village exists only once.
Values shared by every map live in /Game/HD2D: BP_HD2DEnvironment (post process, DoF strength, fog)
and M_HD2D_SpriteLift (sprite lift and backlight). The presets below hold only what differs by time of day.

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

STYLE = '/Game/HD2D'
ENVIRONMENT = STYLE + '/BP_HD2DEnvironment'
SPRITE_MATERIAL = STYLE + '/M_HD2D_SpriteLift'
CAMERA_DISTANCE = 2250.0

# Shared by every map through BP_HD2DEnvironment. Cameras override only the focal distance,
# so the f-stop and sensor width here set the blur strength everywhere.
POST_PROCESS = dict(
    ambient_occlusion_fade_distance=50000.0, ambient_occlusion_intensity=0.3,
    ambient_occlusion_quality=13.0, ambient_occlusion_radius=60.0,
    auto_exposure_bias=1.263, auto_exposure_min_brightness=1.0, auto_exposure_max_brightness=1.0,
    bloom_intensity=0.4, vignette_intensity=0.15,
    color_contrast=unreal.Vector4(1, 1, 1, 1.1), color_gain=unreal.Vector4(1, 0.98, 1.19, 1),
    color_gain_midtones=unreal.Vector4(1, 1, 1.03, 1), color_saturation=unreal.Vector4(1.03, 1.03, 1.03, 1),
    depth_of_field_fstop=5.6, depth_of_field_sensor_width=600.0,
    dynamic_global_illumination_method=unreal.DynamicGlobalIlluminationMethod.LUMEN,
    film_shoulder=1.0, indirect_lighting_intensity=4.0,
)
FOG = dict(
    fog_density=0.03, fog_height_falloff=0.2, volumetric_fog_extinction_scale=1.0,
    fog_inscattering_luminance=unreal.LinearColor(0.337, 0.397, 1.0, 1.0),
    directional_inscattering_luminance=unreal.LinearColor(0.25, 0.25, 0.125, 1.0),
    volumetric_fog_albedo=unreal.Color(r=255, g=183, b=142),
    volumetric_fog_scattering_distribution=0.9, volumetric_fog_distance=6000.0,
)

PRESETS = {
    '/Game/Maps/Village/MAP_Village_Day': dict(
        sun_rotation=unreal.Rotator(roll=0, pitch=-40, yaw=-50), sun_intensity=6.0,
        sun_color=unreal.Color(r=255, g=240, b=215), sky_intensity=1.0, fill_lights=[], character_light=None),
    # Lantern pools around the start; they skip volumetric scattering, which drew them as floating orbs.
    '/Game/Maps/Village/MAP_Village_Night': dict(
        sun_rotation=unreal.Rotator(roll=0, pitch=-35, yaw=-50), sun_intensity=0.8,
        sun_color=unreal.Color(r=120, g=160, b=255), sky_intensity=0.15,
        fill_lights=[unreal.Vector(SPAWN.x - 350, SPAWN.y + 350, 1300), unreal.Vector(SPAWN.x + 350, SPAWN.y + 350, 1300)],
        # Lights only lighting channel 1, which only the character sprites join, so they stay readable outside the lanterns.
        # It shines from the camera side (the camera looks down -Y) and 39 degrees above.
        character_light=dict(rotation=unreal.Rotator(roll=0, pitch=-39, yaw=-90), intensity=3.0, color=unreal.Color(r=170, g=195, b=255))),
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
            set_camera_focus(o, CAMERA_DISTANCE)
        if isinstance(o, unreal.BoxComponent):
            o.set_editor_property('hidden_in_game', True)
    print('BP_Hero saved', EAL.save_loaded_asset(bp, False))


def set_camera_focus(camera, distance):
    st = camera.get_editor_property('post_process_settings')
    st.set_editor_property('override_depth_of_field_focal_distance', True)
    st.set_editor_property('depth_of_field_focal_distance', distance)
    st.set_editor_property('override_depth_of_field_fstop', False)
    st.set_editor_property('override_depth_of_field_sensor_width', False)
    camera.set_editor_property('post_process_settings', st)


def build_sprite_material():
    # A masked lit sprite whose emissive adds a fraction of the art, so a sprite in shadow keeps its colors.
    # Two Sided Foliage lets a sun behind the sprite through as Subsurface, so a backlit sprite does not turn muddy.
    mel = unreal.MaterialEditingLibrary
    if EAL.does_asset_exist(SPRITE_MATERIAL):
        mat = unreal.load_asset(SPRITE_MATERIAL)
        mel.delete_all_material_expressions(mat)
    else:
        mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(SPRITE_MATERIAL.rsplit('/', 1)[1], STYLE, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property('blend_mode', unreal.BlendMode.BLEND_MASKED)
    mat.set_editor_property('two_sided', True)
    tex = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D)
    tex.set_editor_property('parameter_name', 'SpriteTexture')
    tex.set_editor_property('texture', unreal.load_asset('/Paper2D/DummySpriteTexture'))
    vertex = mel.create_material_expression(mat, unreal.MaterialExpressionVertexColor)
    color = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply)
    mel.connect_material_expressions(tex, 'RGB', color, 'A')
    mel.connect_material_expressions(vertex, 'RGB', color, 'B')
    lift = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter)
    lift.set_editor_property('parameter_name', 'Lift')
    lift.set_editor_property('default_value', 0.25)
    emissive = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply)
    mel.connect_material_expressions(color, '', emissive, 'A')
    mel.connect_material_expressions(lift, '', emissive, 'B')
    mat.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_TWO_SIDED_FOLIAGE)
    transmission = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter)
    transmission.set_editor_property('parameter_name', 'Transmission')
    transmission.set_editor_property('default_value', 0.8)
    subsurface = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply)
    mel.connect_material_expressions(color, '', subsurface, 'A')
    mel.connect_material_expressions(transmission, '', subsurface, 'B')
    mask = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply)
    mel.connect_material_expressions(tex, 'A', mask, 'A')
    mel.connect_material_expressions(vertex, 'A', mask, 'B')
    mel.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(emissive, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.connect_material_property(subsurface, '', unreal.MaterialProperty.MP_SUBSURFACE_COLOR)
    mel.connect_material_property(mask, '', unreal.MaterialProperty.MP_OPACITY_MASK)
    mel.recompile_material(mat)
    print('sprite material saved', EAL.save_loaded_asset(mat, False))


def set_sprite_material(paths):
    material = unreal.load_asset(SPRITE_MATERIAL)
    for path in paths:
        bp = unreal.load_asset(path)
        subs = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
        lib = unreal.SubobjectDataBlueprintFunctionLibrary
        for h in subs.k2_gather_subobject_data_for_blueprint(bp):
            o = lib.get_object(lib.get_data(h))
            if isinstance(o, unreal.PaperFlipbookComponent):
                o.set_material(0, material)
                # Channel 1 is for character_light; maps without one light the sprite through channel 0 as before.
                o.set_editor_property('lighting_channels', unreal.LightingChannels(channel0=True, channel1=True, channel2=False))
        print(path, 'saved', EAL.save_loaded_asset(bp, False))


def set_texture_filter():
    # The village textures ship as TF_Nearest; TF_Default hands filtering to the World texture group.
    changed = 0
    for data in unreal.AssetRegistryHelpers.get_asset_registry().get_assets_by_path('/Game/Asian_Village/textures', recursive=True):
        t = data.get_asset()
        if isinstance(t, unreal.Texture2D) and t.get_editor_property('filter') != unreal.TextureFilter.TF_DEFAULT:
            t.set_editor_property('filter', unreal.TextureFilter.TF_DEFAULT)
            EAL.save_loaded_asset(t, False)
            changed += 1
    print('textures changed', changed)


def build_environment_blueprint():
    if not EAL.does_asset_exist(ENVIRONMENT):
        factory = unreal.BlueprintFactory()
        factory.set_editor_property('parent_class', unreal.Actor)
        unreal.AssetToolsHelpers.get_asset_tools().create_asset(ENVIRONMENT.rsplit('/', 1)[1], STYLE, unreal.Blueprint, factory)
    bp = unreal.load_asset(ENVIRONMENT)
    subs = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    lib = unreal.SubobjectDataBlueprintFunctionLibrary
    handles = subs.k2_gather_subobject_data_for_blueprint(bp)
    found = {type(lib.get_object(lib.get_data(h))) for h in handles}
    for cls in (unreal.PostProcessComponent, unreal.ExponentialHeightFogComponent):
        if cls not in found:
            subs.add_new_subobject(unreal.AddNewSubobjectParams(parent_handle=handles[0], new_class=cls, blueprint_context=bp))
    for h in subs.k2_gather_subobject_data_for_blueprint(bp):
        o = lib.get_object(lib.get_data(h))
        if isinstance(o, unreal.PostProcessComponent):
            o.set_editor_property('unbound', True)
            st = o.get_editor_property('settings')
            for k, v in POST_PROCESS.items():
                st.set_editor_property('override_' + k, True)
                st.set_editor_property(k, v)
            o.set_editor_property('settings', st)
        if isinstance(o, unreal.ExponentialHeightFogComponent):
            o.set_volumetric_fog(True)
            for k, v in FOG.items():
                o.set_editor_property(k, v)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print('environment saved', EAL.save_loaded_asset(bp, False))


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

    ACT.spawn_actor_from_class(unreal.load_asset(ENVIRONMENT).generated_class(), anchor)

    character = preset['character_light']
    if character:
        light = ACT.spawn_actor_from_class(unreal.DirectionalLight, anchor, character['rotation'])
        c = light.get_component_by_class(unreal.DirectionalLightComponent)
        c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
        c.set_editor_property('intensity', character['intensity'])
        c.set_editor_property('light_color', character['color'])
        c.set_editor_property('cast_shadows', False)
        c.set_editor_property('atmosphere_sun_light', False)
        c.set_editor_property('volumetric_scattering_intensity', 0.0)
        c.set_editor_property('lighting_channels', unreal.LightingChannels(channel0=False, channel1=True, channel2=False))

    for location in preset['fill_lights']:
        light = ACT.spawn_actor_from_class(unreal.PointLight, location)
        c = light.get_component_by_class(unreal.PointLightComponent)
        c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
        c.set_editor_property('intensity', 180.0)
        c.set_editor_property('attenuation_radius', 900.0)
        c.set_editor_property('light_color', unreal.Color(r=255, g=170, b=90))
        c.set_editor_property('volumetric_scattering_intensity', 0.0)


def build_preset(path, preset, npcs):
    world = MAPS.new_blank_map(False)
    add_environment(world, preset, SPAWN)
    for cls, location, rotation, table_id in npcs:
        ACT.spawn_actor_from_class(cls, location, rotation).get_component_by_class(unreal.HDNPCComponent).set_editor_property('table_id', table_id)
    print('level', unreal.EditorLevelUtils.add_level_to_world(world, GEO, unreal.LevelStreamingAlwaysLoaded))
    print('saved', path, MAPS.save_map(world, path))


if __name__ == '__main__':
    log_to_file('build.txt')
    set_texture_filter()
    build_environment_blueprint()
    build_sprite_material()
    set_sprite_material(['/Game/Blueprints/Characters/BP_Hero', '/Game/Blueprints/Characters/BP_NPC'])
    set_camera_mode()
    set_hero_focus()
    mirror_hero_right_slots()
    npcs = build_geo()
    for path, preset in PRESETS.items():
        build_preset(path, preset, npcs)
    print('done')
