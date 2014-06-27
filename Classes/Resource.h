#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#define BIRD_DIE_MUSIC "sfx_die.mp3"
#define BIRD_HIT_MUSIC "sfx_hit.mp3"
#define BIRD_POINT_MUSIC "sfx_point.mp3"
#define BIRD_SWOOSHING_MUSIC "sfx_swooshing.mp3"
#define BIRD_WING_MUSIC "sfx_wing.mp3"

#else

#define BIRD_DIE_MUSIC "sfx_die.ogg"
#define BIRD_HIT_MUSIC "sfx_hit.ogg"
#define BIRD_POINT_MUSIC "sfx_point.ogg"
#define BIRD_SWOOSHING_MUSIC "sfx_swooshing.ogg"
#define BIRD_WING_MUSIC "sfx_wing.ogg"

#endif

#endif