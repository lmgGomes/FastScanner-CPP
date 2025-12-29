#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

std::mutex mtx; // Para organizar as mensagens no terminal

void scanPort(std::string ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    // Tenta conectar
    int result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    
    if (result == 0) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[+] Porta " << port << " está ABERTA!" << std::endl;
    }

    close(sock);
}

int main() {
    std::string targetIP = "127.0.0.1"; // Teste local
    int startPort = 1, endPort = 1024;
    std::vector<std::thread> threads;

    std::cout << "Iniciando scan em " << targetIP << "..." << std::endl;

    for (int i = startPort; i <= endPort; ++i) {
        threads.push_back(std::thread(scanPort, targetIP, i));
        
        // Limita o número de threads para não travar o sistema
        if (threads.size() >= 100) {
            for (auto& t : threads) t.join();
            threads.clear();
        }
    }

    for (auto& t : threads) t.join();
    std::cout << "Scan finalizado." << std::endl;

    return 0;
}
