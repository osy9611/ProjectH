"""Builds the lighting study assets under /Game/HD2D/Lab. Safe to rerun.

Material copies of M_HD2D_SpriteLift rebuild the same graph plus one change; the shared material is never touched.
Map copies of the day and night presets add one hidden point light tagged LabCompanion,
because a -game Python script cannot spawn actors; light_probe.py attaches it to the hero when a candidate needs it.
With HD2D_NORMAL_ART set to the art track's folder, the normal-map test also imports its 64x64 sprites into
/Game/HD2D/Lab/Normal, builds one sprite per pose and normal texture, and places two movable sprite actors
and a camera that light_probe.py arranges at runtime.
Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file> -unattended
Result lines go to Client/Saved/HD2D_Explore/build_light_lab.txt.
"""
import os, sys, unreal

EAL = unreal.EditorAssetLibrary
MEL = unreal.MaterialEditingLibrary
LAB = '/Game/HD2D/Lab'
NORMAL = LAB + '/Normal'
ART = os.environ.get('HD2D_NORMAL_ART', '')
POSES = ('Front', '3Q')
# Art spec draft: 0.2255 pixels per unit, feet at y62 on a 64x64 canvas.
PIXELS_PER_UNIT = 0.2255
PIVOT = unreal.Vector2D(32, 62)

# UV of the neighbour Width screen pixels away. Paper2D binds the sprite only to TextureSampleParameter2D nodes
# named SpriteTexture, so the edge test samples through four of them instead of reading texture dimensions.
NEIGHBOUR_CODE = 'return UV + Dir * (abs(ddx(UV)) + abs(ddy(UV))) * Width;'


def log_to_file(name):
    out = os.path.join(unreal.Paths.project_saved_dir(), 'HD2D_Explore')
    os.makedirs(out, exist_ok=True)
    sys.stdout = open(os.path.join(out, name), 'w', encoding='utf-8', buffering=1)
    sys.stderr = sys.stdout


def scalar(mat, name, value):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionScalarParameter)
    e.set_editor_property('parameter_name', name)
    e.set_editor_property('default_value', value)
    return e


def vector(mat, name, value):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionVectorParameter)
    e.set_editor_property('parameter_name', name)
    e.set_editor_property('default_value', value)
    return e


def custom(mat, code, output, names):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionCustom)
    e.set_editor_property('code', code)
    e.set_editor_property('output_type', output)
    inputs = []
    for n in names:
        i = unreal.CustomInput()
        i.set_editor_property('input_name', n)
        inputs.append(i)
    e.set_editor_property('inputs', inputs)
    return e


def sprite_sample(mat, uv=None):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D)
    e.set_editor_property('parameter_name', 'SpriteTexture')
    e.set_editor_property('texture', unreal.load_asset('/Paper2D/DummySpriteTexture'))
    if uv:
        MEL.connect_material_expressions(uv, '', e, 'UVs')
    return e


def binary(mat, cls, a, b, a_out='', b_out=''):
    e = MEL.create_material_expression(mat, cls)
    MEL.connect_material_expressions(a, a_out, e, 'A')
    MEL.connect_material_expressions(b, b_out, e, 'B')
    return e


