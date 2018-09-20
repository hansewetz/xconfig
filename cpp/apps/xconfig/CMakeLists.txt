FIND_PACKAGE( Boost 1.57 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable (xconfig xconfig.cc)
TARGET_LINK_LIBRARIES(xconfig xconfigl Boost::program_options)

install(TARGETS xconfig DESTINATION bin)
