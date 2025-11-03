#include <httplib.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>
#include <chrono>

class StaticFileServer {
private:
    httplib::Server server;
    std::string public_dir;

public:
    StaticFileServer(const std::string& pub_dir) : public_dir(pub_dir) {
        setupRoutes();
        setupMiddleware();
    }

    void setupMiddleware() {
        server.set_logger([](const httplib::Request& req, const httplib::Response& res) {
            std::cout << std::format("{} {} - {}\n", req.method, req.path, res.status);
        });
        // cors headers dev
        server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return httplib::Server::HandlerResponse::Unhandled;
        });
    }

    void setupRoutes() {
    
        // def route
        server.Get("/", [this](const httplib::Request& req, httplib::Response& res) {
            serveFile("index.html", res);
        });

        server.Get("R(/(.+))", [this](const httplib::Request& req, httplib::Response& res) {
            auto filename = req.matches[1].str();
            serveFile(filename, res);
        });

        // api test 
        server.Get("/api/hello", [](const httplib::Request& req, httplib::Response& res) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto message = std::format(R"({{"message": "Hello from C++20 server!", "timestamp": "{}"}})", 
                                     std::ctime(&time_t));
            res.set_content(message, "application/json");
        });
    }

    void serveFile(const std::string& filename, httplib::Response& res) {
        auto filepath = std::filesystem::path(public_dir) / filename;

        if (!std::filesystem::exists(filepath)) {
            res.status = 404;
            res.set_content("file not found", "text/plain");
            return;
        }

        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            res.status = 500;
            res.set_content("internal server error", "text/plain");
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

        std::string content_type = getContentType(filename);
        res.set_content(content, content_type);
    }

    std::string getContentType(const std::string& filename) {
        auto ext = std::filesystem::path(filename).extension().string();

        if (ext == ".html") return "text/html";
        if (ext == ".css") return "text/css";
        if (ext == ".js") return "application/javascript";
        if (ext == ".json") return "application/json";
        if (ext == ".png") return "image/png";
        if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
        if (ext == ".svg") return "image/svg+xml";

        return "text/plain";
    }

    void start(int port) {
        std::cout << std::format("Starting server on http://localhost:{}\n", port);
        std::cout << std::format("Serving files from: {}\n", std::filesystem::absolute(public_dir).string());

        if (!server.listen("localhost", port)) {
            std::cerr << std::format("Failed to start server on port {}\n", port);
        }
    }
};

int main() {
    // scan for pub dir
    const auto public_dir = std::filesystem::path("public");
    if (!std::filesystem::exists(public_dir)) {
        std::cerr << "Public directory not found. Make sure 'public' folder exists." << std::endl;
        return 1;
    }

    StaticFileServer server(public_dir.string());
    server.start(5001);

    return 0;
}