VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["Ecosystem"] = "%{wks.location}/Ecosystem/src"
IncludeDir["GLFW"] = "%{wks.location}/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/vendor/stb/includes"

LibraryDir = {}

Library = {}