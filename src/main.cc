#include <iostream>
#include <memory>

#include "Text.h"
#include "GameState.h"
#include "Board.h"

int main(int argc , char**argv) {

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
			break;
		}
	}
	const auto winner = maingame -> get_winner();
	if (winner == Board::BLACK) {
		std::cout<< "Winner : black" <<std::endl;
	} else if (winner == Board::WHITE) {
		std::cout<< "Winner : white" <<std::endl;
	} else {
		std::cout<< "Draw" <<std::endl;
	}
	
	
	return 0;
}
