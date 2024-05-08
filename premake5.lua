workspace "CryptographyApp"
   configurations { "Debug", "Release" }
   platforms { "x32", "x64"}
   
project "CryptographyApp"
	language "C++"
	cppdialect "C++17"
	
	targetdir "bin/%{cfg.architecture}/%{cfg.buildcfg}"
	objdir "bin-int/%{cfg.architecture}/%{cfg.buildcfg}"
	defines "GLEW_STATIC"
   
	includedirs {
				 "./vendors/glew/include",
				 "./vendors/stb/include",
				 "./vendors/glfw/include",
				 "./vendors/imgui/include",
				 "./vendors/csprng/include",
				 "./vendors/encrypt/include",
				 "./vendors/nativefiledialog/include"
			    }

	files { "vendors/**.c", "vendors/**.cpp", "src/**.h", "src/**.c", "src/**.hpp", "src/**.cpp" }
	links { "opengl32.lib", "glew32s.lib", "glfw3_mt.lib" }
	
	filter { "platforms:x32" }
		libdirs { "vendors/glew/lib/x32", "vendors/glfw/lib/x32" }
	filter { "platforms:x64" }
		libdirs { "vendors/glew/lib/x64", "vendors/glfw/lib/x64" }
	
	filter "configurations:Debug"
		kind "ConsoleApp"
		defines { "DEBUG" }
		symbols "On"
		filter { "platforms:x32" }
			architecture "x86"
		filter { "platforms:x64" }
			architecture "x86_64"

	filter "configurations:Release"
		kind "WindowedApp"
		defines { "NDEBUG" }
		optimize "Speed"
		filter { "platforms:x32" }
			architecture "x86"
		filter { "platforms:x64" }
			libdirs { "vendors/glew/lib/x64", "vendors/glfw/lib/x64" }
