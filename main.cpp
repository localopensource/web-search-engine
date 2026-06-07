#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

class WebPage {
public:
    std::string url;
    std::string content;
    WebPage(std::string u, std::string c) : url(u), content(c) {}
};

class InvertedIndex {
private:
    std::unordered_map<std::string, std::set<int>> index;

public:

    void addPage(int pageId, const std::vector<std::string>& tokens) {
        for (const std::string& token : tokens) {
            index[token].insert(pageId);
        }
    }

    std::set<int> search(const std::string& token) const {
        if (index.find(token) != index.end()) {
            return index.at(token);
        }
        return std::set<int>();
    }
};

class SearchEngine {
private:
    std::vector<WebPage> pages;
    InvertedIndex index;

    std::vector<std::string> tokenize(const std::string& content) {
        std::vector<std::string> tokens;
        std::string token;
        for (char ch : content) {
            if (ch == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += ch;
            }
        }
        if (!token.empty()) tokens.push_back(token);
        return tokens;
    }

public:
 
    void addPage(const WebPage& page) {
        pages.push_back(page);
        int pageId = pages.size() - 1;
        index.addPage(pageId, tokenize(page.content));
    }

    std::string search(const std::string& query) {
        std::set<int> results = index.search(query);
        if (results.empty()) {
            return "No results found.";
        }
        std::stringstream resultStream;
        for (int pageId : results) {
            resultStream << "Found: " << pages[pageId].url << "<br>";
        }
        return resultStream.str();
    }
};

void handleRequest(int clientSocket, SearchEngine& engine) {
    char buffer[1024];
    recv(clientSocket, buffer, sizeof(buffer), 0);

    std::string request(buffer);
    std::string response;

    if (request.find("GET /search?query=") == 0) {
        size_t queryStart = request.find("query=") + 6;
        size_t queryEnd = request.find(" ", queryStart);
        std::string query = request.substr(queryStart, queryEnd - queryStart);
        response = engine.search(query);
    } else if (request.find("GET / HTTP/1.1") == 0) {
        response = "<html><body><h1>Welcome to the Search Engine</h1>"
                   "<form action='/search' method='get'>"
                   "Enter search query: <input type='text' name='query'>"
                   "<button type='submit'>Search</button>"
                   "</form></body></html>";
    } else {
        response = "HTTP/1.1 404 Not Found\n";
    }

    std::string httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + response;

    send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    close(clientSocket);
}

void startServer(SearchEngine& engine) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);  // Use port 8080
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "Server is running on port 8080...\n";

    while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        handleRequest(clientSocket, engine);
    }
}

int main() {
    SearchEngine engine;

    engine.addPage(WebPage("https://example.com", "Example content with data structures and algorithms"));
    engine.addPage(WebPage("https://example2.com", "Another example with algorithms and more data"));

    startServer(engine);

    return 0;
}
