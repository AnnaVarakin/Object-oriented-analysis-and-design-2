#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "httplib.h"

using namespace std;

// КЛАСС ЗВЕЗДЫ 
class Star
{
private:
    int m_id;
    int m_x;
    int m_y;
    int m_size;          // 1, 2, 3
    string m_color;      // "white", "yellow"
    bool m_outline;      // true = есть контур
    bool m_twinkle;      // true = может мерцать
    int m_brightness;  
    int m_type;          // для совместимости 

public:
    Star(int id, int x, int y, int size, const string& color, bool outline, bool twinkle)
        : m_id(id), m_x(x), m_y(y), m_size(size), m_color(color), m_outline(outline), m_twinkle(twinkle)
    {
        m_brightness = rand() % 9 + 1; // случайная яркость
        if (m_brightness > 7) m_type = 2;
        else if (m_brightness > 4) m_type = 1;
        else m_type = 0;
    }

    int getId() const { return m_id; }
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    int getSize() const { return m_size; }
    string getColor() const { return m_color; }
    bool hasOutline() const { return m_outline; }
    bool canTwinkle() const { return m_twinkle; }
    int getBrightness() const { return m_brightness; }
    int getType() const { return m_type; }

    void twinkle()
    {
        if (!m_twinkle) return;
        int change = rand() % 3 - 1;
        m_brightness += change;
        if (m_brightness < 1) m_brightness = 1;
        if (m_brightness > 9) m_brightness = 9;

        if (m_brightness > 7) m_type = 2;
        else if (m_brightness > 4) m_type = 1;
        else m_type = 0;
    }

    string toJSON() const
    {
        stringstream ss;
        ss << "{";
        ss << "\"id\":" << m_id << ",";
        ss << "\"x\":" << m_x << ",";
        ss << "\"y\":" << m_y << ",";
        ss << "\"size\":" << m_size << ",";
        ss << "\"color\":\"" << m_color << "\",";
        ss << "\"outline\":" << (m_outline ? "true" : "false") << ",";
        ss << "\"twinkle\":" << (m_twinkle ? "true" : "false") << ",";
        ss << "\"b\":" << m_brightness;
        ss << "}";
        return ss.str();
    }
};

// КЛАСС НЕБА 
class Sky
{
private:
    int m_width;
    int m_height;
    map<int, Star> m_stars;
    int m_nextId;

public:
    Sky(int width, int height)
        : m_width(width), m_height(height), m_nextId(1) {}

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    int addStar(int x, int y, int size, const string& color, bool outline, bool twinkle)
    {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) return -1;
        // проверка, что клетка свободна
        for (const auto& pair : m_stars) {
            if (pair.second.getX() == x && pair.second.getY() == y)
                return -1; // клетка уже занята
        }
        int id = m_nextId++;
        m_stars.emplace(id, Star(id, x, y, size, color, outline, twinkle));
        return id;
    }

    bool removeStar(int id)
    {
        return m_stars.erase(id) > 0;
    }

    bool removeStarAt(int x, int y)
    {
        for (auto it = m_stars.begin(); it != m_stars.end(); ++it) {
            if (it->second.getX() == x && it->second.getY() == y) {
                m_stars.erase(it);
                return true;
            }
        }
        return false;
    }

    void clear()
    {
        m_stars.clear();
        m_nextId = 1;
    }

    size_t getStarCount() const { return m_stars.size(); }

    void update()
    {
        for (auto& pair : m_stars) {
            pair.second.twinkle();
        }
    }

    string getStarsJSON() const
    {
        stringstream ss;
        ss << "[";
        bool first = true;
        for (const auto& pair : m_stars) {
            if (!first) ss << ",";
            ss << pair.second.toJSON();
            first = false;
        }
        ss << "]";
        return ss.str();
    }

    // Генерация случайного созвездия 
    void generateRandomConstellation(int count)
    {
        int centerX = rand() % m_width;
        int centerY = rand() % m_height;
        for (int i = 0; i < count; ++i) {
            int dx = rand() % 5 - 2;
            int dy = rand() % 5 - 2;
            int x = centerX + dx;
            int y = centerY + dy;
            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                int size = rand() % 3 + 1;
                string color = (rand() % 2 == 0) ? "white" : "yellow";
                bool outline = rand() % 2;
                bool twinkle = rand() % 2;
                addStar(x, y, size, color, outline, twinkle);
            }
        }
    }
};

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ 
Sky* g_sky = nullptr;
mutex g_skyMutex;

