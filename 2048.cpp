#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <conio.h>

class Tile {
protected:
    int value;
public:
    Tile(int v = 0) : value(v) {}
    virtual ~Tile() {}
    int getValue() const { return value; }
    void setValue(int v) { value = v; }

    virtual std::string getDisplay() const {
        return (value == 0) ? "." : std::to_string(value);
    }

    friend std::ostream& operator<<(std::ostream& os, const Tile& tile) {
        os << tile.getDisplay();
        return os;
    }
};

class TileLetter : public Tile {
public:
    TileLetter(int v = 0) : Tile(v) {}
    std::string getDisplay() const override {
        if (value == 0) return ".";
        int index = 0, n = value;
        while (n > 1) { n /= 2; ++index; }
        if (index - 1 >= 0 && index - 1 < 26)
            return std::string(1, 'A' + (index - 1));
        return "?";
    }
};

class TileFish : public Tile {
public:
    TileFish(int v = 0) : Tile(v) {}
    std::string getDisplay() const override {
        if (value == 0) return ".";
        int index = 0, n = value;
        while (n > 1) { n /= 2; ++index; }
        const std::string fishes[] = {"🐟", "🐠", "🐬", "🦈", "🐡", "🐳"};
        return (index - 1 >= 0 && index - 1 < 6) ? fishes[index - 1] : "?";
    }
};

class TileVeg : public Tile {
public:
    TileVeg(int v = 0) : Tile(v) {}
    std::string getDisplay() const override {
        if (value == 0) return ".";
        int index = 0, n = value;
        while (n > 1) { n /= 2; ++index; }
        const std::string veggies[] = {"🥕", "🍅", "🥦", "🌽", "🍆", "🥒"};
        return (index - 1 >= 0 && index - 1 < 6) ? veggies[index - 1] : "?";
    }
};

class Board {
protected:
    int width, height;
    Tile*** tiles;
    char type;
public:
    Board(int w, int h, char t) : width(w), height(h), type(t) {
        srand(static_cast<unsigned>(time(0)));
        tiles = new Tile**[height];
        for (int i = 0; i < height; ++i) {
            tiles[i] = new Tile*[width];
            for (int j = 0; j < width; ++j)
                tiles[i][j] = createTile();
        }
        spawn();
        spawn();
    }

    virtual ~Board() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j)
                delete tiles[i][j];
            delete[] tiles[i];
        }
        delete[] tiles;
    }

    Tile* createTile(int val = 0) {
        switch (type) {
            case 'l': return new TileLetter(val);
            case 'f': return new TileFish(val);
            case 'v': return new TileVeg(val);
            default:  return new Tile(val);
        }
    }

    void draw() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::cout << "2048 Game (" << width << "x" << height << ")\n";
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j)
                std::cout << std::setw(5) << *tiles[i][j];
            std::cout << '\n';
        }
    }

    void spawn() {
        std::vector<std::pair<int, int>> empty;
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                if (tiles[i][j]->getValue() == 0)
                    empty.emplace_back(i, j);

        if (!empty.empty()) {
            auto [x, y] = empty[rand() % empty.size()];
            delete tiles[x][y];
            tiles[x][y] = createTile((rand() % 10 == 0) ? 4 : 2); // 10% là 4, còn lại là 2
        }
    }

    bool moveLeft();
    bool moveRight();
    bool moveUp();
    bool moveDown();
    bool canMove();
};

// ==== Các hàm di chuyển ====

bool Board::moveLeft() {
    bool moved = false;
    for (int i = 0; i < height; ++i) {
        int last = -1, lastVal = 0;
        std::vector<int> newRow(width, 0);
        int idx = 0;
        for (int j = 0; j < width; ++j) {
            int v = tiles[i][j]->getValue();
            if (v == 0) continue;
            if (v == lastVal && last != -1) {
                newRow[last] *= 2;
                last = -1;
                moved = true;
            } else {
                newRow[idx++] = v;
                last = idx - 1;
                lastVal = v;
            }
        }
        for (int j = 0; j < width; ++j) {
            if (tiles[i][j]->getValue() != newRow[j]) {
                delete tiles[i][j];
                tiles[i][j] = createTile(newRow[j]);
                moved = true;
            }
        }
    }
    if (moved) spawn();
    return moved;
}

