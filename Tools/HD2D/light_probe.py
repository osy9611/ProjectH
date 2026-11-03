"""Runs inside a -game process: pins the hero to one spot and switches lighting candidates one at a time.

Start through run_driver.ps1 -Script light_probe.py with HD2D_PROBE set to JSON:
  {"loc": [x, y, z], "sun": [pitch, yaw] or null, "variants": ["base", "companion", ...], "settle": 4, "measure": 12}
Every candidate runs between two "base" segments so drift shows up in the base numbers.
Each segment logs "[Driver] seg <name> <first tick> <last tick>"; ticks count frames since the script armed,
which is also the frame the CSV profile started, so perf_segments.py can slice the CSV by them.
Shots go through <tag>.req files that run_driver.ps1 answers. Nothing here is saved to an asset.
"""
import json, math, os, unreal

SPEC = json.loads(os.environ['HD2D_PROBE'])
LAB = '/Game/HD2D/Lab/'
# Change notifications rerun BP_HD2DEnvironment's construction script, which replaces the component being edited.
QUIET = unreal.PropertyAccessChangeNotifyMode.NEVER


def log(*a):
    unreal.log('[Driver] ' + ' '.join(str(x) for x in a))


def request(tag):
    open(os.path.join(os.environ['HD2D_OUT'], tag + '.req'), 'w').close()


def world():
    for gi in unreal.ObjectIterator(unreal.GameInstance):
        if gi.get_world():
            return gi.get_world()


def find(w, cls, comp_cls):
    actors = unreal.GameplayStatics.get_all_actors_of_class(w, cls)
    return actors[0].get_component_by_class(comp_cls) if actors else None


def linear(c, scale=1.0):
    return unreal.LinearColor(*[(v / 255.0) ** 2.2 * scale for v in (c.r, c.g, c.b)], 1.0)


