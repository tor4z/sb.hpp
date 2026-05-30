#define SB_IMPLEMENTATION
#include "sb.hpp"


int main(int argc, char** argv)
{
    sb::auto_rebuild_self(argc, argv);

    auto exe = sb::create_exe("00_exe")
        .set_compiler("clang++-22")
        .add_srcs("main.cpp")
        .add_flags("--target=aarch64-linux-gnu")
        .build();

    if (exe.status() == 0) {
        return sb::run_command({"scp", exe.name, "tor4z@192.168.1.131:~"});
    }
    return 1;
}
