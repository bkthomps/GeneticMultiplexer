#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

void evaluateExpression(std::vector<std::string>& tree, int start, int end) {
    assert(start < end);
    assert(tree[start] == "(");
    assert(tree[end] == ")");
    if (tree[start + 1] == "NOT") {
        assert(start + 3 == end);
        int notExpr = tree[start + 2][0] - '0';
        int evaluation = !notExpr;
        tree.erase(tree.begin() + start + 1, tree.begin() + end + 1);
        tree[start] = evaluation + '0';
        return;
    }
    if (tree[start + 1] == "IF") {
        assert(start + 7 == end);
        int ifExpr = tree[start + 2][0] - '0';
        int trueExpr = tree[start + 4][0] - '0';
        int falseExpr = tree[start + 6][0] - '0';
        int evaluation = ifExpr ? trueExpr : falseExpr;
        tree.erase(tree.begin() + start + 1, tree.begin() + end + 1);
        tree[start] = evaluation + '0';
        return;
    }
    if (tree[start + 2] == "AND") {
        assert(start + 4 == end);
        int firstExpr = tree[start + 1][0] - '0';
        int secondExpr = tree[start + 3][0] - '0';
        int evaluation = firstExpr & secondExpr;
        tree.erase(tree.begin() + start + 1, tree.begin() + end + 1);
        tree[start] = evaluation + '0';
        return;
    }
    if (tree[start + 2] == "OR") {
        assert(start + 4 == end);
        int firstExpr = tree[start + 1][0] - '0';
        int secondExpr = tree[start + 3][0] - '0';
        int evaluation = firstExpr | secondExpr;
        tree.erase(tree.begin() + start + 1, tree.begin() + end + 1);
        tree[start] = evaluation + '0';
        return;
    }
    assert(false);
}

bool evaluateTree(std::vector<std::string> tree,
                  const std::vector<int>& address, const std::vector<int>& data) {
    for (std::size_t i = 0; i < tree.size(); i++) {
        if (tree[i][0] == 'a') {
            int number = tree[i][1] - '0';
            int value = address[number];
            tree[i] = std::to_string(value);
        } else if (tree[i][0] == 'd') {
            int number = tree[i][1] - '0';
            int value = data[number];
            tree[i] = std::to_string(value);
        }
    }
    while (true) {
        std::stack<int> stack{};
        for (std::size_t i = 0; i < tree.size(); i++) {
            if (tree[i] == "(") {
                stack.push(i);
                continue;
            }
            if (tree[i] == ")") {
                int end = i;
                int start = stack.top();
                evaluateExpression(tree, start, end);
                break;
            }
        }
        if (stack.empty()) {
            assert(tree.size() == 1);
            return tree[0][0] - '0';
        }
    }
}

bool isValid(const std::vector<std::string>& tree, int addressPinsLeft, int dataPinsLeft,
             std::vector<int> address, std::vector<int> data) {
    if (addressPinsLeft == 0 && dataPinsLeft == 0) {
        int offset{0};
        for (int a : address) {
            assert(a == 0 || a == 1);
            offset *= 2;
            offset += a;
        }
        int mappedTruth = data.at(offset);
        int treeTruth = evaluateTree(tree, address, data);
        return mappedTruth == treeTruth;
    }
    if (addressPinsLeft == 0) {
        data.emplace_back(0);
        bool first = isValid(tree, 0, dataPinsLeft - 1, address, data);
        data.pop_back();
        data.emplace_back(1);
        bool second = isValid(tree, 0, dataPinsLeft - 1, address, data);
        data.pop_back();
        return first && second;
    }
    address.emplace_back(0);
    bool first = isValid(tree, addressPinsLeft - 1, dataPinsLeft, address, data);
    address.pop_back();
    address.emplace_back(1);
    bool second = isValid(tree, addressPinsLeft - 1, dataPinsLeft, address, data);
    address.pop_back();
    return first && second;
}

std::tuple<int, int> pinsCount(const std::vector<std::string>& tree) {
    std::unordered_set<std::string> addressPins{};
    std::unordered_set<std::string> dataPins{};
    for (auto str : tree) {
        if (str.rfind("a", 0) == 0) {
            addressPins.emplace(str);
        } else if (str.rfind("d", 0) == 0) {
            dataPins.emplace(str);
        }
    }
    return std::make_tuple(addressPins.size(), dataPins.size());
}

std::vector<std::string> splitTree(std::string tree) {
    std::vector<std::string> splitTree{};
    std::istringstream stream{tree};
    std::string cur{};
    while (stream >> cur) {
        splitTree.push_back(cur);
    }
    return splitTree;
}

std::string getTree(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Must specify the file to test as an argument" << std::endl;
        return "";
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "File did not open" << std::endl;
        return "";
    }
    std::string tree{};
    std::getline(file, tree);
    file.close();
    return tree;
}

int main(int argc, char* argv[]) {
    auto tree = getTree(argc, argv);
    if (tree == "") {
        std::cerr << "Could not read logic tree" << std::endl;
        return -1;
    }
    auto split = splitTree(tree);
    auto[addressPins, dataPins] = pinsCount(split);
    std::vector<int> address{};
    std::vector<int> data{};
    bool isMuxValid = isValid(split, addressPins, dataPins, address, data);
    if (!isMuxValid) {
        std::cerr << "Error: the multiplexer is invalid" << std::endl;
        return -1;
    }
    std::cout << "Success: the multiplexer is valid" << std::endl;
    return 0;
}
