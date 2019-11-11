add_executable(ls-owners
               ls-owners.x.cpp)
target_link_libraries(ls-owners PRIVATE codeowners Boost::program_options)
