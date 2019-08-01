#include <codeowners/repository.hpp>

#include <git2/global.h> /* git_libgit2_{init,shutdown} functions */

namespace co
{

struct libgit_handle
{
    libgit_handle() { ::git_libgit2_init(); }
    ~libgit_handle() { ::git_libgit2_shutdown(); }
};

namespace
{
/* Module-level global to ensure that init/shutdown functions are called. */
static libgit_handle handle;
} /* end anonymous namespace */

} /* end namepsace 'co' */