bool Board::moveRight() {
    bool moved = false;
    for (int i = 0; i < height; ++i) {
        int last = -1, lastVal = 0;
        std::vector<int> newRow(width, 0);
        int idx = width - 1;
        for (int j = width - 1; j >= 0; --j) {
            int v = tiles[i][j]->getValue();
            if (v == 0) continue;
            if (v == lastVal && last != -1) {
                newRow[last] *= 2;
                last = -1;
                moved = true;
            } else {
                newRow[idx--] = v;
                last = idx + 1;
                lastVal = v;
            }
        }
        for (int j = 0; j < width; ++j) {
            if (tiles[i][j]->getValue() != newRow[j]) {
                delete tiles[i][j];
                tiles[i][j] = createTile(newRow[j]);
                moved = true;
            }
        }
    }
    if (moved) spawn();
    return moved;
}

bool Board::moveUp() {
    bool moved = false;
    for (int j = 0; j < width; ++j) {
        int last = -1, lastVal = 0;
        std::vector<int> newCol(height, 0);
        int idx = 0;
        for (int i = 0; i < height; ++i) {
            int v = tiles[i][j]->getValue();
            if (v == 0) continue;
            if (v == lastVal && last != -1) {
                newCol[last] *= 2;
                last = -1;
                moved = true;
            } else {
                newCol[idx++] = v;
                last = idx - 1;
                lastVal = v;
            }
        }
        for (int i = 0; i < height; ++i) {
            if (tiles[i][j]->getValue() != newCol[i]) {
                delete tiles[i][j];
                tiles[i][j] = createTile(newCol[i]);
                moved = true;
            }
        }
    }
    if (moved) spawn();
    return moved;
}

bool Board::moveDown() {
    bool moved = false;
    for (int j = 0; j < width; ++j) {
        int last = -1, lastVal = 0;
        std::vector<int> newCol(height, 0);
        int idx = height - 1;
        for (int i = height - 1; i >= 0; --i) {
            int v = tiles[i][j]->getValue();
            if (v == 0) continue;
            if (v == lastVal && last != -1) {
                newCol[last] *= 2;
                last = -1;
                moved = true;
            } else {
                newCol[idx--] = v;
                last = idx + 1;
                lastVal = v;
            }
        }
        for (int i = 0; i < height; ++i) {
            if (tiles[i][j]->getValue() != newCol[i]) {
                delete tiles[i][j];
                tiles[i][j] = createTile(newCol[i]);
                moved = true;
            }
        }
    }
    if (moved) spawn();
    return moved;
}

bool Board::canMove() {
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            if (tiles[i][j]->getValue() == 0) return true;
            if (j + 1 < width && tiles[i][j]->getValue() == tiles[i][j + 1]->getValue()) return true;
            if (i + 1 < height && tiles[i][j]->getValue() == tiles[i + 1][j]->getValue()) return true;
        }
    return false;
}

// ==== Game class ====
class Game {
private:
    Board* board;
public:
    Game(int w, int h, char type) {
        board = new Board(w, h, type);
    }
    ~Game() { delete board; }

    void run() {
    while (true) {
        board->draw();
        std::cout << "Di chuyen (WASD hoac phim mui ten, Q de thoat): ";
        int key = _getch();

        bool moved = false;

        if (key == 224) {
            // Phím đặc biệt (phím mũi tên)
            int arrow = _getch();
            switch (arrow) {
                case 72: moved = board->moveUp(); break;    // ↑
                case 80: moved = board->moveDown(); break;  // ↓
                case 75: moved = board->moveLeft(); break;  // ←
                case 77: moved = board->moveRight(); break; // →
            }
        } else {
            // Phím thường (wasd)
            char c = static_cast<char>(key);
            std::cout << c << "\n";

            switch (c) {
                case 'a': case 'A': moved = board->moveLeft(); break;
                case 'd': case 'D': moved = board->moveRight(); break;
                case 'w': case 'W': moved = board->moveUp(); break;
                case 's': case 'S': moved = board->moveDown(); break;
                case 'q': case 'Q': return;
            }
        }

        if (!board->canMove()) {
            board->draw();
            std::cout << "Game Over! Nhan Q de thoat...\n";
            while (true) {
                int key = _getch();
                if (key == 'q' || key == 'Q')
                    return;
            }
        }

    }
}
};

// ==== main ====
int main() {
    int w, h;
    char type;
    std::cout << "Nhap kich thuoc bang (w h): ";
    std::cin >> w >> h;

    std::cout << "Chon kieu o:\n"
              << "  s = so\n"
              << "  l = chu\n"
              << "  f = ca\n"
              << "  v = rau cu\n"
              << "Lua chon: ";
    std::cin >> type;

    Game game(w, h, type);
    game.run();

    return 0;
}
