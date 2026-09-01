project "raylib"
    kind "StaticLib"
    language "C"
    cdialect "C11"

    targetdir ("../../Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("../../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    includedirs
    {
        "raylib/src",
        "raylib/src/external",
        "raylib/src/external/glfw/include"
    }

    files
    {
        "raylib/src/raylib.h",
        "raylib/src/raymath.h",
        "raylib/src/rlgl.h",

        "raylib/src/raudio.c",
        "raylib/src/rcore.c",
        "raylib/src/rglfw.c",
        "raylib/src/rmodels.c",
        "raylib/src/rshapes.c",
        "raylib/src/rtext.c",
        "raylib/src/rtextures.c"
    }

    defines
    {
        "PLATFORM_DESKTOP_GLFW",
        "GRAPHICS_API_OPENGL_33",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"