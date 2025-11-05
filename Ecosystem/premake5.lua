project "Ecosystem"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"   -- garde-le en phase avec tes vendors (GLFW/Glad)

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp"
    }

    -- Headers uniquement
    includedirs {
        "src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}"      -- stb: headers seulement
    }

    -- Pas besoin de libdirs si tu compiles les vendors dans la workspace
    libdirs { }

    defines {
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS"
    }

    -- Link uniquement ce qui génère un .lib/.dll
    links {
        "GLFW",
        "Glad",
        "opengl32"               -- Windows OpenGL system lib
        -- NE PAS lier 'stb' (header-only)
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "RELEASE", "NDEBUG" }
        runtime "Release"
        optimize "on"