class Scene:
    def __init__(self, w):
        self.w = w
        self.pawn = unreal.GameplayStatics.get_player_pawn(w, 0)
        self.sprite = self.pawn.get_component_by_class(unreal.PaperFlipbookComponent)
        # The night preset adds a second directional light that lights only channel 1 (build_explore character_light).
        lights = [a.get_component_by_class(unreal.DirectionalLightComponent) for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.DirectionalLight)]
        self.sun = next(c for c in lights if c.get_editor_property('lighting_channels').channel0)
        self.character_light = next((c for c in lights if not c.get_editor_property('lighting_channels').channel0), None)
        self.channels = self.sprite.get_editor_property('lighting_channels')
        self.sky = find(w, unreal.SkyLight, unreal.SkyLightComponent)
        self.material = self.sprite.get_material(0)
        self.pp_original = {}
        self.sky_lower = self.sky.get_editor_property('lower_hemisphere_color')
        self.sun_contact = self.sun.get_editor_property('contact_shadow_length')
        self.sky_intensity = self.sky.get_editor_property('intensity')
        self.light = None

    @property
    def pp(self):
        # Looked up each time: the environment blueprint can rebuild its components, which invalidates a kept reference.
        return find(self.w, unreal.load_asset('/Game/HD2D/BP_HD2DEnvironment').generated_class(), unreal.PostProcessComponent)

    def reset(self):
        self.sprite.set_material(0, self.material)
        self.sprite.set_lighting_channels(self.channels.channel0, self.channels.channel1, self.channels.channel2)
        if self.character_light:
            self.character_light.set_visibility(True)
        if self.light:
            self.light.point_light_component.set_visibility(False)
            self.light = None
        st = self.pp.get_editor_property('settings')
        for k, (flag, v) in self.pp_original.items():
            st.set_editor_property('override_' + k, flag)
            st.set_editor_property(k, v)
        self.pp.set_editor_property('settings', st, QUIET)
        self.sky.set_intensity(self.sky_intensity)
        self.sky.set_editor_property('lower_hemisphere_color', self.sky_lower)
        self.sky.recapture_sky()
        self.sun.set_editor_property('contact_shadow_length', self.sun_contact)

    def material_copy(self, name):
        self.sprite.set_material(0, unreal.load_asset(LAB + name))
        return self.sprite.create_dynamic_material_instance(0)

    def post(self, **values):
        st = self.pp.get_editor_property('settings')
        for k, v in values.items():
            self.pp_original.setdefault(k, (st.get_editor_property('override_' + k), st.get_editor_property(k)))
            st.set_editor_property('override_' + k, True)
            st.set_editor_property(k, v)
        self.pp.set_editor_property('settings', st, QUIET)

    def companion(self, channel_only=False):
        # In front of the sprite towards the camera (the camera looks down -Y) and above the head.
        self.light = unreal.GameplayStatics.get_all_actors_with_tag(self.w, 'LabCompanion')[0]
        self.light.attach_to_actor(self.pawn, '', unreal.AttachmentRule.KEEP_WORLD, unreal.AttachmentRule.KEEP_WORLD, unreal.AttachmentRule.KEEP_WORLD, False)
        self.light.set_actor_relative_location(unreal.Vector(0, 150, 120), False, True)
        c = self.light.point_light_component
        # A fifth of the night preset's lantern fill (180 cd, see build_explore.PRESETS).
        c.set_intensity(SPEC.get('companion', 36.0))
        c.set_attenuation_radius(400.0)
        c.set_light_color(linear(unreal.Color(r=170, g=195, b=255)))
        c.set_lighting_channels(not channel_only, channel_only, False)
        if channel_only:
            self.sprite.set_lighting_channels(True, True, False)
        c.set_visibility(True)

    def arrange_normal_test(self):
        # The two lab sprites stand on the ground around SPEC['row'], the hero is hidden, and a camera at the art spec's
        # distance (1500, pitch -18, FOV 35) frames them so one texel covers about 9 screen pixels.
        cx, cy = SPEC['row']
        self.sprite.set_visibility(False)
        rotation = self.sprite.get_world_rotation()
        self.lab_sprites = {}
        for pose, dx in (('Front', -SPEC.get('gap', 70)), ('3Q', SPEC.get('gap', 70))):
            actor = unreal.GameplayStatics.get_all_actors_with_tag(self.w, 'LabSprite' + pose)[0]
            hit = unreal.SystemLibrary.line_trace_single(self.w, unreal.Vector(cx + dx, cy, 5000), unreal.Vector(cx + dx, cy, -5000),
                                                         unreal.TraceTypeQuery.TRACE_TYPE_QUERY1, False, [self.pawn, actor],
                                                         unreal.DrawDebugTrace.NONE, True)
            ground = hit.to_tuple()[4].z if hit else 0
            actor.set_actor_location_and_rotation(unreal.Vector(cx + dx, cy, ground), rotation, False, True)
            actor.set_actor_scale3d(unreal.Vector(1, 1, 1.0515))
            self.lab_sprites[pose] = actor.get_component_by_class(unreal.PaperSpriteComponent)
            self.ground = ground
        camera = unreal.GameplayStatics.get_all_actors_with_tag(self.w, 'LabCamera')[0]
        d, pitch = SPEC.get('distance', 1500.0), 18.0
        focus = unreal.Vector(cx, cy, self.ground + 88)
        camera.set_actor_location_and_rotation(
            focus + unreal.Vector(0, d * math.cos(math.radians(pitch)), d * math.sin(math.radians(pitch))),
            unreal.Rotator(roll=0, pitch=-pitch, yaw=-90), False, True)
        unreal.GameplayStatics.get_player_controller(self.w, 0).set_view_target_with_blend(camera, 0)
        log('normal test ground', self.ground, 'rotation', rotation)

    def normal_variant(self, suffix, material, **params):
        for pose, c in self.lab_sprites.items():
            c.set_sprite(unreal.load_asset(LAB + 'Normal/S_' + pose + suffix))
            c.set_material(0, unreal.load_asset(material))
            if params:
                mid = c.create_dynamic_material_instance(0)
                for k, v in params.items():
                    mid.set_scalar_parameter_value(k, v)


