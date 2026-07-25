#define SB_IMPLEMENTATION
#include "sb.hpp"


int main(int argc, char **argv)
{
    sb::auto_rebuild_self(argc, argv);
    sb::set_build_dir("build");

    auto testting = sb::create_exe("testing")
        .add_srcs("main.cpp")
        .add_flags("-g")
        .set_compiler("clang++")
        .build();
    
    if (testting.status() == 0) {
        sb::run_command(testting.path().c_str(), nullptr);
    }
    return 0;
}
