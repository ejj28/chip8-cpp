project('chip8-cpp', 'cpp',
        default_options: 'default_library=static'
)


imgui_path = 'lib/imgui'
imgui_src = [
        imgui_path / 'imgui.cpp',
        imgui_path / 'imgui_draw.cpp',
        imgui_path / 'imgui_tables.cpp',
        imgui_path / 'imgui_widgets.cpp',
        imgui_path / 'backends/imgui_impl_sdl2.cpp',
        imgui_path / 'backends/imgui_impl_sdlrenderer2.cpp',
]

chip8_src = [
        'src/main.cpp',
        'src/frontend.cpp',
        'src/logic.cpp',
        'src/instructions.cpp'
]

if host_machine.system() == 'nx'

        DEVKITPRO = '/opt/devkitpro'
        LIBNX = DEVKITPRO / 'libnx'
        PORTLIBS = DEVKITPRO / 'portlibs/switch'
        env = environment()
        env.set('DEVKITPRO', DEVKITPRO)

        dkp_tools_bin = DEVKITPRO / 'tools/bin'

        libnx_dep = declare_dependency(
                link_args: ['-L' + LIBNX / 'lib', '-lnx'],
                include_directories: include_directories(LIBNX / 'include')
        )

        # I'm not convinced this actually works
        # Can you tell that I'm very new to build systems and generally C++ projects that aren't a single file?
        libsdl2_config = run_command(PORTLIBS / 'bin/sdl2-config', '--libs', check: true).stdout().strip()
        libsdl2_dep = declare_dependency(
                link_args: ['-L' + PORTLIBS / 'lib', libsdl2_config, '-lSDL2', '-lEGL', '-lGLESv2', '-ldrm_nouveau', '-lglapi'],
                include_directories: include_directories(PORTLIBS / 'include/SDL2')
        )

        nx_specs = LIBNX / 'switch.specs'

        nx_cpp_args = ['-D__SWITCH__', '-fno-rtti', '-DIMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS', '-fPIC']
        nx_link_args = ['-specs=' + nx_specs, '-g', '-march=armv8-a+crc+crypto', '-mtune=cortex-a57', '-mtp=soft', '-fPIE']

        elf = executable('chip8.elf', [chip8_src, imgui_src],
                dependencies: [libnx_dep, libsdl2_dep],
                include_directories: ['lib/imgui_club/imgui_memory_editor', imgui_path, imgui_path / 'backends'],
                cpp_args: nx_cpp_args,
                link_args: nx_link_args
        )

        nro = custom_target('nro',
                input: elf,
                output: 'chip8.nro',
                command: [dkp_tools_bin/'elf2nro', '@INPUT@', '@OUTPUT@']
        )

elif host_machine.system() == 'psp'
        PSPDEV = '/opt/pspdev'
        PSPDEV_SDK = PSPDEV / 'psp/sdk'
        PSPDEV_BIN = PSPDEV / 'bin'

        psp_sdk_deps = declare_dependency(
                link_args: ['-L' + PSPDEV_SDK / 'lib', '-lstdc++', '-lpspdebug', '-lpspdisplay', '-lpspge', '-lpspgu'],
                include_directories: include_directories(PSPDEV_SDK / 'include')
        )

        libsdl2_dep = declare_dependency(
                link_args: ['-L' + PSPDEV / 'psp/lib', '-lSDL2', '-lSDL2main', '-lGL', '-lGLU', '-lglut', '-lz', '-lpspvfpu', '-lpsphprm', '-lpspsdk', '-lpspctrl', '-lpspumd', '-lpsprtc', '-lpsppower', '-lpspgum', '-lpspgu', '-lpspaudiolib', '-lpspaudio', '-lpsphttp', '-lpspssl', '-lpspwlan', '-lpspnet_adhocmatching', '-lpspnet_adhoc', '-lpspnet_adhocctl', '-lm', '-lpspvram'],
                include_directories: include_directories(PSPDEV / 'psp/include/SDL2')
        )

        psp_cpp_args = ['-DIMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS', '-DPSP', '-D__PSP__', '-D_PSP_FW_VERSION=600']
        psp_link_args = ['-L/opt/pspdev/lib', '-L/opt/pspdev/psp/lib', '-Wl,-zmax-page-size=128']

        elf = executable('chip8.elf', [chip8_src, imgui_src],
                dependencies: [psp_sdk_deps, libsdl2_dep],
                include_directories: [PSPDEV / 'psp/include', imgui_path, imgui_path / 'backends', 'lib/imgui_club/imgui_memory_editor'],
                cpp_args: psp_cpp_args,
                link_args: psp_link_args
        )

        fixup = custom_target('fixup',
                input: elf,
                output: 'chip8.elf.fix',
                command: [PSPDEV_BIN/'psp-fixup-imports', '-o@OUTPUT@', '@INPUT@']
        )

        mksfo = custom_target('mksfo',
                output: 'PARAM.SFO',
                command: [PSPDEV_BIN/'mksfoex', '-d', 'MEMSIZE=1', '-s', 'APP_VER=01.00', 'Chip8', '@OUTPUT@']
        )

        makepbp = custom_target('make-pbp',
                input: [fixup, mksfo],
                output: 'EBOOT.PBP',
                command: [PSPDEV_BIN/'pack-pbp', '@OUTPUT@', mksfo, 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', fixup, 'NULL']
        )

elif host_machine.system() == 'windows'
        sdl2_dep = dependency('sdl2')
        sdl2_main_dep = dependency('sdl2main')

        executable('chip8', [chip8_src, imgui_src],
                win_subsystem: 'windows',
                dependencies: [sdl2_dep, sdl2_main_dep],
                include_directories : ['lib/imgui_club/imgui_memory_editor', imgui_path, imgui_path / 'backends']
        )
endif