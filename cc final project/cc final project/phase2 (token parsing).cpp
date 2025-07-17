#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

using namespace std;

// Token types
const vector<string> keywords = { "Adadi", "Ashriya", "Harf", "Matn", "Mantiqi", "Agar", "Wagarna" };
const string identifier = "identifier";

// Parse tree node structure
struct TreeNode {
    string value;
    vector<shared_ptr<TreeNode>> children;

    TreeNode(const string& val) : value(val) {}

    void addChild(shared_ptr<TreeNode> child) {
        children.push_back(child);
    }

    void printTree(const string& prefix = "", bool isLast = true) const {
        cout << prefix;
        cout << (isLast ? "|----" : "|----");
        cout << value << endl;

        for (size_t i = 0; i < children.size(); i++) {
            bool last = (i == children.size() - 1);
            children[i]->printTree(prefix + (isLast ? "    " : "|    "), last);
        }
    }
};

vector<string> tokens;
int currentIndex = 0;

// Token processing functions
string extractToken(const string& tokenStr) {
    size_t start = tokenStr.find('<') + 1;
    size_t comma = tokenStr.find(',');
    size_t end = tokenStr.find('>');

    if (comma != string::npos) {
        // Has a comma (keyword or identifier with number)
        string type = tokenStr.substr(start, comma - start);
        if (type == "keyword") {
            int index = stoi(tokenStr.substr(comma + 1, end - comma - 1));
            return keywords[index - 1]; // Convert to 0-based index
        }
        else if (type == "identifier") {
            return identifier;
        }
    }
    // Single token
    return tokenStr.substr(start, end - start);
}

string currentToken() {
    if (currentIndex < tokens.size())
        return tokens[currentIndex];
    return "$";
}

void nextToken() {
    if (currentIndex < tokens.size())
        currentIndex++;
}

shared_ptr<TreeNode> advance(const string& expected) {
    if (currentToken() == expected) {
        auto node = make_shared<TreeNode>(expected);
        nextToken();
        return node;
    }
    cout << "Expected: " << expected << " but found: " << currentToken() << endl;
    return nullptr;
}

// Forward declarations
shared_ptr<TreeNode> Function();
shared_ptr<TreeNode> ArgList();
shared_ptr<TreeNode> ArgListPrime(shared_ptr<TreeNode> argList);
shared_ptr<TreeNode> Arg();
shared_ptr<TreeNode> Declaration();
shared_ptr<TreeNode> Type();
shared_ptr<TreeNode> IdentList();
shared_ptr<TreeNode> IdentListPrime(shared_ptr<TreeNode> identList);
shared_ptr<TreeNode> Stmt();
shared_ptr<TreeNode> ForStmt();
shared_ptr<TreeNode> WhileStmt();
shared_ptr<TreeNode> IfStmt();
shared_ptr<TreeNode> ElsePart(shared_ptr<TreeNode> ifStmt);
shared_ptr<TreeNode> CompStmt();
shared_ptr<TreeNode> StmtList();
shared_ptr<TreeNode> StmtListPrime(shared_ptr<TreeNode> stmtList);
shared_ptr<TreeNode> Expr();
shared_ptr<TreeNode> ExprPrime(shared_ptr<TreeNode> expr);
shared_ptr<TreeNode> Rvalue();
shared_ptr<TreeNode> RvaluePrime(shared_ptr<TreeNode> rvalue);
shared_ptr<TreeNode> Compare(shared_ptr<TreeNode> node);
shared_ptr<TreeNode> ComparePrime(shared_ptr<TreeNode> node);
shared_ptr<TreeNode> CompareDoublePrime(shared_ptr<TreeNode> node);
shared_ptr<TreeNode> Mag();
shared_ptr<TreeNode> MagPrime(shared_ptr<TreeNode> mag);
shared_ptr<TreeNode> Term();
shared_ptr<TreeNode> TermPrime(shared_ptr<TreeNode> term);
shared_ptr<TreeNode> Factor();

// Main function
shared_ptr<TreeNode> Function() {
    auto functionNode = make_shared<TreeNode>("Function");

    auto typeNode = Type();
    if (!typeNode) return nullptr;
    functionNode->addChild(typeNode);

    auto idNode = advance("identifier");
    if (!idNode) return nullptr;
    functionNode->addChild(idNode);

    auto parenNode = advance("(");
    if (!parenNode) return nullptr;
    functionNode->addChild(parenNode);

    auto argListNode = ArgList();
    if (!argListNode) return nullptr;
    functionNode->addChild(argListNode);

    parenNode = advance(")");
    if (!parenNode) return nullptr;
    functionNode->addChild(parenNode);

    auto compStmtNode = CompStmt();
    if (!compStmtNode) return nullptr;
    functionNode->addChild(compStmtNode);

    return functionNode;
}

