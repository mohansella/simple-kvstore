#cpp files to be compiled (relative paths)
list(APPEND _cpp_files

	main.cpp

)

#convert paths to proper relative paths
foreach(cppFile ${_cpp_files})
	list(APPEND _cpp_file_paths src/${cppFile})
endforeach()

#set This Variable to current module name
set(This SimpleKVStoreApp)

#add module definition
add_executable(
	${This} 
	${_cpp_file_paths}
)

#add header files for the module
target_include_directories(
	${This} 
	PUBLIC include
)

#add dependencies
target_link_libraries(
	${This} PUBLIC 

    SimpleKVStoreLib
	CONAN_PKG::boost
)