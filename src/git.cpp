#include <codeowners/errors.hpp>
#include <codeowners/filesystem.hpp>
#include <codeowners/git.hpp>

#include <git2/global.h> /* git_libgit2_{init,shutdown} functions */
#include <git2/index.h>
#include <git2/pathspec.h>
#include <git2/repository.h>

#include <memory>
#include <string>

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

template <typename T>
using deleter_type = void (*)(T*);

template <typename T>
using resource_ptr = std::unique_ptr<T, deleter_type<T>>;

using repository_ptr = resource_ptr<::git_repository>;
using index_ptr = resource_ptr<::git_index>;

template <typename T>
struct resource_traits;

template <>
struct resource_traits<::git_repository>
{
    using value_type = ::git_repository;
    constexpr static deleter_type<value_type> deleter = ::git_repository_free;
    constexpr static const char* resource_name = "git_repository";
};

template <>
struct resource_traits<::git_index>
{
    using value_type = ::git_index;
    constexpr static deleter_type<value_type> deleter = ::git_index_free;
    constexpr static const char* resource_name = "git_index";
};

template <>
struct resource_traits<::git_pathspec>
{
    using value_type = ::git_pathspec;
    constexpr static deleter_type<value_type> git_pathspec = ::git_pathspec_free;
    constexpr static const char* resource_name = "git_pathspec";
};

template <typename T, typename F, typename... Args>
resource_ptr<T> make_resource_ptr(F f, Args... args)
{
    using traits = resource_traits<T>;
    T* resource = nullptr;
    int error = f(&resource, std::forward<Args>(args)...);
    if (error)
    {
        using namespace std::string_literals;
        throw co::error { "Error while creating "s + traits::resource_name
            + ": error code " + std::to_string(error) };
    }
    assert(resource);
    return resource_ptr<T>(resource, resource_traits<T>::deleter);
};

//repository_ptr make_repository_ptr(const fs::path& repo_root)
//{
//    ::git_repository* repo = nullptr;
//    int error = ::git_repository_open_ext(&repo, repo_root.c_str(), /*flags*/ 0, /*ceiling dirs*/ nullptr);
//    if (error)
//    {
//        using namespace std::string_literals;
//        throw repository_not_found_error { "Could not find git repository at "s + repo_root.string() };
//    }
//    assert(repo);
//    return repository_ptr(repo, ::git_repository_free);
//}
//
//index_ptr make_index_ptr(::git_repository* repo)
//{
//    assert(repo);
//    ::git_index* index = nullptr;
//    int error = ::git_repository_index(&index, repo);
//    if (error)
//    {
//        throw co::error { "Error getting index: libgit2 error code: " + std::to_string(error) };
//    }
//    assert(index);
//    return index_ptr(index, ::git_index_free);
//}

} /* end namepsace 'co' */