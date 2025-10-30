include "Dependencies.lua"

workspace "Ecosystem"
	architecture "x86_64"
    startproject "Ecosystem"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/Glad"
	include "vendor/stb"
group ""

group "Ecosystem"
	include "Ecosystem"
group ""

