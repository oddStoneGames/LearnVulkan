workspace "LearnVulkan"
    startproject "LearnVulkan"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-x64"

include "LearnVulkan/dependencies/GLFW"

project "LearnVulkan"
    location "LearnVulkan"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/dependencies",
        "LearnVulkan/dependencies/GLFW/include",
        "LearnVulkan/dependencies/GLM",
        "D:/Softwares/VulkanSDK/Include"
    }

    libdirs 
    {
        "D:/Softwares/VulkanSDK/Lib"
    }

    links
    {
        "GLFW",
        "vulkan-1.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GLFW_INCLUDE_VULKAN",
            "GLM_FORCE_RADIANS",
            "GLM_FORCE_DEPTH_ZERO_TO_ONE"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"