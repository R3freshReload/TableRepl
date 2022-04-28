//
// Created by Daniel Seifert on 27.04.22.
//

#pragma once

#include <sstream>
#include <vector>

namespace util {
    static inline void fillRight(std::string& str, uint16_t width) {
        if (width <= str.size()) return;
        str.reserve(width);
        for (std::size_t i = str.size(); i < width; ++i) {
            str += ' ';
        }
    }
}

class PrettyTable {
private:
    std::vector<std::vector<std::string>> rows;
    std::vector<uint16_t> widths;

#pragma region Member Function Interface
private:
    void _addRow(int) {}

    template<typename T, typename... Args>
    void _addRow(int depth, T val, Args... args) {
        std::stringstream ss;
        ss << val;
        auto str = ss.str();
        if (widths.size() <= depth) {
            widths.emplace_back(str.length());
        }
        else if (widths[depth] < str.length())
            widths[depth] = str.length();
        rows.back().push_back(str);

        _addRow(depth+1, args...);
    }

public:
    template<typename... Args>
    void addRow(Args... args) {
        rows.emplace_back();
        _addRow(0, args...);
    }

    [[nodiscard]] std::string str() const {
        std::string res;
        uint16_t sum = 0;

        if (rows.empty()) return {};

        for (int i = 0; i < rows[0].size(); ++i) {
            std::string tmp = rows[0][i];
            auto width = widths[i]+2;
            sum += width;
            util::fillRight(tmp, width);
            res += tmp;
        }
        res += '\n';
        res += std::string(sum, '-');
        res += '\n';

        for (std::size_t i = 1; i < rows.size(); ++i) {
            const auto& row = rows[i];
            for (int col = 0; col < row.size(); ++col) {
                std::string tmp = row[col];
                util::fillRight(tmp, widths[col]+2);
                res += tmp;
            }
            res += '\n';
        }

        return res;
    }

    [[nodiscard]] std::vector<std::vector<std::string>> getData() const {
        return rows;
    }
#pragma endregion

#pragma region Stream Operator Interface
private:
    uint64_t cursor = 0;

    template<typename T>
    void addCell(T val) {
        std::stringstream ss;
        ss << val;
        auto str = ss.str();
        if (widths.size() <= cursor) {
            widths.emplace_back(str.length());
        }
        else if (widths[cursor] < str.length())
            widths[cursor] = str.length();
        rows.back().push_back(str);
        ++cursor;
    }

    void newRow() {
        rows.emplace_back();
        cursor = 0;
    }
public:

    struct row{};

    template<typename T>
    PrettyTable& operator<<(T arg) {
        if (rows.empty()) {
            rows.emplace_back();
        }
        if constexpr(std::is_same_v<T, row>) {
            newRow();
        } else {
            addCell(arg);
        }

        return *this;
    }

    PrettyTable& operator<<(const PrettyTable& arg) {

        return *this;
    }

#pragma endregion
};

namespace std {
    constexpr PrettyTable::row row;
}