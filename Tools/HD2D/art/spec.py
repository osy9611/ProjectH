"""Character sprite production spec: the one place the numbers live.

Docs/HD2D/character-sprite-spec.md explains each value; check_sprite.py and build_artspec.py read them from here.
"""
import math

# Exploration camera from art-tech-decisions.md. FOV is horizontal in UE.
CAMERA_FOV = 35.0
CAMERA_DISTANCE = 1500.0
SCREEN_WIDTH = 1920
SCREEN_DOT_PX = 6  # one art dot covers this many screen pixels at CAMERA_DISTANCE


def px_per_uu(distance=CAMERA_DISTANCE):
    return SCREEN_WIDTH / (2 * distance * math.tan(math.radians(CAMERA_FOV / 2)))


UU_PER_DOT = SCREEN_DOT_PX / px_per_uu()  # 2.956
PIXELS_PER_UNREAL_UNIT = 1 / UU_PER_DOT  # Paper2D sprite setting, 0.3383

# Every field frame shares one cell. The pivot is the bottom centre of the soles.
CELL_W, CELL_H = 64, 80
PIVOT = (32, 76)  # texture pixels; soles end on row 75, rows 76-79 stay empty
SOLE_ROW = PIVOT[1] - 1
# Directions drawn per animation. RS is LS mirrored at runtime; a character listed in mirror_exceptions draws RS too.
DIRECTIONS = ('FR', 'BK', 'LS')
OPTIONAL_DIRECTIONS = ('RS',)

# Field animations by role: frames per direction and playback fps.
ROLES = {
    'hero': dict(colors=40, anims={'Idle': (4, 5), 'Walk': (8, 10), 'Run': (6, 12)}),
    'npc': dict(colors=24, anims={'Idle': (2, 3), 'Walk': (4, 8)}),
}

# Outline: the silhouette ring is dark but not black, Rec.601 luma in 0..255.
OUTLINE_LUMA_MAX = 80
OUTLINE_LUMA_MIN = 8
OUTLINE_RATIO_MIN = 0.95


# Day exposure plus sprite Lift maps a source luma of about 190 to 233 and clips from about 210 (MAP_Village_Day capture).
# Only small glints (eye shine, metal) may go brighter.
TONE_LUMA_MAX = 210
BRIGHT_RATIO_MAX = 0.01


def luma(r, g, b):
    return 0.299 * r + 0.587 * g + 0.114 * b