def normal_params():
    return dict(NormalStrength=SPEC.get('normal_strength', 1.0), FlipY=SPEC.get('flip_y', 1.0))


def probe_light(s):
    # A plain point light around the row, to check which side each normal source lights.
    s.light = unreal.GameplayStatics.get_all_actors_with_tag(s.w, 'LabCompanion')[0]
    ox, oy, oz = SPEC.get('probe_offset', [-200, 100, 90])
    s.light.set_actor_location(unreal.Vector(SPEC['row'][0] + ox, SPEC['row'][1] + oy, s.ground + oz), False, True)
    c = s.light.point_light_component
    c.set_intensity(SPEC.get('probe_cd', 200.0))
    c.set_attenuation_radius(700.0)
    c.set_light_color(unreal.LinearColor(1, 1, 1, 1))
    c.set_lighting_channels(True, False, False)
    c.set_visibility(True)


def rim(s):
    mid = s.material_copy('M_Lab_Rim')
    forward = unreal.MathLibrary.get_forward_vector(s.sun.get_owner().get_actor_rotation())
    mid.set_vector_parameter_value('RimDir', unreal.LinearColor(forward.x, forward.y, forward.z, 0))
    mid.set_vector_parameter_value('RimColor', linear(s.sun.get_editor_property('light_color'), SPEC.get('rim', 0.75)))


def sky_lower(s):
    s.sky.set_editor_property('lower_hemisphere_color', unreal.LinearColor(*SPEC.get('sky_lower', [0.3, 0.33, 0.5]), 1))
    s.sky.recapture_sky()
    log('sky', s.sky.get_editor_property('lower_hemisphere_is_black'), s.sky.get_editor_property('lower_hemisphere_color'),
        s.sky.get_editor_property('intensity'), s.sky.get_editor_property('source_type'), s.sky.get_editor_property('real_time_capture'))


def lift(s):
    s.sprite.create_dynamic_material_instance(0, s.material).set_scalar_parameter_value('Lift', SPEC.get('lift', 0.35))


VARIANTS = dict(
    base=lambda s: None,
    companion=lambda s: s.companion(),
    lift=lift,
    sky_lower=sky_lower,
    # Diagnostic: shows whether the sky light reaches the sprite at all.
    sky_up=lambda s: s.sky.set_intensity(s.sky_intensity * 3),
    exposure=lambda s: s.post(auto_exposure_bias=1.263 + SPEC.get('exposure', 0.75)),
    local_exposure=lambda s: s.post(local_exposure_shadow_contrast_scale=0.6),
    channel=lambda s: s.companion(channel_only=True),
    lab_base=lambda s: s.material_copy('M_Lab_Base'),
    foliage=lambda s: s.material_copy('M_Lab_Foliage').set_scalar_parameter_value('Transmission', SPEC.get('foliage', 0.4)),
    fixed_normal=lambda s: s.material_copy('M_Lab_FixedNormal').set_scalar_parameter_value('NormalUp', SPEC.get('fixed_normal', 0.5)),
    rim=rim,
    bloom_threshold=lambda s: s.post(bloom_threshold=SPEC.get('bloom_threshold', 1.0)),
    contact_shadow=lambda s: s.sun.set_editor_property('contact_shadow_length', 0.08),
    # Shared-style checks on the real maps: switch off what build_explore added.
    no_character_light=lambda s: s.character_light.set_visibility(False),
    no_foliage=lambda s: s.sprite.create_dynamic_material_instance(0, s.material).set_scalar_parameter_value('Transmission', 0.0),
    no_lumen=lambda s: s.post(dynamic_global_illumination_method=unreal.DynamicGlobalIlluminationMethod.NONE),
    # Normal-map test (needs "row"): A shared material, B alpha-edge normal, C PixelLab map as BC5 or uncompressed.
    A=lambda s: s.normal_variant('', '/Game/HD2D/M_HD2D_SpriteLift'),
    B=lambda s: s.normal_variant('', LAB + 'M_Lab_NormalEdge', EdgeWidth=SPEC.get('edge_width', 18.0), **normal_params()),
    C=lambda s: s.normal_variant('_N', LAB + 'M_Lab_NormalMap', **normal_params()),
    Craw=lambda s: s.normal_variant('_NRaw', LAB + 'M_Lab_NormalMapRaw', **normal_params()),
    sun_priority=lambda s: s.sun.set_editor_property('forward_shading_priority', 1),
    no_lumen_sky_lower=lambda s: (VARIANTS['no_lumen'](s), sky_lower(s)),
)

