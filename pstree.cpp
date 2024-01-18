#include <cassert>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <format>
#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

auto main(int argc, char* argv[]) -> int {
    for (int i = 0; i < argc; i ++) {
        assert(argv[i] != nullptr);
        // std::cout << std::format("argv[{}] = {}\n", i, argv[i]);
    }
    assert(argv[argc] == nullptr);

    std::filesystem::path proc_dir_path = "/proc";
    assert(std::filesystem::exists(proc_dir_path));

    std::unordered_map<std::uint64_t, std::vector<std::uint64_t>> pid_adj{};
    std::unordered_map<std::uint64_t, std::string> pid_name{};

    auto string_to_pid = [](const std::string &file_name, std::uint64_t &pid_result) -> bool {
        pid_result = 0;
        for (auto ch : file_name) {
            if (ch >= '0' && ch <= '9') {
                pid_result = pid_result * 10 + (ch - '0');
            } else {
                return false;
            }
        }
        return true;
    };

    for (const auto &entry : std::filesystem::directory_iterator(proc_dir_path) 
        | std::views::filter([](const auto &entry) { 
                return entry.status().type() == std::filesystem::file_type::directory 
                    && std::filesystem::exists(entry.path() / "stat"); 
            })
    ) {
        auto file_name = entry.path().filename().string();
        std::uint64_t pid;
        if (!string_to_pid(file_name, pid)) {
            continue;
        }

        auto stat_stream = std::ifstream(entry.path() / "stat");

        std::string _, parent_pid_str, pid_name_str;
        stat_stream >> _ >> pid_name_str >> _ >> parent_pid_str;

        std::uint64_t parent_pid;
        if (!string_to_pid(parent_pid_str, parent_pid)) {
            continue;
        }

        pid_name[pid] = pid_name_str;
        pid_adj[parent_pid].emplace_back(pid);
    }

    auto dump_print = [&](auto &self, std::uint64_t current_pid, std::string indent = "", bool is_last = true) -> void {
        const auto marker = is_last ? "└───" : "├───";
        std::cout << std::format("{}{}{}({})\n", indent, marker, pid_name[current_pid], current_pid);
        
        if (!pid_adj.contains(current_pid)) {
            return;
        }

        indent += is_last ? "    " : "│   ";
        const auto &next = pid_adj[current_pid];
        for (std::size_t i = 0; i < next.size(); i ++) {
            self(self, next[i], indent, i + 1 == next.size());
        }
    };

    constexpr std::uint64_t init_pid = 1;
    dump_print(dump_print, init_pid);
}
