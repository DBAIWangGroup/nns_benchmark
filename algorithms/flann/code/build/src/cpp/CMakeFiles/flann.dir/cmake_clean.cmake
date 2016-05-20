file(REMOVE_RECURSE
  "../../lib/libflann.pdb"
  "../../lib/libflann.so"
  "../../lib/libflann.so.1.8.4"
  "../../lib/libflann.so.1.8"
)

# Per-language clean rules from dependency scanning.
foreach(lang)
  include(CMakeFiles/flann.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