def build(name, foliage=False, normal_up=None, rim=False):
    path = LAB + '/' + name
    if EAL.does_asset_exist(path):
        mat = unreal.load_asset(path)
        MEL.delete_all_material_expressions(mat)
    else:
        mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, LAB, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property('blend_mode', unreal.BlendMode.BLEND_MASKED)
    mat.set_editor_property('two_sided', True)

    # Same graph as build_explore.build_sprite_material.
    tex = sprite_sample(mat)
    vertex = MEL.create_material_expression(mat, unreal.MaterialExpressionVertexColor)
    color = binary(mat, unreal.MaterialExpressionMultiply, tex, vertex, 'RGB', 'RGB')
    emissive = binary(mat, unreal.MaterialExpressionMultiply, color, scalar(mat, 'Lift', 0.25))
    mask = binary(mat, unreal.MaterialExpressionMultiply, tex, vertex, 'A', 'A')

    if foliage:
        mat.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_TWO_SIDED_FOLIAGE)
        sss = binary(mat, unreal.MaterialExpressionMultiply, color, scalar(mat, 'Transmission', 0.4))
        MEL.connect_material_property(sss, '', unreal.MaterialProperty.MP_SUBSURFACE_COLOR)

    if normal_up is not None:
        # World-space normal between "towards the camera" and "up", so the sun's side no longer decides the face.
        mat.set_editor_property('tangent_space_normal', False)
        cam = MEL.create_material_expression(mat, unreal.MaterialExpressionCameraVectorWS)
        up = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector)
        up.set_editor_property('constant', unreal.LinearColor(0, 0, 1, 0))
        blend = MEL.create_material_expression(mat, unreal.MaterialExpressionLinearInterpolate)
        MEL.connect_material_expressions(cam, '', blend, 'A')
        MEL.connect_material_expressions(up, '', blend, 'B')
        MEL.connect_material_expressions(scalar(mat, 'NormalUp', normal_up), '', blend, 'Alpha')
        normal = MEL.create_material_expression(mat, unreal.MaterialExpressionNormalize)
        MEL.connect_material_expressions(blend, '', normal, 'VectorInput')
        MEL.connect_material_property(normal, '', unreal.MaterialProperty.MP_NORMAL)

    if rim:
        # The sun direction and colour come in as parameters; a shipped version would read them from an MPC.
        # Alpha minus the smallest neighbour alpha, so only the outermost opaque pixels light up.
        uv = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate)
        width = scalar(mat, 'RimWidth', 3.0)
        alphas = []
        for d in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            n = custom(mat, NEIGHBOUR_CODE, unreal.CustomMaterialOutputType.CMOT_FLOAT2, ('UV', 'Dir', 'Width'))
            direction = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector)
            direction.set_editor_property('r', d[0])
            direction.set_editor_property('g', d[1])
            MEL.connect_material_expressions(uv, '', n, 'UV')
            MEL.connect_material_expressions(direction, '', n, 'Dir')
            MEL.connect_material_expressions(width, '', n, 'Width')
            alphas.append(sprite_sample(mat, n))
        smallest = binary(mat, unreal.MaterialExpressionMin,
                          binary(mat, unreal.MaterialExpressionMin, alphas[0], alphas[1], 'A', 'A'),
                          binary(mat, unreal.MaterialExpressionMin, alphas[2], alphas[3], 'A', 'A'))
        edge = MEL.create_material_expression(mat, unreal.MaterialExpressionSaturate)
        MEL.connect_material_expressions(binary(mat, unreal.MaterialExpressionSubtract, tex, smallest, 'A', ''), '', edge, '')
        facing = MEL.create_material_expression(mat, unreal.MaterialExpressionDotProduct)
        MEL.connect_material_expressions(vector(mat, 'RimDir', unreal.LinearColor(0, 1, 0, 0)), 'RGB', facing, 'A')
        MEL.connect_material_expressions(MEL.create_material_expression(mat, unreal.MaterialExpressionCameraVectorWS), '', facing, 'B')
        back = MEL.create_material_expression(mat, unreal.MaterialExpressionSaturate)
        MEL.connect_material_expressions(facing, '', back, '')
        amount = binary(mat, unreal.MaterialExpressionMultiply, edge, back)
        light = binary(mat, unreal.MaterialExpressionMultiply, amount, vector(mat, 'RimColor', unreal.LinearColor(1, 1, 1, 1)), '', 'RGB')
        emissive = binary(mat, unreal.MaterialExpressionAdd, emissive, light)

    MEL.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR)
    MEL.connect_material_property(emissive, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    MEL.connect_material_property(mask, '', unreal.MaterialProperty.MP_OPACITY_MASK)
    MEL.recompile_material(mat)
    print(name, 'saved', EAL.save_loaded_asset(mat, False))


def import_texture(png, name, normal=None):
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', png)
    task.set_editor_property('destination_path', NORMAL)
    task.set_editor_property('destination_name', name)
    task.set_editor_property('replace_existing', True)
    task.set_editor_property('automated', True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    t = unreal.load_asset(NORMAL + '/' + name)
    t.set_editor_property('filter', unreal.TextureFilter.TF_NEAREST)
    t.set_editor_property('mip_gen_settings', unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
    t.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_PIXELS2D)
    t.set_editor_property('never_stream', True)
    if normal is None:
        t.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_EDITOR_ICON)
    else:
        t.set_editor_property('srgb', False)
        t.set_editor_property('compression_settings', normal)
    print(name, 'saved', EAL.save_loaded_asset(t, False))
    return t


def build_sprite(name, texture, extra=None):
    path = NORMAL + '/' + name
    if EAL.does_asset_exist(path):
        EAL.delete_asset(path)
    sprite = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, NORMAL, unreal.PaperSprite, unreal.PaperSpriteFactory())
    sprite.set_editor_property('source_dimension', unreal.Vector2D(texture.blueprint_get_size_x(), texture.blueprint_get_size_y()))
    sprite.set_editor_property('source_texture', texture)
    sprite.set_editor_property('pixels_per_unreal_unit', PIXELS_PER_UNIT)
    sprite.set_editor_property('pivot_mode', unreal.SpritePivotMode.CUSTOM)
    sprite.set_editor_property('custom_pivot_point', PIVOT)
    if extra:
        sprite.set_editor_property('additional_source_textures', [extra])
    print(name, 'saved', EAL.save_loaded_asset(sprite, False))