// Argument list handling
shared_ptr<TreeNode> ArgList() {
    auto argListNode = make_shared<TreeNode>("ArgList");

    if (currentToken() == "Adadi" || currentToken() == "Ashriya" ||
        currentToken() == "Harf" || currentToken() == "Matn" ||
        currentToken() == "Mantiqi") {
        auto argNode = Arg();
        if (!argNode) return nullptr;
        argListNode->addChild(argNode);

        auto argListPrimeNode = ArgListPrime(argListNode);
        if (!argListPrimeNode) return nullptr;
    }

    return argListNode;
}

shared_ptr<TreeNode> ArgListPrime(shared_ptr<TreeNode> argList) {
    if (currentToken() == ",") {
        auto commaNode = advance(",");
        if (!commaNode) return nullptr;
        argList->addChild(commaNode);

        auto argNode = Arg();
        if (!argNode) return nullptr;
        argList->addChild(argNode);

        return ArgListPrime(argList);
    }
    return argList;
}

shared_ptr<TreeNode> Arg() {
    auto argNode = make_shared<TreeNode>("Arg");

    auto typeNode = Type();
    if (!typeNode) return nullptr;
    argNode->addChild(typeNode);

    auto idNode = advance("identifier");
    if (!idNode) return nullptr;
    argNode->addChild(idNode);

    return argNode;
}

// Declaration handling
shared_ptr<TreeNode> Declaration() {
    auto declNode = make_shared<TreeNode>("Declaration");

    auto typeNode = Type();
    if (!typeNode) return nullptr;
    declNode->addChild(typeNode);

    auto identListNode = IdentList();
    if (!identListNode) return nullptr;
    declNode->addChild(identListNode);

    auto colonNode = advance("::");
    if (!colonNode) return nullptr;
    declNode->addChild(colonNode);

    return declNode;
}

shared_ptr<TreeNode> Type() {
    auto typeNode = make_shared<TreeNode>("Type");

    if (currentToken() == "Adadi" || currentToken() == "Ashriya" ||
        currentToken() == "Harf" || currentToken() == "Matn" ||
        currentToken() == "Mantiqi") {
        auto typeToken = advance(currentToken());
        if (!typeToken) return nullptr;
        typeNode->addChild(typeToken);
        return typeNode;
    }

    cout << "Error in Type: Unexpected token " << currentToken() << endl;
    return nullptr;
}

shared_ptr<TreeNode> IdentList() {
    auto identListNode = make_shared<TreeNode>("IdentList");

    auto idNode = advance("identifier");
    if (!idNode) return nullptr;
    identListNode->addChild(idNode);

    return IdentListPrime(identListNode);
}

shared_ptr<TreeNode> IdentListPrime(shared_ptr<TreeNode> identList) {
    if (currentToken() == ",") {
        auto commaNode = advance(",");
        if (!commaNode) return nullptr;
        identList->addChild(commaNode);

        auto idNode = advance("identifier");
        if (!idNode) return nullptr;
        identList->addChild(idNode);

        return IdentListPrime(identList);
    }
    return identList;
}

// Statement handling
shared_ptr<TreeNode> Stmt() {
    auto stmtNode = make_shared<TreeNode>("Stmt");

    if (currentToken() == "for") {
        auto forNode = ForStmt();
        if (!forNode) return nullptr;
        stmtNode->addChild(forNode);
    }
    else if (currentToken() == "while") {
        auto whileNode = WhileStmt();
        if (!whileNode) return nullptr;
        stmtNode->addChild(whileNode);
    }
    else if (currentToken() == "Agar") {
        auto ifNode = IfStmt();
        if (!ifNode) return nullptr;
        stmtNode->addChild(ifNode);
    }
    else if (currentToken() == "{") {
        auto compNode = CompStmt();
        if (!compNode) return nullptr;
        stmtNode->addChild(compNode);
    }
    else if (currentToken() == "Adadi" || currentToken() == "Ashriya" ||
        currentToken() == "Harf" || currentToken() == "Matn" ||
        currentToken() == "Mantiqi") {
        auto declNode = Declaration();
        if (!declNode) return nullptr;
        stmtNode->addChild(declNode);
    }
    else if (currentToken() == "identifier" || currentToken() == "(" ||
        currentToken() == "number") {
        auto exprNode = Expr();
        if (!exprNode) return nullptr;
        stmtNode->addChild(exprNode);

        auto colonNode = advance("::");
        if (!colonNode) return nullptr;
        stmtNode->addChild(colonNode);
    }
    else {
        cout << "Error in Stmt: Unexpected token " << currentToken() << endl;
        return nullptr;
    }

    return stmtNode;
}

