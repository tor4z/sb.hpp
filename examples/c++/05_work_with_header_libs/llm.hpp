#ifndef LLM_H_
#define LLM_H_

#include <string>

namespace httplib
{
class Client;
}

class LLM
{
public:
    explicit LLM(const std::string& http_addr);
    ~LLM();
    std::string completion(const std::string& prompt, int n_predict);
private:
    httplib::Client* remote_llm_;
}; // class LLM

#endif // LLM_H_
