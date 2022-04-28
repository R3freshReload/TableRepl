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
    struct row{};
}

namespace std {
    constexpr util::row row;
}

class PrettyTable;

class TableRow {
private:
    std::vector<std::string> data;
public:
    friend class PrettyTable;
};

class PrettyTable {
private:
    std::vector<TableRow> rows;
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
        rows.back().data.push_back(str);

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

        for (int i = 0; i < rows[0].data.size(); ++i) {
            std::string tmp = rows[0].data[i];
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
            for (int col = 0; col < row.data.size(); ++col) {
                std::string tmp = row.data[col];
                util::fillRight(tmp, widths[col]+2);
                res += tmp;
            }
            res += '\n';
        }

        return res;
    }

    [[nodiscard]] std::vector<TableRow> getData() const {
        return rows;
    }
#pragma endregion

#pragma region Stream Operator Interface
private:
    uint64_t cursor = 0;

    template<typename T>
    void addCell(T val) {
        if (rows.empty()) {
            rows.emplace_back();
        }

        std::stringstream ss;
        ss << val;
        auto str = ss.str();
        if (widths.size() <= cursor) {
            widths.emplace_back(str.length());
        }
        else if (widths[cursor] < str.length())
            widths[cursor] = str.length();
        rows.back().data.push_back(str);
        ++cursor;
    }

    void newRow() {
        rows.emplace_back();
        cursor = 0;
    }
public:

    template<typename T>
    PrettyTable& operator<<(T arg) {
        addCell(arg);
        return *this;
    }

    PrettyTable& operator<<(util::row) {
        newRow();
        return *this;
    }

    PrettyTable& operator<<(const PrettyTable& arg) {
        for (const auto& row : arg.getData()) {
            for (const auto& data : row.data) {
                *this << data;
            }
            *this << std::row;
        }
        return *this;
    }

#pragma endregion
};