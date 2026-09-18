#define SB_IMPLEMENTATION
#include "sb.hpp"


int main(int argc, char **argv)
{
    sb::auto_rebuild_self(argc, argv);

    auto exe = sb::create_elf("hello")
        .add_srcs("main.c")
        .set_compiler("clang");

    auto ls = sb::create_phony("ls")
        .add_deps(exe)
        .add_flags("-l")
        .add_flags(exe.path())
        .build();

    return 0;
}
