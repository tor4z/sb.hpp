#include "llm.hpp"
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

// Set headers
const httplib::Headers common_headers = {
    { "Content-Type", "application/json" },
    { "Accept", "application/json" }
};

std::string construct_request(const std::string& prompt, int n_predict)
{
    json j;
    j["prompt"] = prompt;
    j["n_predict"] = n_predict;
    return j.dump();
}

LLM::LLM(const std::string& http_addr)
{
    remote_llm_ = new httplib::Client(http_addr);
}

LLM::~LLM()
{
    delete remote_llm_;
}

std::string LLM::completion(const std::string& prompt, int n_predict)
{
    if (!remote_llm_) {
        return "";
    }

    const auto req_body = construct_request(prompt, n_predict);
    const auto res = remote_llm_->Post("/completion", common_headers, req_body, "application/json");

    if (res) {
        auto res_json = json::parse(res->body);
        return res_json["content"];
    } else {
        auto err = res.error();
        std::cerr << "Request failed: " << static_cast<int>(err) << "\n";
    }
    return "";
}
