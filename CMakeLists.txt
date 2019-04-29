cmake_minimum_required(VERSION 2.6)

project( powerlaw )

# Boost library
set( Boost_USE_STATIC_LIBS ON )
set( Boost_USE_MULTITHREAD ON )

find_package( Boost COMPONENTS program_options REQUIRED )

set( LIBRARY_OUTPUT_PATH ${powerlaw_BINARY_DIR}/lib CACHE INTERNAL "Single library output directory")
set( EXECUTABLE_OUTPUT_PATH ${powerlaw_BINARY_DIR}/bin CACHE INTERNAL "Single binary output directory")

include_directories( ${Boost_INCLUDE_DIRS} )

if( Boost_FOUND )

	# powerlaw library
	add_library( powerlaw powerlawCommon.h powerlawCommon.cpp )
	
	# powerlaw fit
  add_executable( powerlaw_fit powerlawFit.cpp )
  target_link_libraries( powerlaw_fit powerlaw ${Boost_LIBRARIES} )

else( Boost_FOUND )

  message( fatal_error "Cannot build without boost. Please install the boost library first." )

endif( Boost_FOUND )