def build_normal_assets():
    for pose in POSES:
        color = import_texture(os.path.join(ART, 'T_ConceptGirl_%s.png' % pose), 'T_%s' % pose)
        png = os.path.join(ART, 'T_ConceptGirl_%s_N.png' % pose)
        bc5 = import_texture(png, 'T_%s_N' % pose, unreal.TextureCompressionSettings.TC_NORMALMAP)
        raw = import_texture(png, 'T_%s_NRaw' % pose, unreal.TextureCompressionSettings.TC_VECTOR_DISPLACEMENTMAP)
        build_sprite('S_%s' % pose, color)
        build_sprite('S_%s_N' % pose, color, bc5)
        build_sprite('S_%s_NRaw' % pose, color, raw)


def build_normal_material(name, source):
    """The shared sprite graph plus a tangent-space normal. source: 'edge' (B), 'bc5' or 'raw' (C)."""
    path = LAB + '/' + name
    if EAL.does_asset_exist(path):
        mat = unreal.load_asset(path)
        MEL.delete_all_material_expressions(mat)
    else:
        mat = unreal.AssetToolsHelpers.get_asset_tools().create_asset(name, LAB, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property('blend_mode', unreal.BlendMode.BLEND_MASKED)
    mat.set_editor_property('two_sided', True)
    mat.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_TWO_SIDED_FOLIAGE)
    tex = sprite_sample(mat)
    vertex = MEL.create_material_expression(mat, unreal.MaterialExpressionVertexColor)
    color = binary(mat, unreal.MaterialExpressionMultiply, tex, vertex, 'RGB', 'RGB')
    emissive = binary(mat, unreal.MaterialExpressionMultiply, color, scalar(mat, 'Lift', 0.25))
    mask = binary(mat, unreal.MaterialExpressionMultiply, tex, vertex, 'A', 'A')
    sss = binary(mat, unreal.MaterialExpressionMultiply, color, scalar(mat, 'Transmission', 0.8))

    if source == 'edge':
        # Alpha gradient over EdgeWidth screen pixels: the silhouette tilts outward, the inside stays flat.
        uv = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate)
        width = scalar(mat, 'EdgeWidth', 18.0)
        alphas = []
        for d in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            n = custom(mat, NEIGHBOUR_CODE, unreal.CustomMaterialOutputType.CMOT_FLOAT2, ('UV', 'Dir', 'Width'))
            direction = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector)
            direction.set_editor_property('r', d[0])
            direction.set_editor_property('g', d[1])
            MEL.connect_material_expressions(uv, '', n, 'UV')
            MEL.connect_material_expressions(direction, '', n, 'Dir')
            MEL.connect_material_expressions(width, '', n, 'Width')
            alphas.append(sprite_sample(mat, n))
        # Same axes as the PixelLab maps: +x right, +y down the texture.
        normal = custom(mat, 'return float3(L - R, U - D, Z);', unreal.CustomMaterialOutputType.CMOT_FLOAT3, ('R', 'L', 'D', 'U', 'Z'))
        for pin, a in zip(('R', 'L', 'D', 'U'), alphas):
            MEL.connect_material_expressions(a, 'A', normal, pin)
        MEL.connect_material_expressions(scalar(mat, 'EdgeFlat', 1.0), '', normal, 'Z')
        out = ''
    else:
        normal = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D)
        # Paper2D binds a sprite's first Additional Source Texture to this name.
        normal.set_editor_property('parameter_name', 'SpriteAdditionalTexture_0')
        normal.set_editor_property('texture', unreal.load_asset(NORMAL + ('/T_Front_N' if source == 'bc5' else '/T_Front_NRaw')))
        normal.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL if source == 'bc5' else unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
        out = 'RGB'

    # Raw maps are stored 0..1; NormalStrength blends towards the flat sprite normal; FlipY covers a tangent frame running up.
    code = 'float3 n = %s; n.y *= F; return normalize(lerp(float3(0, 0, 1), normalize(n), S));' % ('N * 2 - 1' if source == 'raw' else 'N')
    final = custom(mat, code, unreal.CustomMaterialOutputType.CMOT_FLOAT3, ('N', 'F', 'S'))
    MEL.connect_material_expressions(normal, out, final, 'N')
    MEL.connect_material_expressions(scalar(mat, 'FlipY', 1.0), '', final, 'F')
    MEL.connect_material_expressions(scalar(mat, 'NormalStrength', 1.0), '', final, 'S')

    MEL.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR)
    MEL.connect_material_property(emissive, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    MEL.connect_material_property(mask, '', unreal.MaterialProperty.MP_OPACITY_MASK)
    MEL.connect_material_property(sss, '', unreal.MaterialProperty.MP_SUBSURFACE_COLOR)
    MEL.connect_material_property(final, '', unreal.MaterialProperty.MP_NORMAL)
    MEL.recompile_material(mat)
    stats = MEL.get_statistics(mat)
    print(name, 'saved', EAL.save_loaded_asset(mat, False), 'ps', stats.num_pixel_shader_instructions,
          'samplers', stats.num_samplers, 'texture lookups', stats.num_pixel_texture_samples)


def place_normal_test():
    # Positions, rotation and the camera transform are set at runtime by light_probe.py.
    for pose in POSES:
        a = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PaperSpriteActor, unreal.Vector(0, 0, -100000))
        a.tags = ['LabSprite' + pose]
        c = a.get_component_by_class(unreal.PaperSpriteComponent)
        c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
        c.set_editor_property('cast_shadow', True)
        c.set_editor_property('lighting_channels', unreal.LightingChannels(channel0=True, channel1=True, channel2=False))
        c.set_sprite(unreal.load_asset(NORMAL + '/S_' + pose))
        c.set_material(0, unreal.load_asset('/Game/HD2D/M_HD2D_SpriteLift'))
    cam = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.CameraActor, unreal.Vector(0, 0, -100000))
    cam.tags = ['LabCamera']
    c = cam.get_component_by_class(unreal.CameraComponent)
    c.set_editor_property('field_of_view', 35.0)
    c.set_editor_property('constrain_aspect_ratio', False)
    st = c.get_editor_property('post_process_settings')
    st.set_editor_property('override_depth_of_field_focal_distance', True)
    st.set_editor_property('depth_of_field_focal_distance', 1500.0)
    c.set_editor_property('post_process_settings', st)


