//
// Created by Daniel Seifert on 27.04.22.
//

#include "table/PrettyTable.h"
#include <iostream>
#include <unordered_map>

namespace table  {
    std::unordered_map<std::string, PrettyTable> tables{{"0", PrettyTable{}}};
    std::string selected = "0";

    void processInput(PrettyTable& t, std::string input) {
        if (input.empty())
            return;

        if (input.starts_with(".")) {
            input = input.substr(1);
            if (input.empty()) {
                std::cout << t.str() << std::endl;
            } else {
                std::cout << tables[input].str() << std::endl;
            }
        } else if (input.starts_with(">")) {
            input = input.substr(1);
            std::string name;
            for (std::size_t i = 0; i < input.length(); ++i) {
                if (input[i] == '>') {
                    name = input.substr(0, i);
                    input = input.substr(i+1);
                    break;
                } else if (input[i] == '<') {
                    name = input.substr(i+1);
                    input = input.substr(0, i);
                    break;
                }
            }
            if (!input.empty() && name.empty()) {
                selected = input;
            } else if(!name.empty()) {
                tables[input] << tables[name];
            }
        } else if (input.starts_with("#")) {
            t << std::row;
        } else {
            t << input;
        }
    }

    void runRepl() {
        std::string input;

        std::cout << selected << ">> ";
        while(getline(std::cin, input)) {
            table::processInput(tables[selected], input);
            std::cout << selected << ">> ";
        }
    }
}

int main() {
    table::runRepl();
}