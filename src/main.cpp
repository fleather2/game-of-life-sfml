/*
    Conway's game of life. Does what you think it does.
    Neighbors are horizontal, vertical, or diagonal.
    Four rules in the original:
        1) Any live cell with fewer than 2 live neighbors dies, as if by underpopulation
        2) Any live cell with 2 or 3 live neighbors lives on to the next generation
        3) Any live cell with more than 3 live neighbors dies, as if by overpopulation
        4) Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction
*/

#include <SFML/Graphics.hpp>
#include <stdlib.h>

#define MAX_X 1500
#define MAX_Y 800

#define BLOCK_WIDTH 20
#define BLOCK_HEIGHT 20

#define DEAD_COLOR UINT32_MAX
#define LIVE_COLOR 0

#define SLEEP_BETWEEN_ITERATIONS_MILLISECONDS 50

struct Cell {
    sf::ConvexShape cellShape;
    int status;
    int next_status;
};

int main()
{
    auto window = sf::RenderWindow{ { MAX_X, MAX_Y }, "Game of Life" };
    window.setFramerateLimit(144);
    window.setKeyRepeatEnabled(false);

    int numCellsInRow = MAX_X / BLOCK_WIDTH;
    int numCellsInColumn = MAX_Y / BLOCK_HEIGHT;
    int numCells = numCellsInRow * numCellsInColumn;
    bool paused = false;
    int cellClickedX;
    int cellClickedY;

    Cell cells[numCellsInRow][numCellsInColumn]; // Addressed by [x index][y index]

    for (int j = 0; j < numCellsInColumn; j++) {
        for (int i = 0; i < numCellsInRow; i++) {
            cells[i][j].cellShape.setPointCount(4);
            cells[i][j].cellShape.setOrigin(sf::Vector2f(0.f, 0.f)); // Set origin at top-left corner
            cells[i][j].cellShape.setPoint(0, sf::Vector2f(BLOCK_WIDTH, 0));
            cells[i][j].cellShape.setPoint(1, sf::Vector2f(BLOCK_WIDTH, BLOCK_HEIGHT)); // Remeber that we are upside-down
            cells[i][j].cellShape.setPoint(2, sf::Vector2f(0, BLOCK_HEIGHT));
            cells[i][j].cellShape.setPoint(3, sf::Vector2f(0, 0));
            cells[i][j].cellShape.setPosition(sf::Vector2f(i*BLOCK_WIDTH, j*BLOCK_HEIGHT));
            cells[i][j].cellShape.setScale(sf::Vector2f(0.95, 0.95));

            cells[i][j].status = 0;
            cells[i][j].next_status = 0;
        }
    }

    // Initial shape
    // Simple glider
    cells[13][12].status = 1;
    cells[11][13].status = 1;
    cells[13][13].status = 1;
    cells[12][14].status = 1;
    cells[13][14].status = 1;

    // Block
    cells[52][53].status = 1;
    cells[52][54].status = 1;
    cells[53][53].status = 1;
    cells[53][54].status = 1;

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.scancode == sf::Keyboard::Scan::Space) {
                        paused = !paused;
                        printf("Space Pressed. Toggling pause\n");
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    cellClickedX = (int) event.mouseButton.x / BLOCK_WIDTH;
                    cellClickedY = (int) event.mouseButton.y / BLOCK_HEIGHT;
                    printf("Toggling cell (%d, %d)\n", cellClickedX, cellClickedY);
                    if (cells[cellClickedX][cellClickedY].status == 0) {
                        cells[cellClickedX][cellClickedY].status = 1;
                    } else {
                        cells[cellClickedX][cellClickedY].status = 0;
                    }
                    break;
                default:
                    break;
            }
        }

        if (!paused) {
            for (int j = 0; j < numCellsInColumn; j++) {
                for (int i = 0; i < numCellsInRow; i++) {
                    // Iterate through neighbors
                    int numLiveNeighbors = 0;
                    for (int y = -1; y < 2; y++) {
                        for (int x = -1; x < 2; x++) {
                            int neighborX = (i+x+numCellsInRow)%numCellsInRow;
                            int neighborY = (j+y+numCellsInColumn)%numCellsInColumn;
                            if (!(x == 0 && y == 0) && (cells[neighborX][neighborY].status == 1)) {
                                numLiveNeighbors += 1; 
                            }
                        }
                    }
                    // Rules 
                    if (cells[i][j].status == 1 && numLiveNeighbors < 2) {
                        cells[i][j].next_status = 0;
                    } else if (cells[i][j].status == 1 && (numLiveNeighbors == 2 || numLiveNeighbors == 3)) {
                        cells[i][j].next_status = 1;
                    } else if (cells[i][j].status == 1 && numLiveNeighbors > 3) {
                        cells[i][j].next_status = 0;
                    } else if (cells[i][j].status == 0 && numLiveNeighbors == 3) {
                        cells[i][j].next_status = 1;
                    }

                }
            }
        }

        // Update status to next_status, color, and draw
        window.clear();
        
        for (int j = 0; j < numCellsInColumn; j++) {
            for (int i = 0; i < numCellsInRow; i++) {
                if (!paused) {
                    cells[i][j].status = cells[i][j].next_status;
                    cells[i][j].next_status = 0;
                }
                if (cells[i][j].status == 1) {
                    cells[i][j].cellShape.setFillColor(sf::Color(LIVE_COLOR));
                } else {
                    cells[i][j].cellShape.setFillColor(sf::Color(DEAD_COLOR));
                }
                window.draw(cells[i][j].cellShape);
            }
        }
        
        window.display();
        sf::sleep(sf::milliseconds(SLEEP_BETWEEN_ITERATIONS_MILLISECONDS));
    }
}