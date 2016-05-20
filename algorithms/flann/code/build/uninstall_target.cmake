if(NOT EXISTS "/home/yzhang4/HD/release/algorithms/flann/code/build/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/home/yzhang4/HD/release/algorithms/flann/code/build/install_manifest.txt\"")
endif(NOT EXISTS "/home/yzhang4/HD/release/algorithms/flann/code/build/install_manifest.txt")

file(READ "/home/yzhang4/HD/release/algorithms/flann/code/build/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    if(EXISTS "$ENV{DESTDIR}${file}")
        exec_program("/home/yzhang4/HD/home/liwen/Downloads/software/CMake3.2/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
            OUTPUT_VARIABLE rm_out RETURN_VALUE rm_retval)
        if(NOT "${rm_retval}" STREQUAL 0)
            message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
        endif(NOT "${rm_retval}" STREQUAL 0)
    else(EXISTS "$ENV{DESTDIR}${file}")
        message(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
    endif(EXISTS "$ENV{DESTDIR}${file}")
endforeach(file)

