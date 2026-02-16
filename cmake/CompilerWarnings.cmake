function(engine_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /wd4127
            /wd4100
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wold-style-cast
            -Woverloaded-virtual
            -Wnon-virtual-dtor
        )
    endif()
endfunction()

