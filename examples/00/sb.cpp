#define SB_IMPLEMENTATION
#include "sb.hpp"

#include <fcntl.h>
#include <unistd.h>



int main(int argc, char** argv)
{
    sb::auto_rebuild_self(argc, argv);

    auto exe = sb::create_exe("00_exe")
        .set_compiler("clang++-22")
        .add_src("main.cpp")
        .add_flag("--target=aarch64-linux-gnu")
        .build();
    if (exe.status() == 0) {
        return sb::run_command({"scp", exe.name, "tor4z@192.168.1.131:~"});
    }
    return 1;
}
