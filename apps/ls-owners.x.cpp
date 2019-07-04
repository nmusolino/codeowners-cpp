#include <codeowners/codeowners.hpp>
#include <codeowners/paths.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>

namespace po = boost::program_options;

constexpr const char* PROGRAM_NAME = "ls-owners";

struct list_owners_options {
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
    list_owners_options retval;

    po::variables_map vm;

    po::options_description visible_desc("Options");
    visible_desc.add_options()
        ("help",
            "Print help message")
        ("repo", po::value<boost::optional<fs::path>>(&retval.repo_dir),
            "Repository directory (default: search from current directory)")
        ("include-ignored", po::bool_switch(&retval.include_ignored)->default_value(false),
            "Include files ignored by git")
        ;

    po::options_description opts_desc;
    opts_desc.add(visible_desc).add_options()
        ("input-files", po::value<std::vector<fs::path>>(&retval.paths), "Input files");
    po::positional_options_description pos_opts_desc;
    pos_opts_desc.add("input-files", -1);

    auto parser = po::command_line_parser(argc, argv).options(opts_desc).positional(pos_opts_desc);
    try {
        po::store(parser.run(), vm);
    } catch (const boost::program_options::unknown_option& err)
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

    return retval;
}


int main(int argc, const char* argv[])
{
    fs::path current_path = fs::current_path();

    list_owners_options options = parse(argc, argv);
    std::vector<fs::path>& paths = options.paths;
    if (paths.empty())
        paths.push_back(current_path);

    co::repository repo{options.repo_dir.value_or(current_path)};


    for (const auto& path : repo.index_paths()) {
        std::cout << path.c_str() << '\n';
    }
    return EXIT_SUCCESS;
}