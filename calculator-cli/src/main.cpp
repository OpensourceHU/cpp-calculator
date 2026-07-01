#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <string>
#include <vector>

std::string escape_json(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "{\"status\":\"error\",\"message\":\"Usage: calculator-cli \\\"<expression>\\\"\"}" << std::endl;
        return 1;
    }

    std::string input = argv[1];

    try {
        Lexer lexer(input);
        std::vector<Token> tokens = lexer.tokenize();
        Parser parser(tokens);
        double result = parser.parse();

        // Output results in structured JSON format
        std::cout << "{\"status\":\"success\",\"result\":" << result << "}" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "{\"status\":\"error\",\"message\":\"" << escape_json(e.what()) << "\"}" << std::endl;
        return 0; // Return 0 so caller gets the JSON instead of process crash signal
    }

    return 0;
}
