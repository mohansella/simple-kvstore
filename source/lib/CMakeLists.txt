#cpp files to be compiled (relative paths)
list(APPEND _cpp_files

	mohansella/serial/Reader.cpp
	mohansella/serial/Writer.cpp
	mohansella/serial/FileReader.cpp
	mohansella/serial/FileWriter.cpp

	mohansella/kvstore/SimpleKVStore.cpp
	mohansella/kvstore/StoreValue.cpp

)

#convert paths to proper relative paths
foreach(cppFile ${_cpp_files})
	list(APPEND _cpp_file_paths src/${cppFile})
endforeach()

#set This Variable to current module name
set(This SimpleKVStoreLib)

#add module definition
add_library(
	${This} STATIC 
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

	CONAN_PKG::boost
)