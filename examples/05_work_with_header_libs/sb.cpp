#define SB_IMPLEMENTATION
#include "sb.hpp"

int main(int argc, char** argv)
{
    sb::auto_rebuild_self(argc, argv);

    auto exe = sb::create_exe("05_exe")
        .add_srcs("main.cpp", "llm.cpp")
        .add_flags("--target=aarch64-linux-gnu")
        .set_compiler("clang++-22")
        .build();

    if (exe.status() == 0) {
        std::cout << "Push bin file..\n";
        return sb::run_command("scp", exe.name.c_str(), "tor4z@192.168.1.131:~", nullptr);
    }
}
