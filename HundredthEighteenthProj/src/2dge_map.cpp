#include "2dge_map.hpp"
const std::size_t tile_size = 64;
static std::size_t getCountNeighbours(const std::vector<std::vector<int>>& vector, std::size_t ii, std::size_t jj)
{
    std::size_t count = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            std::size_t neighbour_i = ii + i;
            std::size_t neighbour_j = jj + j;
            if (i == 0 && j == 0) {}
            else if (neighbour_i < 0 || neighbour_j < 0 || neighbour_i >= vector.size() || neighbour_j >= vector[ii].size())
                count = count + 1;
            else if (vector[neighbour_i][neighbour_j] == 1)
                count = count + 1;
        }
    }
    return count;
}
static bool getLeftNeighbour(const std::vector<std::vector<int>>& vector, std::size_t i, std::size_t j)
{
    if (j != 0)
        return vector[i][j - 1];
    return true;
}
static bool getRightNeighbour(const std::vector<std::vector<int>>& vector, std::size_t i, std::size_t j)
{
    if (j + 1 != vector[i].size())
        return vector[i][j + 1];
    return true;
}
static bool getUpNeighbour(const std::vector<std::vector<int>>& vector, std::size_t i, std::size_t j)
{
    if (i != 0)
        return vector[i - 1][j];
    return true;
}
static bool getDownNeighbour(const std::vector<std::vector<int>>& vector, std::size_t i, std::size_t j)
{
    if (i + 1 != vector.size())
        return vector[i + 1][j];
    return true;
}
static int getLRUDcountNeighbours(const std::vector<std::vector<int>>& vector, std::size_t i, std::size_t j)
{
    return getLeftNeighbour(vector, i, j) + getRightNeighbour(vector, i, j) + getUpNeighbour(vector, i, j) + getDownNeighbour(vector, i, j);
}
struct cmp
{
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const
    {
        if (a.x == b.x)
            return a.y > b.y;
        return a.x < b.x;
    }
};
static std::vector<sf::Vector2i> getNeighbours(const std::vector<std::vector<int>>& matrix, const sf::Vector2i& current) {
    std::vector<sf::Vector2i> result;
    int size = matrix.size();
    if (current.x != 0 && matrix[current.y][current.x - 1] != 1) 
        result.push_back(sf::Vector2i(current.x - 1, current.y));
    if (current.x + 1 < size && matrix[current.y][current.x + 1] != 1)
        result.push_back(sf::Vector2i(current.x + 1, current.y));
    if (current.y != 0 && matrix[current.y - 1][current.x] != 1)
        result.push_back(sf::Vector2i(current.x, current.y - 1));
    if (current.y + 1 < size && matrix[current.y + 1][current.x] != 1)
        result.push_back(sf::Vector2i(current.x, current.y + 1));
    if (current.x != 0 && current.y != 0 && matrix[current.y - 1][current.x - 1] != 1)
        result.push_back(sf::Vector2i(current.x - 1, current.y - 1));
    if (current.x + 1 < size && current.y != 0 && matrix[current.y - 1][current.x + 1] != 1)
        result.push_back(sf::Vector2i(current.x + 1, current.y - 1));
    if (current.x != 0 && current.y + 1 < size && matrix[current.y + 1][current.x - 1] != 1)
        result.push_back(sf::Vector2i(current.x - 1, current.y + 1));
    if (current.x + 1 < size && current.y + 1 < size && matrix[current.y + 1][current.x + 1] != 1)
        result.push_back(sf::Vector2i(current.x + 1, current.y + 1));
    return result;
}
static float h(const sf::Vector2i& current, const sf::Vector2i& goal)
{
    float tmp1 = goal.x - current.x;
    float tmp2 = goal.y - current.y;
    tmp1 *= tmp1;
    tmp2 *= tmp2;
    return sqrt(tmp1 + tmp2);
}
static float g(const std::pair<int, int>& start, const std::pair<int, int>& current)
{
    return 0;
}
static std::vector<sf::Vector2i> reconstructPath(const std::vector<std::vector<sf::Vector2i>>& cameFrom, const sf::Vector2i& goal, const sf::Vector2i& start) 
{
    std::vector<sf::Vector2i> result;
    result.push_back(goal);
    auto current = goal;
    //std::cout << "Path:";
    while (current != start) {
        //std::cout << "(" << current.x << "," << current.y << ")";
        result.push_back(current);
        current = cameFrom[current.y][current.x];
    }
    //std::cout << std::endl;
    return result;
}
static sf::Vector2i getMinfScore(const std::vector<std::vector<float>>& fScore, const std::set<sf::Vector2i, cmp>& openSet)
{
    float minfScore = std::numeric_limits<float>::infinity();
    sf::Vector2i result(0, 0);
    for (auto& current: openSet) {
        if (fScore[current.y][current.x] < minfScore) {
            result = current;
            minfScore = fScore[current.y][current.x];
        }   
    }
    return result;
}
Map::Map(ObjectManager& oManager_):
oManager(oManager_)
{
    __DEBUG_EXEC(std::cout << "Map()\n");
}
Map::~Map()
{
    __DEBUG_EXEC(std::cout << "~Map()\n");
}
void Map::setMatrix(std::vector<std::vector<int>> matrix_) 
{
    matrix = matrix_;
}
std::vector<std::vector<int>> Map::getMatrix() const
{
    return matrix;
}
void Map::load() const
{
    LuaScript script("scripts/objects.lua");
    auto isize = matrix.size();
    auto player = oManager.findDynamicObject(script.get<int>("setting.type.player"));
    auto newMatrix = matrix;
    std::shared_ptr<SpriteObject> sprite_ptr;
    std::shared_ptr<StaticGameObject> static_obj_ptr;
    for (std::size_t i = 0, ii = 0; i < isize; i++, ii += 2) {
        auto jsize = matrix[i].size();
        for (std::size_t j = 0, jj = 0; j < jsize; j++, jj += 2) {
            if (matrix[i][j] == 0) {
                sprite_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                sprite_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                sprite_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                sprite_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                sprite_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                sprite_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                sprite_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                sprite_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
            } 
            else {
                auto left = getLeftNeighbour(matrix, i, j);
                auto right = getRightNeighbour(matrix, i, j);
                auto up = getUpNeighbour(matrix, i, j);
                auto down = getDownNeighbour(matrix, i, j);
                std::size_t condition = up + right * 2 + down * 4 + left * 8;
                std::shared_ptr<SpriteObject> sprite_obj_ptr;
                std::shared_ptr<StaticGameObject> static_obj_ptr;
                switch (condition) {
                case 0:     //0000
                    break;
                case 1:     //0001
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_left_down_corner"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_right_down_corner"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_left_down_corner_bot"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_right_down_corner_bot"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                    static_obj_ptr->setPosition(jj * tile_size, (ii - 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_top"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii - 2) * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, (ii - 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_top"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii - 2) * tile_size);
                    break;
                case 2:     //0010
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_leftend_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal_leftend"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j + 1 != matrix[i - 1].size())
                    if (getLRUDcountNeighbours(matrix, i - 1, j + 1) != 2 && matrix[i - 1][j + 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition((jj + 2) * tile_size, ii * tile_size);
                    }
                    break;
                case 3:     //0011
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_left_down_corner"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_left_down_corner_bot"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j != 0)
                    if (matrix[i - 1][j - 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition(jj * tile_size, (ii - 1) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_top"));
                        sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii - 2) * tile_size);
                    }
                    if (i + 1 != matrix.size() && j + 1 != matrix[i + 1].size())
                    if (matrix[i + 1][j + 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition((jj + 2) * tile_size, (ii + 1) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_top"));
                        sprite_obj_ptr->sprite.setPosition((jj + 2) * tile_size, ii * tile_size);
                    }
                    break;
                case 4:     //0100
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_left_up_corner_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_right_up_corner_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_left_up_corner"));
                    static_obj_ptr->setPosition(jj * tile_size, (ii + 1) * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_right_up_corner"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    break;
                case 5:     //0101
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                    static_obj_ptr->setPosition(jj * tile_size, (ii + 1) * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    break;
                case 6:     //0110
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_left_up_corner_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_left_up_corner"));
                    static_obj_ptr->setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (j != 0 && i + 1 != matrix.size())
                    if (matrix[i + 1][j - 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition(jj * tile_size, (ii + 2) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_bot"));
                        sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 3) * tile_size);
                    }
                    if (i != 0 && j + 1 != matrix[i - 1].size())
                    if (matrix[i - 1][j + 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition((jj + 2) * tile_size, ii * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_bot"));
                        sprite_obj_ptr->sprite.setPosition((jj + 2) * tile_size, (ii + 1) * tile_size);
                    }
                    break;
                case 7:     //0111
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                    static_obj_ptr->setPosition(jj * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j != 0)
                    if (matrix[i - 1][j - 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition(jj * tile_size, (ii - 1) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_top"));
                        sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii - 2) * tile_size);
                    }
                    break;
                case 8:     //1000
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_rightend_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal_rightend"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j != 0)
                    if (getLRUDcountNeighbours(matrix, i - 1, j - 1) != 2) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj - 1) * tile_size, ii * tile_size);
                    }
                    break;
                case 9:     //1001
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_right_down_corner"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_right_down_corner_bot"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j + 1 != matrix[i - 1].size()) 
                    if (matrix[i - 1][j + 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj + 1) * tile_size, (ii - 1) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_top"));
                        sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii - 2) * tile_size);
                    }
                    if (j != 0 && i + 1 != matrix.size())
                    if (matrix[i + 1][j - 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj - 1) * tile_size, (ii + 1) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_top"));
                        sprite_obj_ptr->sprite.setPosition((jj - 1) * tile_size, ii * tile_size);
                    }
                    break;
                case 10:    //1010
                    ;
                case 11:    //1011
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    break;
                case 12:    //1100
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_right_up_corner_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_right_up_corner"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j != 0)
                    if (matrix[i - 1][j - 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj - 1) * tile_size, ii * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_bot"));
                        sprite_obj_ptr->sprite.setPosition((jj - 1) * tile_size, (ii + 1) * tile_size);
                    }
                    if (i + 1 != matrix.size() && j + 1 != matrix[i + 1].size())
                    if (matrix[i + 1][j + 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj + 1) * tile_size, (ii + 2) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_bot"));
                        sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 3) * tile_size);
                    }
                    break;
                case 13:    //1101
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i != 0 && j + 2 != matrix[j].size())
                    if ((matrix[i - 1][j + 1] == 1 && matrix[i - 1][j + 2] == 1) ||
                        (i - 1 != 0 && j + 2 != matrix[i - 1].size() && matrix[i - 1][j + 1] == 1 && matrix[i - 1][j + 2] == 0 && matrix[i - 2][j + 1] == 0)) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj + 1) * tile_size, (ii - 1) * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_top"));
                        sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii - 2) * tile_size);
                    }
                    break;
                case 14:    //1110
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.ground"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition(jj * tile_size, ii * tile_size);
                    static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_horizontal_top"));
                    static_obj_ptr->setPosition((jj + 1) * tile_size, ii * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition(jj * tile_size, (ii + 1) * tile_size);
                    sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_horizontal"));
                    sprite_obj_ptr->sprite.setPosition((jj + 1) * tile_size, (ii + 1) * tile_size);
                    if (i - 1 != 0 && j != 0)
                    if (matrix[i - 1][j - 1] == 1 && matrix[i - 2][j - 1] == 1) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj - 1) * tile_size, ii * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_bot"));
                        sprite_obj_ptr->sprite.setPosition((jj - 1) * tile_size, (ii + 1) * tile_size);
                    }
                    if (i - 1 != 0 && j + 1 != matrix[i - 1].size())
                    if ((matrix[i - 1][j + 1] == 1 && matrix[i - 2][j + 1] == 1) ||
                        (i != 0 && j + 2 != matrix[i - 1].size() && matrix[i - 1][j + 1] == 1 && matrix[i - 1][j + 2] == 0)) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_left"));
                        static_obj_ptr->setPosition((jj + 2) * tile_size, ii * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_left_bot"));
                        sprite_obj_ptr->sprite.setPosition((jj + 2) * tile_size, (ii + 1) * tile_size);
                    }
                    if (i != 0 && j - 1 != 0)
                    if (matrix[i - 1][j - 1] == 1 && matrix[i - 1][j - 2] == 0) {
                        static_obj_ptr = oManager.newStaticObject(script.get<int>("setting.type.wall_vertical_right"));
                        static_obj_ptr->setPosition((jj - 1) * tile_size, ii * tile_size);
                        sprite_obj_ptr = oManager.newSpriteObject(script.get<int>("setting.type.wall_vertical_right_bot"));
                        sprite_obj_ptr->sprite.setPosition((jj - 1) * tile_size, (ii + 1) * tile_size);
                    }
                    break;
                case 15:    //1111
                    break;
                default:
                    std::cerr << "_____ERROR: Loading map failed: Unknown condition" << condition << std::endl;
                    break;
                }
            }
        }
    }
}
std::vector<std::vector<int>> Map::initMatrix(std::size_t size, double init_chance)
{
    std::mt19937 gen(time(0)); 
    std::uniform_real_distribution<> urd(0, 1); 
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size, 0));
    for (std::size_t i = 0; i < size; i++)
        for (std::size_t j = 0; j < size; j++) {
            if (urd(gen) <= init_chance)
                matrix[i][j] = 1;
            else 
                matrix[i][j] = 0;
        }
    return matrix;
}
std::vector<std::vector<int>> Map::generate(std::size_t size, double init_chance,
std::size_t newlimit, std::size_t deletelimit, std::size_t numberSteps) 
{
    matrix = initMatrix(size, init_chance);
    for (std::size_t step = 0; step < numberSteps; step++) {
        matrix = generateStep(newlimit, deletelimit);
    }
    dumpMatrix(matrix);
    return matrix;
}
std::vector<std::vector<int>> Map::generateStep(std::size_t newlimit, std::size_t deletelimit)
{
    auto size = matrix.size();
    auto newmatrix = std::vector<std::vector<int>>(size, std::vector<int>(size, 0));
    for (std::size_t i = 0; i < size; i++) 
        for (std::size_t j = 0; j < size; j++) {
            auto countNeighbours = getCountNeighbours(matrix, i, j);
            if (matrix[i][j] == 1) {
                if (countNeighbours < deletelimit)
                    newmatrix[i][j] = 0;
                else
                    newmatrix[i][j] = 1;
            } 
            else {
                if (countNeighbours > newlimit)
                    newmatrix[i][j] = 1;
                else
                    newmatrix[i][j] = 0;
            }
        }
    return newmatrix;
}
void Map::dumpMatrix(std::vector<std::vector<int>> matrix) const
{
    for (std::size_t i = 0; i < matrix.size(); i++) {
        for (std::size_t j = 0; j < matrix[i].size(); j++) {
            if (matrix[i][j] == 1)
                std::cout << char(219);
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }
}
std::vector<sf::Vector2i> Map::getPath(sf::Vector2i start, sf::Vector2i goal)
{
    auto size = matrix.size();
    //std::cout << "Start: (" << start.x << "," << start.y << ")\n";
    //std::cout << "End: (" << goal.x << "," << goal.y << ")\n";
    std::vector<std::vector<sf::Vector2i>> cameFrom(size, std::vector<sf::Vector2i>(size, sf::Vector2i(0, 0)));
    std::vector<std::vector<float>> gScore(size, std::vector<float>(size, std::numeric_limits<float>::infinity()));
    gScore[start.y][start.x] = 0;
    std::vector<std::vector<float>> fScore(size, std::vector<float>(size, std::numeric_limits<float>::infinity()));
    fScore[start.y][start.x] = h(start, goal);
    std::set<sf::Vector2i, cmp> openSet;
    openSet.insert(start);
    std::set<sf::Vector2i, cmp> closedSet;
    auto dump = [](const std::set<sf::Vector2i, cmp>& openSet) {
        for (auto& current: openSet) {
            std::cout << "(" << current.x << "," << current.y << ") ";
        }
        std::cout << std::endl;
    };
    auto max_iter = 50;
    auto iter = 0;
    while (!openSet.empty()) {
        if (iter++ > max_iter)
            break;
        //dump(openSet);
        auto current = getMinfScore(fScore, openSet);
        //std::cout << "MinfScore: (" << current.x << "," << current.y << ")\n";
        if (current.x == goal.x && current.y == goal.y)
            return reconstructPath(cameFrom, current, start);
        openSet.erase(current);
        closedSet.insert(current);
        auto neighbours = getNeighbours(matrix, current);
        for (auto& neighbour: neighbours) {
            if (closedSet.find(neighbour) != closedSet.end())
                continue;
            auto tentative_gScore = gScore[current.y][current.x] + 1;
            if (tentative_gScore < gScore[neighbour.y][neighbour.x]) {
                openSet.insert(neighbour);
                cameFrom[neighbour.y][neighbour.x] = current;
                //std::cout << "(" << neighbour.x << "," << neighbour.y << ") came from minfScore\n";
                gScore[neighbour.y][neighbour.x] = tentative_gScore;
                fScore[neighbour.y][neighbour.x] = tentative_gScore + h(neighbour, goal);
            }
        }
    }
    std::vector<sf::Vector2i> result;
    result.push_back(start);
    return result;
}