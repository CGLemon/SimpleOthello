#include <iostream>
#include <memory>

#include "Text.h"
#include "GameState.h"
#include "Board.h"



void NomalLoop() {
	Text::init_all();

	auto maingame = std::make_unique<GameState>();
	maingame -> display();
	
	for(;;){
		std::cout<<"Input : "<<std::endl;
		
		auto input = std::string{};
        if (std::getline(std::cin, input)) {
			Text::text(input, *maingame);
		}
		maingame -> display();

		if (maingame -> is_gameover()) {
			const auto winner = maingame -> get_winner();
			if (winner == Board::BLACK) {
				std::cout<< "Winner : black" <<std::endl;
			} else if (winner == Board::WHITE) {
				std::cout<< "Winner : white" <<std::endl;
			} else {
				std::cout<< "Draw" <<std::endl;
			}
			std::cout << "Enter 'clear' to get new game" <<std::endl;
		}
	}
}

void VSComputerLoop() {

	Text::init_all();
	auto maingame = std::make_unique<GameState>();
	int human_color;
	maingame -> display();
	for(;;){
		auto color_str = std::string{};
		std::cout << "Enter your color...(black/white)" <<std::endl;
		std::getline(std::cin, color_str);
		if (color_str == "black" || color_str == "b") {
			human_color= Board::BLACK;
			break;
		} else if (color_str == "white" || color_str == "w") {
			human_color = Board::WHITE;
			break;
		} else if (color_str == "quit" || color_str == "q"){
			Text::text(color_str, *maingame);
		}
	}
	for(;;){
		std::cout<<"Setting input (Enter finish to quit): "<<std::endl;
		auto input = std::string{};
		if (std::getline(std::cin, input)) {
			if (input == "finish" || input == "f") {
				break;
			}
			Text::text(input, *maingame);
		}
	}

	maingame -> display();
	for(;;){
		std::cout<<"Input : "<<std::endl;
		auto input = std::string{};

		if (maingame -> board.get_to_move() == human_color) {
		    if (std::getline(std::cin, input)) {
				Text::text(input, *maingame);
			}
			maingame -> display();
		} else {
			input = "genmove";
			Text::text(input, *maingame);
			maingame -> display();
		}

		if (maingame -> is_gameover()) {
			const auto winner = maingame -> get_winner();
			if (winner == Board::BLACK) {
				std::cout<< "Winner : black" <<std::endl;
			} else if (winner == Board::WHITE) {
				std::cout<< "Winner : white" <<std::endl;
			} else {
				std::cout<< "Draw" <<std::endl;
			}
			std::cout << "Enter 'clear' to get new game" <<std::endl;
		}
	}
}



int main(int argc , char**argv) {

	
	if (argc == 1) {
		NomalLoop();
	} else if (argc >= 2) {
		const auto argv_1 = std::string{argv[1]};
		if (argv_1 == "vscomputer") {
			VSComputerLoop();
		}
	}

	return 0;
}
