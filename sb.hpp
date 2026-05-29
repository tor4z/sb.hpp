#ifndef SB_HPP_
#define SB_HPP_

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/stat.h>

namespace sb
{

enum Lib
{
    DYNAMIC = 0,
    STATIC,
};

class Instance
{
public:
    Instance& set_compiler(const std::string& compiler);
    Instance& add_src(const std::string& file);
    Instance& add_flag(const std::string& flag);
    Instance& build();
    int status() const;

    friend Instance create_lib(const std::string& name);
    friend Instance create_exe(const std::string& name);

    const std::string name;
private:
    enum TargetType
    {
        EXECUTABLE,
        LIBRARY,
    }; // enum TargetType

    Instance(const std::string& name, TargetType tt);

    std::vector<std::string> srcs_;
    std::vector<std::string> flags_;
    std::string compiler_;
    const TargetType tt_;
    int status_;
}; // class Instance

bool auto_rebuild_self__(int argc, char** argv, const char* src);
int run_command(const std::vector<std::string>& cmd);
Instance create_lib(const std::string& name);
Instance create_exe(const std::string& name);

#define auto_rebuild_self(argc, argv) auto_rebuild_self__(argc, argv, __FILE__)
} // namespace sb

#endif // SB_HPP_

#define SB_IMPLEMENTATION

#ifdef SB_IMPLEMENTATION

namespace sb
{

int copy_file(const char *src, const char *dest);

bool auto_rebuild_self__(int argc, char** argv, const char* src)
{
    if (!argv || argc == 0) {
        std::cerr << "Invalid argc argv.\n";
        return false;
    }

    const uint64_t s_to_ns = 1000 * 1000 * 1000;
    uint64_t bin_mtime;
    uint64_t src_mtime;
    struct stat st;

    const char* tmp_suffix = ".tmp";
    bool is_tmp = strstr(argv[0], tmp_suffix) != NULL;
    char sb_name[128];
    strncpy(sb_name, argv[0], 128);
    if (is_tmp) {
        sb_name[strlen(argv[0]) - strlen(tmp_suffix)] = '\0';
    }

    if (stat(sb_name, &st) == 0) {
        bin_mtime = st.st_mtim.tv_nsec + st.st_mtim.tv_sec * s_to_ns;
    } else {
        perror("Error bin file stats");
        return false;
    }

    if (stat(src, &st) == 0) {
        src_mtime = st.st_mtim.tv_nsec + st.st_mtim.tv_sec * s_to_ns;
    } else {
        perror("Error src file stats");
        return false;
    }

    if (src_mtime > bin_mtime) {
        // rebuild self
        if (is_tmp) {
            std::cout << "Rebuilding self ..\n";
            if (run_command({"c++", src, "-o", sb_name}) == 0) {
                exit(run_command({sb_name}));
            }
        }
        std::string tmp_path = argv[0];
        tmp_path += tmp_suffix;
        copy_file(argv[0], tmp_path.c_str());
        if (chmod(tmp_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
            perror("Failed to set executable permissions");
            return false;
        }
        exit(run_command({tmp_path}));
    }
    return true;
}

int run_command(const std::vector<std::string>& cmd)
{
    if (cmd.empty()) {
        return -1;
    }

    const char* cmd_arr[cmd.size() + 1];
    for (size_t i = 0; i < cmd.size(); ++i) {
        cmd_arr[i] = cmd.at(i).c_str();
    }
    cmd_arr[cmd.size()] = nullptr;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    int status = 0;
    if (pid == 0) {
        execvp(cmd_arr[0], (char*const*)cmd_arr);
        // only reach on error
        perror("execvp:");
    } else {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
    }

    return status;
}


Instance::Instance(const std::string& name, TargetType tt)
    : compiler_("c++")
    , name(name)
    , tt_(tt)
    , status_(0)
{}

Instance create_lib(const std::string& name)
{
    return Instance(name, Instance::LIBRARY);
}

Instance create_exe(const std::string& name)
{
    return Instance(name, Instance::EXECUTABLE);
}

Instance& Instance::add_src(const std::string& file)
{
    srcs_.push_back(file);
    return *this;
}

Instance& Instance::build()
{
    std::vector<std::string> cmd;
    cmd.push_back(compiler_);
    cmd.push_back(srcs_.at(0));
    for (const auto& f: flags_) {
        cmd.push_back(f);
    }
    cmd.push_back("-o");
    cmd.push_back(name);

    status_ = run_command(cmd);
    return *this;
}

int Instance::status() const
{
    return status_;
}

Instance& Instance::add_flag(const std::string& flag)
{
    flags_.push_back(flag);
    return *this;
}

Instance& Instance::set_compiler(const std::string& compiler)
{
    compiler_ = compiler;
    return *this;
}

int copy_file(const char *src, const char *dest) {
    FILE *src_file = fopen(src, "rb");
    if (!src_file) return -1;

    FILE *dest_file = fopen(dest, "wb");
    if (!dest_file) {
        fclose(src_file);
        return -1;
    }

    char buffer[8192]; // 8KB buffer size
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);
    return 0;
}

} // namespace sb

#endif // SB_IMPLEMENTATION
