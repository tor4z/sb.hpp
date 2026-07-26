#include <vector>
#define SB_IMPLEMENTATION
#include "sb.hpp"


int main(int argc, char **argv)
{
    sb::auto_rebuild_self(argc, argv);
    sb::set_build_dir("build");

    auto& clean = sb::flags_arg("clean", false, "clean generated binary files");
    auto& rebuild = sb::flags_arg("-rebuild", false, "rebuild all target");
    auto& run = sb::flags_arg("-run", false, "run the program when build is success");
    auto& jobs = sb::flags_arg("-j", 1, "num jobs to build");
    auto& vstr = sb::flags_arg("-vstr", std::vector<std::string>{}, "string list");
    auto& vi = sb::flags_arg("-vi", std::vector<int>{}, "string list");
    auto& vf = sb::flags_arg("-vf", std::vector<float>{}, "string list");
    auto& help = sb::flags_arg("--help", false, "Show this help info");
    if (!sb::flags_parse(argc, argv) || help) {
        sb::flags_show_usage();
        return 0;
    }

    std::cout << "clean: " << clean << "\n";
    std::cout << "rebuild: " << rebuild << "\n";
    std::cout << "jobs: " << jobs << "\n";
    std::cout << "--help: " << help << "\n";
    std::cout << "-vsr: ";
    for (const auto& s : vstr) {
        std::cout << s << " ";
    }
    std::cout << "\n";

    std::cout << "-vi: ";
    for (const auto& s : vi) {
        std::cout << s << " ";
    }
    std::cout << "\n";

    std::cout << "-vf: ";
    for (const auto& s : vf) {
        std::cout << s << " ";
    }
    std::cout << "\n";
    // auto flags = sb::create_exe("flags")
    //     .add_srcs("main.cpp")
    //     .set_compiler("clang++")
    //     .always_build(rebuild)
    //     .build();

    // if (run && flags.status() == 0) {
    //     sb::run_command(flags.path().c_str(), nullptr);
    // }

    // return flags.status();
}