# Candidates that read another candidate's SPEC value.
PARAM = dict(channel='companion', B='normal_strength', C='normal_strength', Craw='normal_strength')

# (seconds after ready, action, argument), built once the scene is ready.
plan, ticks, clock, scene = [], 0, 0.0, None


def build_plan():
    t = 4.0
    # Scouting only: visit candidate spots and shoot each, to pick "loc".
    for i, loc in enumerate(SPEC.get('scout', [])):
        plan.append((t, 'move', loc)); plan.append((t + 3, 'shot', 'scout_%d' % i)); t += 4
    for name in ('lab_base', 'foliage', 'fixed_normal', 'rim'):
        if name in [v.partition('@')[0] for v in SPEC['variants']]:
            # Shaders for the lab copies compile on first use; a short pass keeps that out of the measured segments.
            plan.append((t, 'apply', name)); t += 3
    for i, name in enumerate(SPEC['variants']):
        plan.append((t, 'apply', name))
        t += SPEC.get('settle', 4)
        plan.append((t, 'shot', '%02d_%s' % (i, name)))
        plan.append((t + 0.5, 'start', name))
        t += SPEC.get('measure', 12)
        plan.append((t, 'end', name))
    plan.append((t + 1, 'quit', ''))


seg_start = {}


def tick(dt):
    global ticks, clock, scene
    ticks += 1
    if scene is None:
        w = world()
        if not w or not unreal.GameplayStatics.get_player_pawn(w, 0) or ticks < 120:
            return
        scene = Scene(w)
        request('focus')
        if SPEC.get('sun'):
            scene.sun.get_owner().set_actor_rotation(unreal.Rotator(roll=0, pitch=SPEC['sun'][0], yaw=SPEC['sun'][1]), True)
        scene.pawn.set_actor_location(unreal.Vector(*SPEC['loc']), False, True)
        if SPEC.get('row'):
            scene.arrange_normal_test()
        log('ready tick', ticks, 'pawn', scene.pawn.get_actor_location(), 'material', scene.material.get_path_name())
        for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor):
            if a.get_class().get_name() == 'BP_NPC_C':
                log('npc', a.get_actor_location())
        build_plan()
        return
    clock += dt
    while plan and plan[0][0] <= clock:
        _, action, arg = plan.pop(0)
        try:
            if action == 'apply':
                scene.reset()
                # "companion@12" runs the companion candidate with its SPEC value set to 12.
                name, _, value = arg.partition('@')
                if value:
                    SPEC[PARAM.get(name, name)] = float(value)
                VARIANTS[name](scene)
                if SPEC.get('probe'):
                    probe_light(scene)
                log('apply', arg, 'tick', ticks)
            elif action == 'move':
                scene.pawn.set_actor_location(unreal.Vector(*arg), False, True)
            elif action == 'shot':
                request(arg)
            elif action == 'start':
                seg_start[arg] = ticks
            elif action == 'end':
                log('seg', arg, seg_start.pop(arg), ticks)
            elif action == 'quit':
                unreal.SystemLibrary.quit_game(scene.w, None, unreal.QuitPreference.QUIT, False)
        except Exception as e:
            log('step failed', action, arg, repr(e))


unreal.register_slate_post_tick_callback(tick)
log('armed', SPEC)
