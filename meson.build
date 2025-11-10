project('chip8-cpp', 'cpp',
        default_options: 'default_library=static')

sdl2_dep = dependency('sdl2')
sdl2_main_dep = dependency('sdl2main')

imgui_dep = dependency('imgui')

incdir = include_directories('lib/imgui_club/imgui_memory_editor')

src = ['src/main.cpp', 'src/frontend.cpp', 'src/logic.cpp', 'src/instructions.cpp']

executable('chip8', src,
            win_subsystem: 'windows',
            dependencies: [sdl2_dep, sdl2_main_dep, imgui_dep],
            include_directories : incdir
            )

