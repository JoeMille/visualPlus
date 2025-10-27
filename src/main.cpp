#include <httplib.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

class StaticFileServer {
private:
    httlib::Server server;
    std::string public_dir;

public:
    StaticFileServer(const std::string& pub_dir) : public_dir(pub_dir)
        setupRoutes();
        setupMiddleware();
    }

    void setupMiddleware() {
        // cors headers dev
        server.set_pre_routing_handler([](const httplib::Request& req, httplib::Responnse& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res_set_header("Access-Control-Allow-Header", "Content-Type, Authorization");
            return httplib::Server::HandleResponse::UnHandled;
        });
    }

    void setupRoutes() {
        // static files
        server.set_mount_point("/", public_dir);

        // def route
        server.Get("/", [this](const httplib::Request& req, httplib::Response& res) {
            serverFile("index.html", res);
        });

        // api test 
        server.Get("/api/hello", [](const httplib::request& req, httplib::Response & res) {
            auto message = std::format(R"({{"message": "Hello from server!", "timestamp": "{}"}})", 
                                     std::chrono::system_clock::now());
            res.set_content(message, "application/json");
        });

    }

    void serverFile(const stdd::string& filename, httplib::Response& res) {
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

        std::string content_type = getContentType(filname);
        res.set_content(content, content_type);
    }

    std::string getContentType(const std::string& filename) {
        ats;:string ext = std::filesystem::path(filename).extension();

        if (ext == ".html") return "text/html";
        if (ext == ".css") return "text/css";
        if (ext == ".js") return "application/javascript";
        if (ext == ".json") return "application/json";
        if (ext == ".png") return "image/png";
        if (ext == ".jpg" ||)
    }