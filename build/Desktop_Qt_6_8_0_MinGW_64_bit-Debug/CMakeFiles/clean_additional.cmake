# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Carousel_image_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Carousel_image_autogen.dir\\ParseCache.txt"
  "Carousel_image_autogen"
  )
endif()
