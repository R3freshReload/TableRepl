//
// Created by Daniel Seifert on 27.04.22.
//

#pragma once

#include <sstream>
#include <utility>
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
class TableRow;

class TableCell {
private:
    std::size_t width;
    std::string value;
    std::size_t col;
public:
    TableCell(std::size_t col, std::string value) : col{col}, value{std::move(value)} {
        width = this->value.size();
    }

    TableCell& operator=(std::string newVal) {
        value = std::move(newVal);
        width = value.size();
        return *this;
    }

    [[nodiscard]] std::size_t length() const {
        return width;
    }

    bool operator==(const TableCell& ref) const {
        return value == ref.value;
    }

    explicit operator std::string() const {
        return value;
    }
};

class TableRow {
private:
    std::vector<TableCell> data;
    std::size_t row;
public:
    TableRow(std::size_t row) : row{row} {}

    TableCell& emplace_back(const std::string& val) {
        return data.emplace_back(data.size(), val);
    }

    TableCell& operator[](std::size_t idx) {
        return data[idx];
    }

    const TableCell& operator[](std::size_t idx) const {
        return data[idx];
    }

    bool operator==(const TableRow& ref) const {
        return data == ref.data;
    }

    [[nodiscard]] std::size_t size() const {
        return data.size();
    }

    std::vector<TableCell>::iterator begin() {
        return data.begin();
    }

    std::vector<TableCell>::iterator end() {
        return data.end();
    }

    [[nodiscard]] std::vector<TableCell>::const_iterator begin() const {
        return data.begin();
    }

    [[nodiscard]] std::vector<TableCell>::const_iterator end() const {
        return data.end();
    }
};

class PrettyTable {
private:
    std::vector<TableRow> rows;

public:
    PrettyTable() = default;
    PrettyTable(const PrettyTable& rhs) {
        *this << rhs;
    }

#pragma region iterators
    std::vector<TableRow>::iterator begin() {
        return rows.begin();
    }

    std::vector<TableRow>::iterator end() {
        return rows.end();
    }

    [[nodiscard]] std::vector<TableRow>::const_iterator begin() const {
        return rows.begin();
    }

    [[nodiscard]] std::vector<TableRow>::const_iterator end() const {
        return rows.end();
    }
#pragma endregion

#pragma region Member Function Interface
private:
    void _addRow(int) {}

    template<typename T, typename... Args>
    void _addRow(int depth, T val, Args... args) {
        std::stringstream ss;
        ss << val;
        auto str = ss.str();
        rows.back().emplace_back(str);

        _addRow(depth+1, args...);
    }

    [[nodiscard]] std::vector<std::size_t> get_widths() const {
        std::vector<std::size_t> res;
        for (const auto& row : rows) {
            for (std::size_t col = 0; col < row.size(); ++col) {
                if (res.size() <= col) {
                    res.emplace_back(row[col].length());
                }
                else if (res[col] < row[col].length())
                    res[col] = row[col].length();
            }
        }
        return res;
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
        auto widths = get_widths();

        for (const auto& width : widths) {
            sum += width+2;
        }

        if (rows.empty()) return {};

        for (int i = 0; i < rows[0].size(); ++i) {
            std::string tmp = (std::string)(rows[0][i]);
            auto width = widths[i]+2;
            util::fillRight(tmp, width);
            res += tmp;
        }
        res += '\n';
        res += std::string(sum, '-');
        res += '\n';

        for (std::size_t i = 1; i < rows.size(); ++i) {
            const auto& row = rows[i];
            for (int col = 0; col < row.size(); ++col) {
                std::string tmp = (std::string)row[col];
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
            newRow();
        }

        std::stringstream ss;
        ss << val;
        auto str = ss.str();
        rows.back().emplace_back(str);
        ++cursor;
    }

    void newRow() {
        rows.emplace_back(rows.size());
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
            for (const auto& data : row) {
                *this << (std::string)data;
            }
            *this << std::row;
        }
        return *this;
    }

    PrettyTable& operator<<(const TableRow& row) {
        for (const auto& data : row) {
            *this << (std::string)data;
        }
        *this << std::row;
        return *this;
    }

#pragma endregion

    TableRow& operator[](std::size_t x) {
        return rows[x];
    }

#pragma region Set Operators

    PrettyTable operator-(const PrettyTable& rhs) const {
        PrettyTable res(*this);
        auto torem = std::remove_if(res.rows.begin(), res.rows.end(),
                       [&rhs](const TableRow& i) { return rhs.contains(i); });
        res.rows.erase(torem, res.rows.end());
        return res;
    }

    PrettyTable operator+(const PrettyTable& rhs) const {
        PrettyTable res(*this);
        res << rhs;
        return res;
    }

    PrettyTable& operator-=(const PrettyTable& rhs) {
        auto torem = std::remove_if(rows.begin(), rows.end(),
                                    [&rhs](const TableRow& i) { return rhs.contains(i); });
        rows.erase(torem, rows.end());
        return *this;
    }

    PrettyTable& operator+=(const PrettyTable& rhs) {
        *this << rhs;
        return *this;
    }

    PrettyTable operator&(const PrettyTable& rhs) const {
        PrettyTable res;
        for (const auto& tr : rhs) {
            if (contains(tr))
                res << tr;
        }
        for (const auto& tr : *this) {
            if (rhs.contains(tr) && !res.contains(tr))
                res << tr;
        }
        return res;
    }

    PrettyTable operator^(const PrettyTable& rhs) const {
        PrettyTable res(*this);
        res += rhs;
        res -= rhs & *this;
        return res;
    }
#pragma endregion

    [[nodiscard]] bool contains(const TableRow& row) const {
        return std::any_of(rows.begin(), rows.end(), [&row](const TableRow& p){return p==row;});
    }
};