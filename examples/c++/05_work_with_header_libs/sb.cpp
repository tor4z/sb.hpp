#include <vector>
#define SB_IMPLEMENTATION
#include "sb.hpp"

int main(int argc, char** argv)
{
    sb::auto_rebuild_self(argc, argv);

    auto exe = sb::create_elf("05_exe")
        .add_srcs(std::vector<std::string>{"main.cpp", "llm.cpp"})
        .set_compiler("clang++")
        .build();
}
