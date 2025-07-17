#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <stack>

using namespace std;

class TACGenerator {
private:
    vector<string> tacInstructions;
    int tempCounter = 0;
    int labelCounter = 0;

    map<string, string> symbolTable;
    map<string, string> literalTable;
    map<string, string> keywordTable;

    stack<string> ifLabelStack;
    stack<pair<string, string>> whileLabelStack;

    ofstream tacFile;

public:
    TACGenerator() {
        loadTable("SymbolTable.txt", symbolTable, 's');
        loadTable("LiteralTable.txt", literalTable, 'l');
        loadKeywordTable("KeywordTable.txt");

        tacFile.open("tac.txt");
        if (!tacFile.is_open()) {
            cerr << "Error: Could not open tac.txt for writing" << endl;
        }
    }

    ~TACGenerator() {
        if (tacFile.is_open()) tacFile.close();
    }

    void loadTable(const string& filename, map<string, string>& table, char prefix) {
        ifstream file(filename);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            size_t start = line.find('<') + 1;
            size_t mid = line.find(',');
            size_t end = line.find('>');
            if (start == string::npos || mid == string::npos || end == string::npos) continue;

            string id = line.substr(start, mid - start);
            string val = line.substr(mid + 2, end - mid - 2);
            table[prefix + id] = val;
        }
    }

    void loadKeywordTable(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            size_t start = line.find('<') + 1;
            size_t mid = line.find(',');
            size_t end = line.find('>');
            if (start == string::npos || mid == string::npos || end == string::npos) continue;

            string code = line.substr(start, mid - start);
            string word = line.substr(mid + 2, end - mid - 2);
            keywordTable[code] = word;
        }
    }

    string newTemp() {
        return "t" + to_string(tempCounter++);
    }

    string newLabel() {
        return "L" + to_string(labelCounter++);
    }

    string getValue(const string& tokenType, const string& tokenValue) {
        if (tokenType[0] == 's') return symbolTable.count(tokenType) ? symbolTable[tokenType] : tokenValue;
        if (tokenType[0] == 'l') return literalTable.count(tokenType) ? literalTable[tokenType] : tokenValue;
        if (tokenType[0] == 'k') return keywordTable.count(tokenType) ? keywordTable[tokenType] : tokenValue;
        return tokenValue;
    }

    void generateFromTokens(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open token file" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            vector<pair<string, string>> tokens = parseLine(line);

            for (size_t i = 0; i < tokens.size(); ++i) {
                string type = tokens[i].first;
                string val = tokens[i].second;

                string word = getValue(type, val);

                if (type[0] == 'k') {
                    if (word == "Adadi") {
                        string varName = getValue(tokens[i + 1].first, tokens[i + 1].second);
                        tacInstructions.push_back("DECLARE int Adadi " + varName);
                        i++;
                    }
                    else if (word == "Agar") {
                        string lhs = getValue(tokens[i + 2].first, tokens[i + 2].second);
                        string op = getValue(tokens[i + 3].first, tokens[i + 3].second);
                        string rhs = getValue(tokens[i + 4].first, tokens[i + 4].second);
                        tacInstructions.push_back("cond = " + lhs + " " + op + " " + rhs);
                        string falseLabel = newLabel();
                        tacInstructions.push_back("IF_FALSE condition Agar GOTO " + falseLabel);
                        ifLabelStack.push(falseLabel);
                        i += 4;
                    }
                    else if (word == "while") {
                        string startLabel = newLabel();
                        string endLabel = newLabel();
                        string lhs = getValue(tokens[i + 2].first, tokens[i + 2].second);
                        string op = getValue(tokens[i + 3].first, tokens[i + 3].second);
                        string rhs = getValue(tokens[i + 4].first, tokens[i + 4].second);
                        tacInstructions.push_back(startLabel + ":");
                        tacInstructions.push_back("cond = " + lhs + " " + op + " " + rhs);
                        tacInstructions.push_back("IF_FALSE condition GOTO " + endLabel);
                        whileLabelStack.push(make_pair(startLabel, endLabel));
                        i += 4;
                    }
                    else if (word == "output<-") {
                        string var = getValue(tokens[i + 1].first, tokens[i + 1].second);
                        tacInstructions.push_back("OUTPUT " + var);
                        i++;
                    }
                    else if (word == "Wapas") {
                        string val = getValue(tokens[i + 1].first, tokens[i + 1].second);
                        tacInstructions.push_back("RETURN Wapas " + val);
                        i++;
                    }
                }
                else if (type == "op1" && val == "=") {
                    string lhs = getValue(tokens[i - 1].first, tokens[i - 1].second);
                    string rhs1 = getValue(tokens[i + 1].first, tokens[i + 1].second);

                    if (i + 2 < tokens.size() && tokens[i + 2].first == "op3") {
                        string op = getValue(tokens[i + 2].first, tokens[i + 2].second);
                        string rhs2 = getValue(tokens[i + 3].first, tokens[i + 3].second);
                        string temp = newTemp();
                        tacInstructions.push_back(temp + " = " + rhs1 + " " + op + " " + rhs2);
                        tacInstructions.push_back("ASSIGN " + lhs + " = " + temp);
                        i += 3;
                    }
                    else {
                        tacInstructions.push_back("ASSIGN " + lhs + " = " + rhs1);
                        i++;
                    }
                }
                else if (type == "p5") {
                    if (!ifLabelStack.empty()) {
                        tacInstructions.push_back(ifLabelStack.top() + ":");
                        ifLabelStack.pop();
                    }
                    else if (!whileLabelStack.empty()) {
                        pair<string, string> labels = whileLabelStack.top();
                        string startLabel = labels.first;
                        string endLabel = labels.second;
                        tacInstructions.push_back("GOTO " + startLabel);
                        tacInstructions.push_back(endLabel + ":");
                        whileLabelStack.pop();
                    }
                }
                else if (type[0] == 's') {
                    tacInstructions.push_back("VAR " + getValue(type, val));
                }
                else if (type[0] == 'l') {
                    tacInstructions.push_back("CONST " + getValue(type, val));
                }
            }
        }

        for (size_t i = 0; i < tacInstructions.size(); ++i) {
            tacFile << tacInstructions[i] << endl;
        }

        file.close();
    }

    vector<pair<string, string>> parseLine(const string& line) {
        vector<pair<string, string>> tokens;
        stringstream ss(line);
        string token;
        while (getline(ss, token, '\t')) {
            size_t start = token.find('<') + 1;
            size_t mid = token.find(',');
            size_t end = token.find('>');
            if (start == string::npos || mid == string::npos || end == string::npos) continue;
            string type = token.substr(start, mid - start);
            string val = token.substr(mid + 2, end - mid - 2);
            tokens.push_back(make_pair(type, val));
        }
        return tokens;
    }
};

int main() {
    cout << "Generating Three Address Code..." << endl;
    TACGenerator generator;
    generator.generateFromTokens("tok.txt");
    cout << "TAC generation complete. Output written to tac.txt" << endl;
    return 0;
}