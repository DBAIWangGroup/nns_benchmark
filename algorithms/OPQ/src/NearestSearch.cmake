# lets divide binaries in groups, for comfort navigation
SOURCE_GROUP(util FILES ${Source_Path}/data_util.h
                        ${Source_Path}/data_util.cpp
                        ${Source_Path}/multitable.hpp
                        ${Source_Path}/perfomance_util.h
                        ${Source_Path}/perfomance_util.cpp)
						
SET(UTIL                ${Source_Path}/data_util.h
                        ${Source_Path}/data_util.cpp
                        ${Source_Path}/multitable.hpp
                        ${Source_Path}/perfomance_util.h
                        ${Source_Path}/perfomance_util.cpp)
						
SOURCE_GROUP(indexer FILES ${Source_Path}/indexer.h)
						   
SET(INDEXER                ${Source_Path}/indexer.h)
						   
SOURCE_GROUP(searcher FILES ${Source_Path}/searcher.h
                            ${Source_Path}/ordered_lists_merger.h)
							
SET(SEARCHER                ${Source_Path}/searcher.h
                            ${Source_Path}/ordered_lists_merger.h)
							

# lets list all Nearest's source binaries
SET(NEAREST_ALL_CC ${UTIL} ${INDEXER} ${SEARCHER})

