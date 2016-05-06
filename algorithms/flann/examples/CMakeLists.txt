add_custom_target(examples ALL)


if (BUILD_C_BINDINGS)
    add_executable(flann_example_c flann_example.c)
    target_link_libraries(flann_example_c flann)
    set_target_properties(flann_example_c PROPERTIES COMPILE_FLAGS -std=c99)

    add_dependencies(examples flann_example_c)
    install (TARGETS flann_example_c DESTINATION bin )
endif()

if (HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIR})

	###kdtree_tunning
    add_executable(flann_kdtree_tunning_cpp kdtree_tunning/flann_kdtree_tunning.cpp)
    target_link_libraries(flann_kdtree_tunning_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_kdtree_tunning_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_kdtree_tunning_cpp)
    install (TARGETS flann_kdtree_tunning_cpp DESTINATION bin)
	
	###kdtree_tunning_loadindex
    add_executable(flann_kdtree_loadindex_cpp kdtree_tunning/flann_kdtree_tunning_loadindex.cpp)
    target_link_libraries(flann_kdtree_loadindex_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_kdtree_loadindex_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_kdtree_loadindex_cpp)
    install (TARGETS flann_kdtree_loadindex_cpp DESTINATION bin)

	###hkmeans_tunning
    add_executable(flann_h_kmeans_tunning_cpp hkmeans_tunning/flann_h_kmeans_example.cpp)
    target_link_libraries(flann_h_kmeans_tunning_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_h_kmeans_tunning_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_h_kmeans_tunning_cpp)
    install (TARGETS flann_h_kmeans_tunning_cpp DESTINATION bin)

	###kmeans_tunning
    add_executable(flann_kmeans_tunning_cpp hkmeans_tunning/flann_kmeans_example.cpp)
    target_link_libraries(flann_kmeans_tunning_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_kmeans_tunning_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_kmeans_tunning_cpp)
    install (TARGETS flann_kmeans_tunning_cpp DESTINATION bin)


    ###hkmeans_tunning_load_index
    add_executable(flann_h_kmeans_loadindex_cpp hkmeans_tunning/flann_h_kmeans_tunning_loadindex.cpp)
    target_link_libraries(flann_h_kmeans_loadindex_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_h_kmeans_loadindex_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_h_kmeans_loadindex_cpp)
    install (TARGETS flann_h_kmeans_loadindex_cpp DESTINATION bin)


	###flann_tunning
    add_executable(flann_auto_tunning_cpp auto_tunning/flann_auto_example.cpp)
    target_link_libraries(flann_auto_tunning_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_tunning_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_tunning_cpp)
    install (TARGETS flann_auto_tunning_cpp DESTINATION bin)

	###flann_tunning_loadIndex
    add_executable(flann_auto_loadIndex auto_tunning/flann_auto_loadIndex.cpp)
    target_link_libraries(flann_auto_loadIndex ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_loadIndex ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_loadIndex)
    install (TARGETS flann_auto_loadIndex DESTINATION bin)

	###flann_tunning_randomquery
    add_executable(flann_auto_randomquery auto_tunning/flann_auto_randomquery.cpp)
    target_link_libraries(flann_auto_randomquery ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_randomquery ${MPI_LIBRARIES})
    endif()
    add_dependencies(examples flann_auto_randomquery)
    install (TARGETS flann_auto_randomquery DESTINATION bin)


	###flann_tunning_RC
    add_executable(flann_auto_RC auto_tunning/flann_auto_RC.cpp)
    target_link_libraries(flann_auto_RC ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_RC ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_RC)
    install (TARGETS flann_auto_RC DESTINATION bin)


	###flann_tunning_bigann
    add_executable(flann_auto_bigann auto_tunning/flann_auto_bigann.cpp)
    target_link_libraries(flann_auto_bigann ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_bigann ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_bigann)
    install (TARGETS flann_auto_bigann DESTINATION bin)


	###flann_tunning_K
    add_executable(flann_auto_K auto_tunning/flann_auto_K.cpp)
    target_link_libraries(flann_auto_K ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_K ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_K)
    install (TARGETS flann_auto_K DESTINATION bin)

    ###flann_Dim
    add_executable(flann_auto_Dim auto_tunning/flann_auto_Dim.cpp)
    target_link_libraries(flann_auto_Dim ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_Dim ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_Dim)
    install (TARGETS flann_auto_Dim DESTINATION bin)

    ###flann_tunning_GVAR
    add_executable(flann_auto_GVar auto_tunning/flann_auto_GVar.cpp)
    target_link_libraries(flann_auto_GVar ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_auto_GVar ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_auto_GVar)
    install (TARGETS flann_auto_GVar DESTINATION bin)


	###linear
    add_executable(flann_linear_example_cpp linear/flann_linear_example.cpp)
    target_link_libraries(flann_linear_example_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_linear_example_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_linear_example_cpp)
    install (TARGETS flann_linear_example_cpp DESTINATION bin)

	###linear_randomquery
    add_executable(flann_linear_randomquery_example_cpp linear/flann_linear_example_randomquery.cpp)
    target_link_libraries(flann_linear_randomquery_example_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_linear_randomquery_example_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_linear_randomquery_example_cpp)
    install (TARGETS flann_linear_randomquery_example_cpp DESTINATION bin)


	###test quality of answer
    add_executable(answer_of_quality_cpp linear/answer_of_quality.cpp)
    target_link_libraries(answer_of_quality_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(answer_of_quality_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples answer_of_quality_cpp)
    install (TARGETS answer_of_quality_cpp DESTINATION bin)


	###kdtree_randomquery
    add_executable(flann_kdtree_tunning_randomquery_cpp kdtree_tunning/flann_kdtree_tunning_randomquery.cpp)
    target_link_libraries(flann_kdtree_tunning_randomquery_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_kdtree_tunning_randomquery_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_kdtree_tunning_randomquery_cpp)
    install (TARGETS flann_kdtree_tunning_randomquery_cpp DESTINATION bin)

	###kmeans_randomquery
    add_executable(flann_h_kmeans_tunning_randomquery_cpp hkmeans_tunning/flann_h_kmeans_example_randomquery.cpp)
    target_link_libraries(flann_h_kmeans_tunning_randomquery_cpp ${HDF5_LIBRARIES})
    if (HDF5_IS_PARALLEL)
        target_link_libraries(flann_h_kmeans_tunning_randomquery_cpp ${MPI_LIBRARIES})
    endif()

    add_dependencies(examples flann_h_kmeans_tunning_randomquery_cpp)
    install (TARGETS flann_h_kmeans_tunning_randomquery_cpp DESTINATION bin)


    if (USE_MPI AND HDF5_IS_PARALLEL)
        add_executable(flann_example_mpi flann_example_mpi.cpp)
        target_link_libraries(flann_example_mpi flann_cpp ${HDF5_LIBRARIES} ${MPI_LIBRARIES} ${Boost_LIBRARIES})

        add_dependencies(examples flann_example_mpi)
        install (TARGETS flann_example_mpi DESTINATION bin)
    endif()
else()
    message("hdf5 library not found, not compiling flann_example.cpp")
endif()




