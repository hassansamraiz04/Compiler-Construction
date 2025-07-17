#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#define ROWS 53  
#define COLS 27

int final[] = { 1,2,3,4,5,6,8,9,46,48,12,13,14,15,19,20,22,23,24,26,27,29,30,31,34,35,37,38,39,40,42,45,50 };
string transitionTable[ROWS][COLS];
string keyword[] = {
        "asm", "Wagarna", "new", "this", "auto", "enum", "operator", "throw", "Mantiqi", "explicit", "private",
        "True", "break", "export", "protected", "try", "case", "extern", "public", "typedef", "catch", "False", "register", "typeid",
        "Harf", "Ashriya", "typename", "Adadi", "class", "for", "Wapas", "union", "const", "dost", "short", "unsigned", "goto",
        "signed", "using", "continue", "Agar", "sizeof", "virtual", "default", "inline", "static", "Khali", "delete", "volatile", "do",
        "long", "struct", "double", "mutable", "switch", "while", "namespace", "template", "Marqazi", "Matn", "input->", "output<-"
};


char nextcharfetch(const string& input, int& position) {
    if (position < input.size()) {
        return input[position++];
    }
    return '\0';
}

bool iskeyword(string str) {
    int size = sizeof(keyword) / sizeof(keyword[0]);
    for (int i = 0; i < size; i++) {
        if (str == keyword[i]) {
            return true;
        }
    }
    return false;
}

bool isfinalstate(int state) {
    int size = sizeof(final) / sizeof(final[0]);
    for (int i = 0; i < size; i++) {
        if (state == final[i]) {
            return true;
        }
    }
    return false;
}