string loadHTMLFile()
{
    ifstream file("index.html");
    if (!file.is_open())
    {
        return "<html><body><h1>Ошибка: файл index.html не найден</h1></body></html>";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // Небо по умолчанию: 25x15, без звёзд
    g_sky = new Sky(25, 15);

    // Поток анимации (мерцание)
    thread animationThread([]() {
        while (true)
        {
            this_thread::sleep_for(chrono::milliseconds(300));
            lock_guard<mutex> lock(g_skyMutex);
            if (g_sky) g_sky->update();
        }
        });
    animationThread.detach();

    httplib::Server svr;

    // Главная страница
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        string html = loadHTMLFile();
        res.set_content(html, "text/html");
        });

    // Получить текущее состояние неба
    svr.Get("/api/stars", [](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(g_skyMutex);
        string json = "{";
        json += "\"width\":" + to_string(g_sky->getWidth()) + ",";
        json += "\"height\":" + to_string(g_sky->getHeight()) + ",";
        json += "\"count\":" + to_string(g_sky->getStarCount()) + ",";
        json += "\"stars\":" + g_sky->getStarsJSON();
        json += "}";
        res.set_content(json, "application/json");
        });

    // Добавить звезду
    svr.Post("/api/star", [](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(g_skyMutex);
        int x = stoi(req.get_param_value("x"));
        int y = stoi(req.get_param_value("y"));
        int size = stoi(req.get_param_value("size"));
        string color = req.get_param_value("color");
        bool outline = req.get_param_value("outline") == "true";
        bool twinkle = req.get_param_value("twinkle") == "true";

        int id = g_sky->addStar(x, y, size, color, outline, twinkle);
        if (id > 0) {
            res.set_content("{\"status\":\"ok\",\"id\":" + to_string(id) + "}", "application/json");
        }
        else {
            res.set_content("{\"status\":\"error\",\"message\":\"cell occupied or invalid\"}", "application/json");
        }
        });

    // Удалить звезду по координатам
    svr.Delete("/api/star", [](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(g_skyMutex);
        int x = stoi(req.get_param_value("x"));
        int y = stoi(req.get_param_value("y"));
        bool ok = g_sky->removeStarAt(x, y);
        if (ok) {
            res.set_content("{\"status\":\"ok\"}", "application/json");
        }
        else {
            res.set_content("{\"status\":\"error\",\"message\":\"star not found\"}", "application/json");
        }
        });

    // Удалить все звезды
    svr.Delete("/api/stars", [](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(g_skyMutex);
        g_sky->clear();
        res.set_content("{\"status\":\"ok\"}", "application/json");
        });

    // Сгенерировать случайное созвездие
    svr.Post("/api/constellation", [](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(g_skyMutex);
        int count = 5; // количество звёзд в созвездии
        if (req.has_param("count")) count = stoi(req.get_param_value("count"));
        g_sky->generateRandomConstellation(count);
        res.set_content("{\"status\":\"ok\"}", "application/json");
        });

    // Обновить мерцание (для анимации)
    svr.Get("/api/update", [](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(g_skyMutex);
        if (g_sky) g_sky->update();
        res.set_content("{\"status\":\"ok\"}", "application/json");
        });

    svr.listen("0.0.0.0", 8080);

    delete g_sky;
    return 0;
}