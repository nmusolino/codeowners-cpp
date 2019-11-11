add_executable(ls-owners
               ls-owners.x.cpp)
# This dependency on `Threads` (pthread) was added solely to get the Linux CI
# to build.  It should not be here.
find_package(Threads)
target_link_libraries(ls-owners PRIVATE codeowners Boost::program_options Threads::Threads)