def build_map(source, name):
    path = LAB + '/' + name
    if EAL.does_asset_exist(path):
        EAL.delete_asset(path)
    EAL.duplicate_asset(source, path)
    world = unreal.EditorLoadingAndSavingUtils.load_map(path)
    light = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PointLight, unreal.Vector(0, 0, -100000))
    light.tags = ['LabCompanion']
    c = light.get_component_by_class(unreal.PointLightComponent)
    c.set_editor_property('mobility', unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('cast_shadows', False)
    c.set_editor_property('intensity_units', unreal.LightUnits.CANDELAS)
    c.set_editor_property('volumetric_scattering_intensity', 0.0)
    c.set_editor_property('visible', False)
    if ART:
        place_normal_test()
    print(name, 'saved', unreal.EditorLoadingAndSavingUtils.save_map(world, path))


if __name__ == '__main__':
    log_to_file('build_light_lab.txt')
    build('M_Lab_Base')
    build('M_Lab_Foliage', foliage=True)
    build('M_Lab_FixedNormal', normal_up=0.5)
    build('M_Lab_Rim', rim=True)
    if ART:
        build_normal_assets()
        build_normal_material('M_Lab_NormalEdge', 'edge')
        build_normal_material('M_Lab_NormalMap', 'bc5')
        build_normal_material('M_Lab_NormalMapRaw', 'raw')
        stats = MEL.get_statistics(unreal.load_asset('/Game/HD2D/M_HD2D_SpriteLift'))
        print('M_HD2D_SpriteLift ps', stats.num_pixel_shader_instructions, 'samplers', stats.num_samplers)
    build_map('/Game/Maps/Village/MAP_Village_Night', 'MAP_Lab_Night')
    build_map('/Game/Maps/Village/MAP_Village_Day', 'MAP_Lab_Day')
    print('done')