void lexicalanalyzer(const string& code) {
    int advance[ROWS][COLS];
    int mappingtable[ROWS][COLS];

    string alphabets[COLS];
    string states[ROWS];
    int count = 0;
    for (int j = 1; j < COLS; j++)
    {
        alphabets[count] = transitionTable[0][j];
        count++;
    }
    
    count = 0;
    for (int i = 1; i < ROWS; i++)
    {
        states[count] = transitionTable[i][0];
        count++;
    }

    ////mapping////

    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (transitionTable[0][i] == alphabets[j] && !transitionTable[0][i].empty())
            {
                mappingtable[0][i] = j;
                break;
            }
            else
            {
                mappingtable[0][i] = -9;
            }
        }
    }

    for (int i = 1; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
        {
            for (int z = 0; z < ROWS; z++) {
                if (transitionTable[i][j] == states[z] && !transitionTable[i][j].empty())
                {
                    mappingtable[i][j] = z;
                    break;
                }
                else
                {
                    mappingtable[i][j] = -9;
                }

            }
        }
    }

    //// advanced table
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || j == 0) {
                advance[i][j] = mappingtable[i][j];
            }
            else if (mappingtable[i][j] != -9)
            {
                advance[i][j] = 1;
            }
            else {
                advance[i][j] = -9;
            }
        }
    }


    int colm = 1;
    for (int j = 1; j < ROWS; j++) {
        if (mappingtable[j][colm] != -9) {
            advance[j][colm] = 0;
        }
    }



    ///files read///

    bool symbol1 = false;
    bool keyword1 = false;


    bool literal1 = false;

    ofstream symbol("symbol_tableL226625.txt");
    ofstream keyword("keyword_tableL226625.txt");
    ofstream literal("literal_tableL226625.txt");
    ofstream tokens("tokens_tableL226625.txt");
    ofstream error("error_tableL226625.txt");

    // Check if files are opened successfully
    if (!symbol.is_open() || !keyword.is_open() || !literal.is_open() || !tokens.is_open()) {
        cerr << "Error: Unable to open one or more files!" << endl;
        return;
    }

    // Write headers
    symbol << "Position\tValue\n";
    keyword << "Position\tValue\n";
    literal << "Position\tValue\n";
    tokens << "Token Table\n";


    int sympos = 1;
    int keypos = 1;
    int litpos = 1;


    ////main////


    vector<string> keywordchecking;
    vector<string> symbolchecking;

    bool inout = false;
    bool adv2 = false;
    bool numbe = false;

    int st = 0;
    int alph = 0;
    int pos = 0;
    string storestring = "";



    string input = code;
    char chr = '#';


    bool lit = false;
    int litsecond = 0;

    string literalValue = "\"";

    bool aterror = false;

    bool chother = false;

    bool literror = false;


    while (chr != '\0')
    {
        numbe = false;
        keyword1 = false;
        inout = false;
        aterror = false;
        literalValue = "";
        literal1 = false;
        st = 0;
        chr = nextcharfetch(input, pos);

        if (chr == '"') {
            litsecond++;
        }

        if (chr == '"' && litsecond % 2 != 0) {
            literror = true;
            lit = true;
            literalValue = "";

            while (lit) {
                chr = nextcharfetch(input, pos);
                cout << literalValue;
                if (chr == '"') {
                    lit = false;
                    break;
                }

                if (chr == '\0') {
                    cout << "error: String not closed" << endl;
                    error << "error could not find second \" in code \n";
                    lit = false;
                    break;
                }

                literalValue += chr;

                literal1 = true;
            }

            if (literal1) {
                literal << litpos << "\t\t" << literalValue << "\n";
                tokens << "< literal, " << litpos << " > ";
                litpos++;
            }
            chr = nextcharfetch(input, pos);
            litsecond++;
        }

        if (chr == '@')
        {
            error << "found @ in code which is not valid \n";
            chr = nextcharfetch(input, pos);
        }

        if (chr == '_') {
            symbol1 = true;
        }
        storestring = "";
        bool check = false;


        string ch;

        ch = "";
        ch = ch + chr;
        storestring += chr;
        for (int i = 0; i < COLS; i++)
        {
            if (ch == alphabets[i]) {
                alph = i;
                break;
            }
        }

        if (check == false)
        {
            if (chr >= '0' && chr <= '9') {
                ch = "num";
            }
            else if (chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z') {
                ch = "letter";
            }

            else if (mappingtable[st + 1][alph + 1] == -9 || chr == ' ') {
                ch = "other";
                chother = true;
            }

            if (iskeyword(storestring) == true) {
                ch = "keyword";
            }

            for (int i = 0; i < COLS; i++)
            {
                if (ch == alphabets[i]) {
                    alph = i;
                    check = true;
                }
            }
        }


        bool adv = false;
        adv2 = false;
        while (chr != '\0' && isfinalstate(st) != true && mappingtable[st + 1][alph + 1] != -9 && adv != true && lit == false && aterror == false && inout == false)
        {
            inout = false;
            check = false;
            adv = true;
            int newstate = mappingtable[st + 1][alph + 1];
            if (advance[st + 1][alph + 1] == 1)
            {
                bool check = false;
                chr = nextcharfetch(input, pos);


                if (chr == '"') {
                    litsecond++;
                }

                if (chr == '"' && litsecond % 2 != 0) {
                    literror = true;
                    lit = true;
                    literalValue = "";

                    while (lit) {
                        chr = nextcharfetch(input, pos);
                        cout << literalValue;
                        if (chr == '"') {
                            lit = false;
                            break;
                        }

                        if (chr == '\0') {
                            cout << "error: String not closed" << endl;
                            error << "error could not find second \" in code \n";
                            lit = false;
                            break;
                        }

                        literalValue += chr;

                        literal1 = true;
                    }

                    if (literal1) {
                        literal << litpos << "\t\t" << literalValue << "\n";
                        tokens << "< literal, " << litpos << " > ";
                        litpos++;
                    }
                    chr = nextcharfetch(input, pos);
                    litsecond++;
                }
                if (chr == '@')
                {
                    error << "found @ in code which error \n";
                    aterror = true;
                    break;
                }

                if (chr == '_') {
                    symbol1 = true;
                }
                ch = "";
                ch = ch + chr;

                storestring += chr;

                for (int i = 0; i < COLS; i++)
                {
                    if (ch == alphabets[i]) {
                        alph = i;
                        check = true;

                    }
                }

                if (check == false)
                {
                    if (chr >= '0' && chr <= '9') {
                        check = true;
                        ch = "num";
                    }
                    else if (chr >= 'a' && chr <= 'z' || chr >= 'A' && chr <= 'Z') {
                        check = true;
                        ch = "letter";
                    }

                    else if (mappingtable[st + 1][alph + 1] == -9 || chr == ' ') {
                        check = true;
                        ch = "other";
                        chother = true;


                    }

                    if (iskeyword(storestring) == true) {
                        check = true;
                        ch = "keyword";
                        keyword1 = true;
                    }


                    if (storestring == "input") {
                        char tempchr1 = nextcharfetch(input, pos);
                        char tempchr2 = nextcharfetch(input, pos);
                        if (tempchr1 == '-' && tempchr2 == '>')
                        {
                            storestring = storestring + tempchr1 + tempchr2;
                            ch = "keyword";
                            inout = true;
                            keyword1 = true;
                        }
                        else {
                            pos = pos - 2;
                        }

                    }

                    if (storestring == "output") {
                        char tempchr1 = nextcharfetch(input, pos);
                        char tempchr2 = nextcharfetch(input, pos);
                        if (tempchr1 == '-' && tempchr2 == '<')
                        {
                            storestring = storestring + tempchr1 + tempchr2;
                            ch = "keyword";
                            inout = true;
                            keyword1 = true;
                        }
                        else {
                            pos = pos - 2;
                        }

                    }



                    for (int i = 0; i < COLS; i++)
                    {
                        if (ch == alphabets[i]) {
                            alph = i;
                            check = true;
                        }
                    }
                }
                adv = false;


                st = newstate;


            }
            else
            {
                adv2 = true;
            }


        }

        if ((chr != ' ' && chother == true && inout != true) || adv2 == true && chr != ' ' && inout != true && lit == false) {
            pos = pos - 1;
            storestring.pop_back();
            cout << "error";
        }
        if (!storestring.empty() && storestring.back() == ' ') {
            storestring.pop_back();
        }


        if (storestring != " " && storestring != "\n" && storestring != "\r" && !storestring.empty() && lit == false) {
            if (iskeyword(storestring) || keyword1 == true) {

                int index = -1;

                for (size_t i = 0; i < keywordchecking.size(); i++) {
                    if (keywordchecking[i] == storestring) {
                        index = i;
                        break;
                    }
                }

                if (index == -1) {
                    keyword << keypos << "\t\t" << storestring << "\n";
                    tokens << "< keyword," << keypos << " > ";
                    keypos++;
                    keywordchecking.push_back(storestring);
                }
                else {
                    tokens << "< keyword," << index + 1 << " > ";
                }

            }

            else if (symbol1 == true) {

                int index = -1;

                for (size_t i = 0; i < symbolchecking.size(); i++) {
                    if (symbolchecking[i] == storestring) {
                        index = i;
                        break;
                    }
                }

                if (index == -1) {
                    symbol << sympos << "\t\t" << storestring << "\n";

                    tokens << "< " << "id" << "," << sympos << " > ";
                    sympos++;
                    symbolchecking.push_back(storestring);
                }
                else {
                    tokens << "< id," << index + 1 << " > ";
                }


            }
            else {
                tokens << "< " << storestring << " > ";
            }

        }



        symbol1 = false;
        literal1 = false;
        keyword1 = false;

    }
    symbol.close();
    literal.close();
    keyword.close();
    tokens.close();
    error.close();
}

