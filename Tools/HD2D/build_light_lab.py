"""Builds the lighting study assets under /Game/HD2D/Lab. Safe to rerun.

Material copies of M_HD2D_SpriteLift rebuild the same graph plus one change; the shared material is never touched.
Map copies of the day and night presets add one hidden point light tagged LabCompanion,
because a -game Python script cannot spawn actors; light_probe.py attaches it to the hero when a candidate needs it.
Run: UnrealEditor-Cmd.exe <uproject> -run=pythonscript -script=<this file> -unattended
Result lines go to Client/Saved/HD2D_Explore/build_light_lab.txt.
"""
import os, sys, unreal

EAL = unreal.EditorAssetLibrary
MEL = unreal.MaterialEditingLibrary
LAB = '/Game/HD2D/Lab'

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
    print(name, 'saved', unreal.EditorLoadingAndSavingUtils.save_map(world, path))


if __name__ == '__main__':
    log_to_file('build_light_lab.txt')
    build('M_Lab_Base')
    build('M_Lab_Foliage', foliage=True)
    build('M_Lab_FixedNormal', normal_up=0.5)
    build('M_Lab_Rim', rim=True)
    build_map('/Game/Maps/Village/MAP_Village_Night', 'MAP_Lab_Night')
    build_map('/Game/Maps/Village/MAP_Village_Day', 'MAP_Lab_Day')
    print('done')
