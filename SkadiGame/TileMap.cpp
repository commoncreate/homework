#include "TileMap.h"
#include <fstream>
#include <iostream>

bool TileMap::loadFromFile(const std::string& tilesetPath,
    const std::string& mapPath, unsigned ts) {
    tileSize = ts;
    if (!tileset.loadFromFile(tilesetPath)) {
        std::cerr << "[ERROR] tileset not found: " << tilesetPath << "\n";
        return false;
    }
    std::ifstream f(mapPath);
    if (!f.is_open()) {
        std::cerr << "[ERROR] map file not found: " << mapPath << "\n";
        return false;
    }
    tiles.clear(); rows = 0; cols = 0;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (cols == 0) cols = (int)line.size();
        for (char c : line) tiles.push_back(c - '0');
        rows++;
    }
    buildVertices();
    return true;
}

bool TileMap::generateArenaLevel(const std::string& tilesetPath, unsigned ts) {

    tileSize = ts;
    if (!tileset.loadFromFile(tilesetPath)) {
        std::cerr << "[ERROR] tileset not found: " << tilesetPath << "\n";
        return false;
    }

    cols = 80;
    rows = 45;
    tiles.assign(cols * rows, 0);

    int groundRow = 38;           
    groundRowY = (float)groundRow * tileSize;  

    for (int r = groundRow; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            tiles[r * cols + c] = (r == groundRow) ? 1 : 2;
        }
    }

    for (int r = 0; r < groundRow; ++r) {
        tiles[r * cols + 0] = 3;   
        tiles[r * cols + (cols - 1)] = 3;   
    }

    for (int c = 0; c < cols; ++c) {
        tiles[0 * cols + c] = 3;
    }

    buildVertices();
    std::cout << "[OK] Arena generated: " << cols << "x" << rows
        << " (" << getWidthPx() << "x" << getHeightPx() << "px)\n";
    std::cout << "     Ground Y = " << groundRowY << "\n";
    return true;
}

int TileMap::getTileIdAt(float x, float y) const {
    int c = (int)(x / tileSize);
    int r = (int)(y / tileSize);
    if (c < 0 || c >= cols || r < 0 || r >= rows) return 0;
    return tiles[r * cols + c];
}

bool TileMap::isSolidAt(float x, float y) const {
    return getTileIdAt(x, y) != 0;
}

void TileMap::buildVertices() {
    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(cols * rows * 4);

    int tilesetCols = tileset.getSize().x / tileSize;
    if (tilesetCols <= 0) tilesetCols = 1;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int id = tiles[r * cols + c];
            sf::Vertex* quad = &vertices[(r * cols + c) * 4];
            float x = (float)c * tileSize;
            float y = (float)r * tileSize;
            quad[0].position = { x, y };
            quad[1].position = { x + tileSize, y };
            quad[2].position = { x + tileSize, y + tileSize };
            quad[3].position = { x, y + tileSize };

            if (id == 0) {
                for (int i = 0; i < 4; ++i) quad[i].color = sf::Color::Transparent;
            }
            else {
                int tu = (id - 1) % tilesetCols;
                int tv = (id - 1) / tilesetCols;
                quad[0].texCoords = { (float)(tu * tileSize),       (float)(tv * tileSize) };
                quad[1].texCoords = { (float)((tu + 1) * tileSize), (float)(tv * tileSize) };
                quad[2].texCoords = { (float)((tu + 1) * tileSize), (float)((tv + 1) * tileSize) };
                quad[3].texCoords = { (float)(tu * tileSize),       (float)((tv + 1) * tileSize) };
            }
        }
    }
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &tileset;
    target.draw(vertices, states);
}