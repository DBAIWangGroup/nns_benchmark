add_custom_target(examples ALL)


#if (BUILD_C_BINDINGS)
    #add_executable(flann_example_c flann_example.c)
    #target_link_libraries(flann_example_c flann)
    #set_target_properties(flann_example_c PROPERTIES COMPILE_FLAGS -std=c99)

    #add_dependencies(examples flann_example_c)
    #install (TARGETS flann_example_c DESTINATION bin )
#endif()

if (HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIR})

	###build kdtree
    add_executable(build_kdtree build_kdtree.cpp)
    target_link_libraries(build_kdtree ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(build_kdtree ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples build_kdtree)
    install (TARGETS build_kdtree DESTINATION bin)

	##search kdtree
	add_executable(search_kdtree search_kdtree.cpp)
    target_link_libraries(search_kdtree ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(search_kdtree ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples search_kdtree)
    install (TARGETS search_kdtree DESTINATION bin)

	###build hkmeans
    add_executable(build_hkmeans build_hkmeans.cpp)
    target_link_libraries(build_hkmeans ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(build_hkmeans ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples build_hkmeans)
    install (TARGETS build_hkmeans DESTINATION bin)

	##search hkmeans
	add_executable(search_hkmeans search_hkmeans.cpp)
    target_link_libraries(search_hkmeans ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(search_hkmeans ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples search_hkmeans)
    install (TARGETS search_hkmeans DESTINATION bin)

	###build auto
    add_executable(build_auto build_auto.cpp)
    target_link_libraries(build_auto ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(build_auto ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples build_auto)
    install (TARGETS build_auto DESTINATION bin)

	##search auto
	add_executable(search_auto search_auto.cpp)
    target_link_libraries(search_auto ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(search_auto ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples search_auto)
    install (TARGETS search_auto DESTINATION bin)


	###linear
    add_executable(linear linear.cpp)
    target_link_libraries(linear ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(linear ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples linear)
    install (TARGETS linear DESTINATION bin)


    if (USE_MPI AND HDF5_IS_PARALLEL)
        add_executable(flann_example_mpi flann_example_mpi.cpp)
        target_link_libraries(flann_example_mpi flann_cpp ${HDF5_LIBRARIES} ${MPI_LIBRARIES} ${Boost_LIBRARIES})

        add_dependencies(examples flann_example_mpi)
        install (TARGETS flann_example_mpi DESTINATION bin)
    endif()
else()
    message("hdf5 library not found, not compiling flann_example.cpp")
endif()




