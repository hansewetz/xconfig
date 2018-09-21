# create makefile containing correct installation path
configure_file(makefile.in ${CMAKE_CURRENT_BINARY_DIR}/makefile)

# add executable
add_executable (example1 example1.cc)
TARGET_LINK_LIBRARIES(example1 xconfigl)

# installation (executable + cfg file for testing + makefile)
install(TARGETS example1 DESTINATION examples/example1)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/makefile example1.cc example1.cfg DESTINATION examples/example1)
