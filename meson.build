project('File Encrypter', 'c', version: '1.0')

c_flags = ['-std=c11', '-Wall', '-Wextra', '-pedantic']

add_global_arguments('-Wl,-rpath=./', language: 'c')
if get_option('buildtype') == 'release'
  add_global_arguments('-DRELEASE', language: 'c')
  c_flags += ['-O3','-s']
else
  c_flags += ['-O0','-ggdb']
endif

sdl_options = [
  'default_library=static',
  'prefer_dlopen=false', 
  'warning_level=0',
  'werror=false',
  'use_loadso=enabled',
  'prefer_dlopen=true',
  'use_video=enabled',
  'use_atomic=enabled',
  'use_threads=disabled',
  'use_timers=enabled',
  'with_main=true',
  'use_events=enabled',
]

if host_machine.system() == 'darwin' or host_machine.system() == 'windows'
  sdl_options += [
    'use_video_x11=disabled',
    'use_video_wayland=disabled',
  ]
else
  sdl_options += [
    'use_render=enabled',
    'use_video_x11=auto',
    'use_video_wayland=auto',
  ]
endif

sdl_dep = dependency('sdl2', fallback: ['sdl2'], required: true, static: true)
if host_machine.system() != 'windows'
  sdl_options += [
    'use_video_opengl=auto',
    'use_video_openglesv2=auto',
  ]

  sdlmain_dep = dependency('', required: false)
else
  sdl_options += [
    'use_video_opengl=disabled',
    'use_video_openglesv2=disabled',
  ]

  c_flags += ['-Dmain=SDL_main','-mwindows']
  if sdl_dep.type_name() == 'internal'
            sdlmain_dep = dependency('sdl2main', fallback: ['sdl2main_dep'])
  else
            sdlmain_dep = cc.find_library('SDL2main')
  endif
endif


# we don't need these
sdl_options += [
  'test=false',
  'use_sensor=disabled',
  'use_haptic=disabled',
  'use_audio=disabled',
  'use_cpuinfo=disabled',
  'use_joystick=disabled',
  'use_video_vulkan=disabled',
  'use_video_offscreen=disabled',
  'use_power=disabled',
  'system_iconv=disabled',
]

include_dir = include_directories('./src/include')
libsodium_include_dir = include_directories('./libsodium/include')

if host_machine.system() == 'windows'

  deps = ['-lm','-mwindows','-static']
else
  deps = [
   '-lm',
   '-ldl',
  ]
endif

compiler = meson.get_compiler('c')
if host_machine.system() == 'linux'
  libcrypto_dep = compiler.find_library('sodium', dirs: [meson.current_source_dir() / 'libsodium/linux_x86_64'])
endif

if host_machine.system() == 'windows'
  libcrypto_dep = compiler.find_library('sodium', dirs: [meson.current_source_dir() / 'libsodium/window_x86_64'])
endif

executable(
  'cryption',
  sources: ['src/main.c','src/list.c','src/utils/file_util.c' , 'src/encrypter/encrypter.c'],
  include_directories: [include_dir,libsodium_include_dir],
  c_args: c_flags,
  dependencies: [
    sdl_dep,sdlmain_dep,libcrypto_dep
  ],
  link_args: deps,
  gui_app : true,
)