// Control structures
shared_ptr<TreeNode> ForStmt() {
    auto forNode = make_shared<TreeNode>("ForStmt");

    auto forToken = advance("for");
    if (!forToken) return nullptr;
    forNode->addChild(forToken);

    auto parenNode = advance("(");
    if (!parenNode) return nullptr;
    forNode->addChild(parenNode);

    auto expr1 = Expr();
    if (!expr1) return nullptr;
    forNode->addChild(expr1);

    auto colon1 = advance("::");
    if (!colon1) return nullptr;
    forNode->addChild(colon1);

    auto expr2 = Expr();
    if (!expr2) return nullptr;
    forNode->addChild(expr2);

    auto colon2 = advance("::");
    if (!colon2) return nullptr;
    forNode->addChild(colon2);

    auto expr3 = Expr();
    if (!expr3) return nullptr;
    forNode->addChild(expr3);

    parenNode = advance(")");
    if (!parenNode) return nullptr;
    forNode->addChild(parenNode);

    auto stmtNode = Stmt();
    if (!stmtNode) return nullptr;
    forNode->addChild(stmtNode);

    return forNode;
}

shared_ptr<TreeNode> WhileStmt() {
    auto whileNode = make_shared<TreeNode>("WhileStmt");

    auto whileToken = advance("while");
    if (!whileToken) return nullptr;
    whileNode->addChild(whileToken);

    auto parenNode = advance("(");
    if (!parenNode) return nullptr;
    whileNode->addChild(parenNode);

    auto exprNode = Expr();
    if (!exprNode) return nullptr;
    whileNode->addChild(exprNode);

    parenNode = advance(")");
    if (!parenNode) return nullptr;
    whileNode->addChild(parenNode);

    auto stmtNode = Stmt();
    if (!stmtNode) return nullptr;
    whileNode->addChild(stmtNode);

    return whileNode;
}

shared_ptr<TreeNode> IfStmt() {
    auto ifNode = make_shared<TreeNode>("IfStmt");

    auto ifToken = advance("Agar");
    if (!ifToken) return nullptr;
    ifNode->addChild(ifToken);

    auto parenNode = advance("(");
    if (!parenNode) return nullptr;
    ifNode->addChild(parenNode);

    auto exprNode = Expr();
    if (!exprNode) return nullptr;
    ifNode->addChild(exprNode);

    parenNode = advance(")");
    if (!parenNode) return nullptr;
    ifNode->addChild(parenNode);

    auto stmtNode = Stmt();
    if (!stmtNode) return nullptr;
    ifNode->addChild(stmtNode);

    auto elseNode = ElsePart(ifNode);
    if (!elseNode) return nullptr;

    return elseNode;
}

shared_ptr<TreeNode> ElsePart(shared_ptr<TreeNode> ifStmt) {
    if (currentToken() == "Wagarna") {
        auto elseToken = advance("Wagarna");
        if (!elseToken) return nullptr;
        ifStmt->addChild(elseToken);

        auto stmtNode = Stmt();
        if (!stmtNode) return nullptr;
        ifStmt->addChild(stmtNode);
    }
    return ifStmt;
}

shared_ptr<TreeNode> CompStmt() {
    auto compNode = make_shared<TreeNode>("CompStmt");

    auto braceNode = advance("{");
    if (!braceNode) return nullptr;
    compNode->addChild(braceNode);

    auto stmtListNode = StmtList();
    if (!stmtListNode) return nullptr;
    compNode->addChild(stmtListNode);

    braceNode = advance("}");
    if (!braceNode) return nullptr;
    compNode->addChild(braceNode);

    return compNode;
}

shared_ptr<TreeNode> StmtList() {
    auto stmtListNode = make_shared<TreeNode>("StmtList");
    return StmtListPrime(stmtListNode);
}

shared_ptr<TreeNode> StmtListPrime(shared_ptr<TreeNode> stmtList) {
    if (currentToken() == "for" || currentToken() == "while" ||
        currentToken() == "Agar" || currentToken() == "{" ||
        currentToken() == "Adadi" || currentToken() == "Ashriya" ||
        currentToken() == "Harf" || currentToken() == "Matn" ||
        currentToken() == "Mantiqi" || currentToken() == "identifier" ||
        currentToken() == "(" || currentToken() == "number") {
        auto stmtNode = Stmt();
        if (!stmtNode) return nullptr;
        stmtList->addChild(stmtNode);

        return StmtListPrime(stmtList);
    }
    return stmtList;
}

