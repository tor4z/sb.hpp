#include <iostream>
#include "llm.hpp"

int main(int argc, char** argv)
{
    LLM llm("http://192.168.1.103:8080");
    auto res = llm.completion("Building an API is", 64);
    std::cout << "AI response: " << res << "\n";
    return 0;
}
