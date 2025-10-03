add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fno-elide-constructors>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wextra>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Winit-self>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wold-style-cast>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Woverloaded-virtual>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wuninitialized>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wmissing-declarations>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wstrict-aliasing>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wno-omit-frame-pointer>)

add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/EHsc>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4>)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14242>) # conversion, possible loss of data
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14254>) # uninitialized variable
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14263>) # member function hiding
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14265>) # class has virtual functions, destructor non-virtual
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14287>) # unsigned/negative mismatch
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/we4289>) # loop variable hiding outer scope
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14905>) # wide string literal cast
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14906>) # string literal cast
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/w14928>) # illegal copy constructor/assignment

if (ENABLE_ASAN)
	add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fsanitize=address>)
	add_link_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fsanitize=address>)

	add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fsanitize=undefined>)
	add_link_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fsanitize=undefined>)

	add_link_options($<$<COMPILE_LANG_AND_ID:CXX,GNU>:-lasan>)
endif()

if (${FORCE_COLORED_OUTPUT})
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_compile_options (-fdiagnostics-color=always)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		add_compile_options (-fcolor-diagnostics)
	endif ()
endif()
