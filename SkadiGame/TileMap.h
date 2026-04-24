#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    bool loadFromFile(const std::string& tilesetPath,
        const std::string& mapPath,
        unsigned tileSize = 16);

    bool generateArenaLevel(const std::string& tilesetPath, unsigned tileSize = 16);

    bool isSolidAt(float x, float y) const;
    int  getTileIdAt(float x, float y) const;

    unsigned getTileSize() const { return tileSize; }
    int   getCols() const { return cols; }
    int   getRows() const { return rows; }
    float getWidthPx()  const { return (float)cols * tileSize; }
    float getHeightPx() const { return (float)rows * tileSize; }

    float getGroundY()   const { return groundRowY; }
    float getArenaLeft()  const { return (float)tileSize; }          
    float getArenaRight() const { return getWidthPx() - tileSize; }  

private:
    sf::Texture tileset;
    sf::VertexArray vertices;
    std::vector<int> tiles;
    int cols = 0, rows = 0;
    unsigned tileSize = 16;

    float groundRowY = 0.f;   

    void buildVertices();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};