void TTtableread(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file!\n";
        return;
    }

    string line;
    int row = 0;

    while (getline(file, line) && row < ROWS) {
        stringstream ss(line);
        string value;
        int col = 0;

        while (getline(ss, value, ',') && col < COLS) {
            transitionTable[row][col] = value;
            col++;
        }
        row++;
    }
    file.close();
}

void spacecheck(string& str) {
    string result;
    bool spaceFound = false;

    for (char c : str) {
        if (c == ' ') {
            if (!spaceFound) {
                result += c;
                spaceFound = true;
            }
        }
        else {
            result += c;
            spaceFound = false;
        }
    }

    str = result;
}

void convertendlintospace(string& str) {
    for (char& ch : str) {
        if (ch == '\n' || ch == '\r') {
            ch = ' '; 
        }
    }
}
void Meow(string& arr) {
    spacecheck(arr);
    convertendlintospace(arr);
    spacecheck(arr);
}

int main() {
    string filename = "TTfinalL226625.csv";
    TTtableread(filename);

    ifstream inputFile("practice codeL226625.txt");

    if (!inputFile) {
        cerr << "Error: Unable to open file!" << endl;
        return 1;
    }

    inputFile.seekg(0, ios::end);
    int fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    string arr(fileSize, '\0');
    inputFile.read(&arr[0], fileSize);
    inputFile.close();

    Meow(arr);

    cout << arr;
    lexicalanalyzer(arr);



}