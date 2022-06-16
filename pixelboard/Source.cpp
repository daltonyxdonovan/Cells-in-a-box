#include <SFML/Graphics.hpp>
#include <atlstr.h>
#include <vector>
#include <iostream>

//in the standard namespace
using namespace std;

//variables
const int width{ 500 };
const int move_speed{ 5 };
const int height{ 500 };
const int spacing{ 8 };
const int limiter{ 20 };

int generation_limit{ 0 };
int tick{ 0 };
int cell_size{ 3 };
int restarted{ -1 };
int successes{ 0 };

bool spawning{ false };

//cell class
class Cell {

public:
	Cell() {
		x = 0;
		y = 0;
		
		move_up = false;
		move_down = false;
		move_left = false;
		move_right = false;
		
		lifespan = rand()%2000;
		deathspan = 50;
		alive = true;
		sf::Rect bounding_box = sf::FloatRect(x, y, cell_size, cell_size);
		
	}
	Cell(int x, int y) {
		this->x = x;
		this->y = y;
		
		move_up = false;
		move_down = false;
		move_left = false;
		move_right = false;
		
		lifespan = rand() % 2000;;
		deathspan = 50;
		alive = true;
		sf::Rect bounding_box = sf::FloatRect(x, y, cell_size, cell_size);
	}
	
	int x;
	int y;
	long int lifespan;
	long int deathspan;
	bool alive;
	
	bool move_up = false;
	bool move_down = false;
	bool move_left = false;
	bool move_right = false;
	
	sf::FloatRect bounding_box = sf::FloatRect(x, y, cell_size, cell_size);
	

	void setAlive(bool alive) {
		this->alive = alive;
	}
	bool getAlive() {
		return alive;
	}
	//every cell should be a square
	int getWidth() {
		return width;
	}
	int getHeight() {
		return height;
	}

	
	
	void draws(sf::RenderWindow& window) {
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(cell_size, cell_size));
		rectangle.setPosition(x, y);
		if (alive) {
			lifespan--;
			if (lifespan < 1)
				alive = false;
			rectangle.setFillColor(sf::Color::Green);
		}
		else {
			deathspan--;
			rectangle.setSize(sf::Vector2f(cell_size*2, cell_size*2));
			rectangle.setFillColor(sf::Color::Red);
		}
		
		int chance = rand() % 100;
		if (chance < 20)
		{
			move_up = true;
			move_down = false;
			move_left = false;
			move_right = false;
		}
		else if (chance < 40 && chance > 19)
		{
			move_up = false;
			move_down = true;
			move_left = false;
			move_right = false;
		}
		else if (chance < 60 && chance > 39)
		{
			move_up = false;
			move_down = false;
			move_left = true;
			move_right = false;
		}
		else if (chance < 80 && chance > 59)
		{
			move_up = false;
			move_down = false;
			move_left = false;
			move_right = true;
		}
		else
		{
			move_up = false;
			move_down = false;
			move_left = false;
			move_right = false;
		}
		if (move_up && alive)
			y -= move_speed;
		if (move_down && alive)
			y += move_speed;
		if (move_left && alive)
			x -= move_speed;
		if (move_right && alive)
			x += move_speed;
		

		window.draw(rectangle);
	}
};

//food class
class Food {

public:
	Food() {
		x = 0;
		y = 0;
		sf::Rect bounding_box = sf::FloatRect(x, y, cell_size * 3, cell_size * 3);
	}
	
	Food(int x, int y) {
		this->x = x;
		this->y = y;
		sf::Rect bounding_box = sf::FloatRect(x, y, cell_size * 3, cell_size * 3);
	}

	int x;
	int y;
	sf::FloatRect bounding_box = sf::FloatRect(x, y, cell_size * 3, cell_size * 3);
	
	void draws(sf::RenderWindow& window) {
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(cell_size * 3, cell_size * 3));
		rectangle.setPosition(x, y);
		rectangle.setFillColor(sf::Color::Blue);
		window.draw(rectangle);
	}
	
};

vector<Cell> cells;
vector<Food> foods;

//hide terminal
void hideConsole() {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
}