// Expression handling
shared_ptr<TreeNode> Expr() {
    auto exprNode = make_shared<TreeNode>("Expr");

    if (currentToken() == "identifier") {
        auto idNode = advance("identifier");
        if (!idNode) return nullptr;
        exprNode->addChild(idNode);

        auto exprPrimeNode = ExprPrime(exprNode);
        if (!exprPrimeNode) return nullptr;
        return exprPrimeNode;
    }
    else if (currentToken() == "(" || currentToken() == "number") {
        auto rvalueNode = Rvalue();
        if (!rvalueNode) return nullptr;
        exprNode->addChild(rvalueNode);
        return exprNode;
    }

    cout << "Error in Expr: Unexpected token " << currentToken() << endl;
    return nullptr;
}

shared_ptr<TreeNode> ExprPrime(shared_ptr<TreeNode> expr) {
    if (currentToken() == ":=") {
        auto assignNode = make_shared<TreeNode>("Assignment");

        auto assignOp = advance(":=");
        if (!assignOp) return nullptr;
        assignNode->addChild(expr->children[0]); // Move the identifier
        assignNode->addChild(assignOp);

        auto rhs = Expr();
        if (!rhs) return nullptr;
        assignNode->addChild(rhs);

        return assignNode;
    }
    else {
        auto termPrimeNode = TermPrime(expr);
        if (!termPrimeNode) return nullptr;

        auto magPrimeNode = MagPrime(termPrimeNode);
        if (!magPrimeNode) return nullptr;

        return RvaluePrime(magPrimeNode);
    }
}

shared_ptr<TreeNode> Rvalue() {
    auto rvalueNode = make_shared<TreeNode>("Rvalue");

    if (currentToken() == "(") {
        auto parenNode = advance("(");
        if (!parenNode) return nullptr;
        rvalueNode->addChild(parenNode);

        auto exprNode = Expr();
        if (!exprNode) return nullptr;
        rvalueNode->addChild(exprNode);

        parenNode = advance(")");
        if (!parenNode) return nullptr;
        rvalueNode->addChild(parenNode);

        auto termPrimeNode = TermPrime(rvalueNode);
        if (!termPrimeNode) return nullptr;

        auto magPrimeNode = MagPrime(termPrimeNode);
        if (!magPrimeNode) return nullptr;

        return RvaluePrime(magPrimeNode);
    }
    else if (currentToken() == "number") {
        auto numNode = advance("number");
        if (!numNode) return nullptr;
        rvalueNode->addChild(numNode);

        auto termPrimeNode = TermPrime(rvalueNode);
        if (!termPrimeNode) return nullptr;

        auto magPrimeNode = MagPrime(termPrimeNode);
        if (!magPrimeNode) return nullptr;

        return RvaluePrime(magPrimeNode);
    }

    cout << "Error in Rvalue: Unexpected token " << currentToken() << endl;
    return nullptr;
}

shared_ptr<TreeNode> RvaluePrime(shared_ptr<TreeNode> rvalue) {
    if (currentToken() == "==" || currentToken() == "<" ||
        currentToken() == ">" || currentToken() == "!=") {
        auto compareNode = make_shared<TreeNode>("Comparison");

        compareNode->addChild(rvalue);

        auto compareOpNode = Compare(compareNode);
        if (!compareOpNode) return nullptr;

        auto magNode = Mag();
        if (!magNode) return nullptr;
        compareNode->addChild(magNode);

        return RvaluePrime(compareNode);
    }
    return rvalue;
}

shared_ptr<TreeNode> Compare(shared_ptr<TreeNode> node) {
    if (currentToken() == "==") {
        auto opNode = advance("==");
        if (!opNode) return nullptr;
        node->addChild(opNode);
        return node;
    }
    else if (currentToken() == "<") {
        auto opNode = advance("<");
        if (!opNode) return nullptr;
        node->addChild(opNode);

        auto primeNode = ComparePrime(node);
        if (!primeNode) return nullptr;
        return primeNode;
    }
    else if (currentToken() == ">") {
        auto opNode = advance(">");
        if (!opNode) return nullptr;
        node->addChild(opNode);

        auto doublePrimeNode = CompareDoublePrime(node);
        if (!doublePrimeNode) return nullptr;
        return doublePrimeNode;
    }
    else if (currentToken() == "!=") {
        auto opNode = advance("!=");
        if (!opNode) return nullptr;
        node->addChild(opNode);
        return node;
    }

    cout << "Error in Compare: Unexpected token " << currentToken() << endl;
    return nullptr;
}

