#include <codeowners/codeowners.hpp>
#include <codeowners/filesystem.hpp>
#include <codeowners/parser.hpp>
#include <codeowners/path_sources.hpp>
#include <codeowners/repository.hpp>
#include <codeowners/ruleset.hpp>

#include <boost/program_options.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/single.hpp>

#include <codeowners/parser.hpp>
#include <cstdlib>
#include <iostream>

namespace po = boost::program_options;

constexpr const char* PROGRAM_NAME = "ls-owners";

struct list_owners_options
{
    bool debug;
    boost::optional<fs::path> repo_dir;
    bool include_ignored;
    std::vector<fs::path> paths;
};

std::string arg_usage_string(const po::option_description& opt)
{
    const auto& value_semantic = *(opt.semantic());
    const bool is_optional = !value_semantic.is_required();

    std::ostringstream oss;
    if (is_optional)
        oss << "[";
    oss << "--" << opt.long_name();
    if (value_semantic.min_tokens() > 0)
        oss << " " << boost::algorithm::to_upper_copy(opt.long_name());
    if (is_optional)
        oss << "]";
    return oss.str();
}

std::ostream& print_help(std::ostream& os, const po::options_description& opts_desc)
{
    os << "USAGE:\n\t" << PROGRAM_NAME;
    for (const auto opt_ptr : opts_desc.options())
    {
        os << " " << (opt_ptr->long_name() == "help" ? "" : arg_usage_string(*opt_ptr));
    }
    os << " [PATHS]\n\n"
       << "If no paths are given, implicit path '.' is used.\n\n";
    os << opts_desc << '\n';
    os << "EXAMPLE:\n\t" << PROGRAM_NAME << " --recurse include/ src/ README.md" << '\n';
    return os;
}

list_owners_options parse(int argc, const char* argv[])
{
    list_owners_options options;

    po::options_description visible_desc("Options");
    visible_desc.add_options()("help", "Print help message")(
        "debug", po::bool_switch(&options.debug)->default_value(false),
        "Enable debug logging")("repo", po::value<boost::optional<fs::path>>(&options.repo_dir),
                                "Repository directory (default: search from current directory)")(
        "include-ignored", po::bool_switch(&options.include_ignored)->default_value(false),
        "Include files ignored by git");

    po::options_description opts_desc;
    opts_desc.add(visible_desc)
        .add_options()("input-files", po::value<std::vector<fs::path>>(&options.paths),
                       "Input files");

    po::positional_options_description pos_opts_desc;
    pos_opts_desc.add("input-files", -1);

    auto parser = po::command_line_parser(argc, argv).options(opts_desc).positional(pos_opts_desc);

    po::variables_map vm;
    try
    {
        po::store(parser.run(), vm);
    }
    catch (const boost::program_options::unknown_option& err)
    {
        print_help(std::cout, visible_desc) << std::flush;
        std::exit(EXIT_FAILURE);
    }
    po::notify(vm);

    if (vm.count("help"))
    {
        print_help(std::cout, visible_desc) << std::flush;
        std::exit(EXIT_SUCCESS);
    }

    return options;
}

int main(int argc, const char* argv[])
{
    fs::path current_path = fs::current_path();
    std::ostream& os = std::cout;

    list_owners_options options = parse(argc, argv);
    fs::path discovery_start = options.repo_dir.value_or(current_path);

    const std::optional<co::repository> maybe_repo = co::repository::try_discover(discovery_start);

    if (!maybe_repo)
    {
        os << "No repo discovered from: " << discovery_start << '\n';
        return EXIT_SUCCESS;
    }
    assert(maybe_repo);

    const co::repository& repo = *maybe_repo;
    const fs::path work_dir = repo.work_directory();
    auto maybe_co_path = co::codeowners_path(work_dir);
    if (!maybe_co_path)
    {
        os << "No CODEOWNERS file found; repo work directory: " << repo.work_directory() << '\n';
        return EXIT_SUCCESS;
    }
    assert(maybe_co_path);

    const co::ruleset ruleset{co::parse(*maybe_co_path)};

    // TODO: parse rules and perform matching of paths.

    std::vector<fs::path>& paths = options.paths;
    if (paths.empty())
        paths.push_back(current_path);

    std::vector<fs::path> to_skip = nonwork_directories(repo);
    for (const auto& start_path : paths)
    {
        for (const auto& path : co::make_filtered_file_range(start_path, to_skip))
        {
            const fs::path rel_path = fs::relative(path, work_dir);
            os << fs::relative(path, current_path).c_str() << ":    ";

            const auto matched_rule = ruleset.apply(rel_path);
            if (matched_rule)
            {
                os << matched_rule->rule.owners.front();
            }
            else
            {
                os << "[NO_OWNER]";
            }
            os << '\n';
        }
    }

    return EXIT_SUCCESS;
}