#include <iostream>
#include <vector>
#include <sstream>
#include <string>

bool isPathInsideRoot(const std::string &root, const std::string &path) {
    if (path.find(root) != 0)
        return false; // Não começa com a raiz -> Acesso negado

    // O caminho é exatamente a raiz OU tem uma / logo após
    if (path.size() == root.size() || path[root.size()] == '/')
        return true;

    return false; // Exemplo perigoso: "/var/www_malicioso"
}


int main() {
    std::cout << isPathInsideRoot("/var/www", "/var/www/html/index.html") << "\n";  // true (válido)
    std::cout << isPathInsideRoot("/var/www", "/var/www_malicioso/index.html") << "\n"; // false (perigoso)
    std::cout << isPathInsideRoot("/var/www", "/var/www") << "\n";  // true (exatamente a raiz)
    std::cout << isPathInsideRoot("/var/www", "/var/www/") << "\n";  // true (subdiretório correto)
    std::cout << isPathInsideRoot("/var/www", "/etc/passwd") << "\n"; // false (fora da raiz)
}