shared_ptr<TreeNode> ComparePrime(shared_ptr<TreeNode> node) {
    if (currentToken() == "=") {
        auto opNode = advance("=");
        if (!opNode) return nullptr;
        // Combine with previous < to make <=
        node->children.back()->value = "<=";
        return node;
    }
    else if (currentToken() == ">") {
        auto opNode = advance(">");
        if (!opNode) return nullptr;
        // Combine with previous < to make <>
        node->children.back()->value = "<>";
        return node;
    }
    return node;
}

shared_ptr<TreeNode> CompareDoublePrime(shared_ptr<TreeNode> node) {
    if (currentToken() == "=") {
        auto opNode = advance("=");
        if (!opNode) return nullptr;
        // Combine with previous > to make >=
        node->children.back()->value = ">=";
        return node;
    }
    return node;
}

shared_ptr<TreeNode> Mag() {
    auto magNode = make_shared<TreeNode>("Mag");

    auto termNode = Term();
    if (!termNode) return nullptr;
    magNode->addChild(termNode);

    return MagPrime(magNode);
}

shared_ptr<TreeNode> MagPrime(shared_ptr<TreeNode> mag) {
    if (currentToken() == "+" || currentToken() == "-") {
        auto op = currentToken();
        auto opNode = advance(op);
        if (!opNode) return nullptr;

        auto newMagNode = make_shared<TreeNode>("Mag");
        newMagNode->addChild(mag);
        newMagNode->addChild(opNode);

        auto termNode = Term();
        if (!termNode) return nullptr;
        newMagNode->addChild(termNode);

        return MagPrime(newMagNode);
    }
    return mag;
}

shared_ptr<TreeNode> Term() {
    auto termNode = make_shared<TreeNode>("Term");

    auto factorNode = Factor();
    if (!factorNode) return nullptr;
    termNode->addChild(factorNode);

    return TermPrime(termNode);
}

shared_ptr<TreeNode> TermPrime(shared_ptr<TreeNode> term) {
    if (currentToken() == "*" || currentToken() == "/") {
        auto op = currentToken();
        auto opNode = advance(op);
        if (!opNode) return nullptr;

        auto newTermNode = make_shared<TreeNode>("Term");
        newTermNode->addChild(term);
        newTermNode->addChild(opNode);

        auto factorNode = Factor();
        if (!factorNode) return nullptr;
        newTermNode->addChild(factorNode);

        return TermPrime(newTermNode);
    }
    return term;
}

shared_ptr<TreeNode> Factor() {
    auto factorNode = make_shared<TreeNode>("Factor");

    if (currentToken() == "(") {
        auto parenNode = advance("(");
        if (!parenNode) return nullptr;
        factorNode->addChild(parenNode);

        auto exprNode = Expr();
        if (!exprNode) return nullptr;
        factorNode->addChild(exprNode);

        parenNode = advance(")");
        if (!parenNode) return nullptr;
        factorNode->addChild(parenNode);
    }
    else if (currentToken() == "identifier") {
        auto idNode = advance("identifier");
        if (!idNode) return nullptr;
        factorNode->addChild(idNode);
    }
    else if (currentToken() == "number") {
        auto numNode = advance("number");
        if (!numNode) return nullptr;
        factorNode->addChild(numNode);
    }
    else {
        cout << "Error in Factor: Unexpected token " << currentToken() << endl;
        return nullptr;
    }

    return factorNode;
}

int main() {
    // Read tokens from file
    ifstream inputFile("tokens.txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open tokens.txt" << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string tokenStr;
        while (iss >> tokenStr) {
            tokens.push_back(extractToken(tokenStr));
        }
    }
    inputFile.close();

    cout << "Starting parsing..." << endl;
    cout << "Tokens to parse: ";
    for (const auto& t : tokens) cout << t << " ";
    cout << endl << endl;

    auto parseTree = Function();

    if (parseTree && currentToken() == "$") {
        cout << "\nParsing succeeded! Parse tree:" << endl;
        parseTree->printTree();  // Changed from print() to printTree()
    }
    else {
        cout << "\nParsing failed at token: " << currentToken()
            << " (position " << currentIndex << ")" << endl;
    }

    return 0;
}