int main()
{
	//hide the damned console because i ticked the wrong box in visual studio
	//hideConsole();
	
	//seed random number gen
	srand(time(NULL));

	//set up window
	sf::RenderWindow window(sf::VideoMode(width, height), "pixelb0ard");
	
	//set repeat keys to true
	window.setKeyRepeatEnabled(true);

	//create event handler
	sf::Event event;
	
	//create font and handle error if it doesn't work
	sf::Font font;
	if (!font.loadFromFile("Cascadia.ttf"))
	{
		cout << "font not loaded" << endl;
	}

	//ticker for total cells
	sf::Text cell_count;
	cell_count.setFont(font);
	cell_count.setCharacterSize(15);
	cell_count.setPosition((5), 480);
	cell_count.setFillColor(sf::Color::White);
	cell_count.setOutlineColor(sf::Color::Black);
	cell_count.setOutlineThickness(1);

	//ticker for failures
	sf::Text restart_count;
	restart_count.setFont(font);
	restart_count.setCharacterSize(15);
	restart_count.setPosition((5), 460);
	restart_count.setFillColor(sf::Color::White);
	restart_count.setOutlineColor(sf::Color::Black);
	restart_count.setOutlineThickness(1);
	
	//ticker for sanitization
	sf::Text success_count;
	success_count.setFont(font);
	success_count.setCharacterSize(15);
	success_count.setPosition((5), 440);
	success_count.setFillColor(sf::Color::White);
	success_count.setOutlineColor(sf::Color::Black);
	success_count.setOutlineThickness(1);
	
	//look for events while the window is open
	while (window.isOpen())
	{
		//if cells exceed limit, sanitize
		if (cells.size() > 8000)
		{
			successes++;
			cells.clear();
			Cell cell1(250, 250);
			Cell cell2(240, 250);
			cells.push_back(cell1);
			cells.push_back(cell2);
		}

		//list info
		generation_limit = 0;
		cell_count.setString(to_string(cells.size()) + " cells total (dead and alive)");
		restart_count.setString(to_string(restarted) + " restarts total (colony died)");
		success_count.setString(to_string(successes) + " total sanitizations (too many cells)");

		//get mouse position
		int mouse_y = sf::Mouse::getPosition(window).y;
		int mouse_x = sf::Mouse::getPosition(window).x;
		
		//if mouse is clicked, create a cell
		if (spawning)
		{
			Cell cell(mouse_x, mouse_y);
			cells.push_back(cell);
		}
		
		//controls
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				if (event.key.code == sf::Keyboard::Space)
				{
					cells.clear();
					foods.clear();
				}
					
					
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					spawning = true;
				}
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					Food food(mouse_x, mouse_y);
					foods.push_back(food);
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{	
					spawning = false;
				}
			}
			
			//if mouse wheel is scrolled up
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (event.mouseWheelScroll.delta > 0)
				{
					if (cell_size < 5)
						cell_size++;
				}
				if (event.mouseWheelScroll.delta < 0)
				{
					if (cell_size > 1)
						cell_size--;
				}
			}
		}
		
		//clear the window
		window.clear(sf::Color::Black);

		//if there are no cells, spawn two cells
		if (cells.size() < 2)
		{
			restarted++;
			Cell cell1(250, 250);
			/*Cell cell2(240, 250);*/
			cells.push_back(cell1);
			/*cells.push_back(cell2);*/
		}
		
		//for every Food in foods, draw a food at it's x and y
		for (int i = 0; i < foods.size(); i++)
		{
			foods[i].draws(window);
		}

		//for every Cell in cells, draw a cell at it's x and y
		for (int i = 0; i < cells.size(); i++)
		{
			cells[i].draws(window);
			if (cells[i].x < 0)
				cells[i].x = 0;
			if (cells[i].x > width)
				cells[i].x = width;
			if (cells[i].y < 0)
				cells[i].y = 0;
			if (cells[i].y > height)
				cells[i].y = height;
			if (cells[i].deathspan < 1)
				cells.erase(cells.begin() + i);
			
			//if the bounding boxes of two cells touch, spawn a new cell
			for (int j = 0; j < cells.size(); j++)
			{
				//update the bounding box
				cells[i].bounding_box.left = cells[i].x;
				cells[i].bounding_box.top = cells[i].y;
				cells[i].bounding_box.width = cell_size;
				cells[i].bounding_box.height = cell_size;
				
				
				//if the cell isn't itself, and it hasn't hit gen limit yet then it can have babby
				if (i != j && generation_limit < limiter)
				{
					//it has to touch another tho
					if (cells[i].bounding_box.intersects((cells[j].bounding_box)) && cells[i].alive == true && cells[j].alive == true && cells[i].lifespan < 2000 && cells[j].lifespan < 2000)
					{
						generation_limit++;
						Cell cell(cells[i].x += (rand() % 19) - 9, cells[j].y += (rand() % 19) - 9);
						cells.push_back(cell);
						Cell cell2(cells[i].x += (rand() % 19) - 9, cells[j].y += (rand() % 19) - 9);
						cells.push_back(cell2);
					}
				}
				
				//if a cell touches a dead cell, set cells alive to false
				if (cells[i].bounding_box.intersects((cells[j].bounding_box)) && cells[i].alive == true && cells[j].alive == false)
				{
					cells[i].x += (rand() % 19) - 9;
					cells[j].y += (rand() % 19) - 9;
					cells[i].alive = false;
					cells[i].lifespan = 0;
				}				
				
			}
			for (int j = 0; j < foods.size(); j++)
			{
				if (foods[j].x < cells[i].x && cells[i].alive == true)
					cells[i].x -= 5;
				if (foods[j].x > cells[i].x && cells[i].alive == true)
					cells[i].x += 5;
				if (foods[j].y < cells[i].y && cells[i].alive == true)
					cells[i].y -= 5;
				if (foods[j].y > cells[i].y && cells[i].alive == true)
					cells[i].y += 5;
				foods[j].bounding_box.left = foods[j].x;
				foods[j].bounding_box.top = foods[j].y;
				foods[j].bounding_box.width = cell_size;
				foods[j].bounding_box.height = cell_size;
				
				//if a cell touches a food cell, spawn a new cell
				if (cells[i].bounding_box.intersects((foods[j].bounding_box)) && cells[i].alive == true)
				{
					Cell cell(cells[i].x += (rand() % 19) - 9, cells[i].y += (rand() % 19) - 9);
					cells.push_back(cell);
					Cell cell2(cells[i].x += (rand() % 19) - 9, cells[i].y += (rand() % 19) - 9);
					cells.push_back(cell2);
				}
				if (cells[i].bounding_box.intersects((foods[j].bounding_box)) && cells[i].alive == false)
				{
					cells[i].alive = true;
				}
			}
				
		}
		
		//draw the tickers with the info
		window.draw(cell_count);
		
		window.draw(success_count);
	
		//update and limit the window
		window.setFramerateLimit(30);
		window.display();
	}
	//return okay if the program exits properly
    return 0;
}