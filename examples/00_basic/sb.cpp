#define SB_IMPLEMENTATION
#include "sb.hpp"


int main(int argc, char** argv)
{
    sb::auto_rebuild_self(argc, argv);

    auto exe = sb::create_exe("00_exe")
        .set_compiler("clang++")
        .add_srcs("main.cpp")
        .build();
    return 1;
}
