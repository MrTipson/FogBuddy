newoption {
	trigger     = "opencv",
	value       = "path",
	description = "OpenCV installation path"
}

workspace "FogBuddy"
	configurations { "Release" }
	startproject "FogBuddy"
	architecture "x86_64"

project "imgui"
	location ("deps/imgui")
	kind "StaticLib"
	targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
	objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"

	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/backends/imgui_impl_win32.cpp",
		"%{prj.location}/backends/imgui_impl_dx11.cpp",
		"%{prj.location}/misc/cpp/imgui_stdlib.cpp"
	}

	includedirs {
		"%{prj.location}"
	}

project "FogBuddy"
	kind "ConsoleApp"
	language "C++"
	location "FogBuddy"
	
	targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
	objdir	  "bin-int/%{cfg.buildcfg}/%{prj.name}"
	debugdir  "%{cfg.targetdir}"

	files { "%{prj.location}/**.h", "%{prj.location}/**.cpp", "%{prj.location}/**.hpp" }
	cppdialect "C++17"

	if _OPTIONS["opencv"] == nil then
		error("You must pass opencv dir")
	else
		print(_OPTIONS["opencv"])
	end

	includedirs {
		_OPTIONS["opencv"].."/build/include",
		"deps/imgui"
	}
	libdirs { _OPTIONS["opencv"].."/build/x64/vc15/lib" }

	links {
		"opencv_world460",
		"imgui",
		"d3d11"
	}

	postbuildcommands {
		"{COPY} ".._OPTIONS["opencv"].."/build/x64/vc15/bin/opencv_world460.dll %{cfg.targetdir}",
		"{COPY} ../data %{cfg.targetdir}/data"
	}