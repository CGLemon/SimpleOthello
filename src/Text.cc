#include "Text.h"
#include "Zobrist.h"
#include "GameState.h"
#include "Search.h"
#include "Board.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

std::vector<std::string> Text::first_cmd = 
{
	"quit"
	"play",
	"genmove",
	"auto"
};

void Text::init_all() {
	Zobrist::init_zobrist();
}


void Text::text(std::string input, GameState & state, bool quite){

	static auto search = std::make_shared<Search>(state);

	std::stringstream txt_input(input);
	std::string cmd;
	if(!(txt_input >> cmd)){
		if(!quite){
			printf("No input! \n");		
		}
	}
	
	if (cmd == "quit") {
		exit(0);
	} else if (cmd == "play"){
		std::string play_input = "";
		while(txt_input >> cmd){
			play_input += cmd; 
			play_input += " ";
		}
		if (!state.text_play_move(play_input)) {
			if(!quite) {printf("Error move! \n");}
		} else {
			state.board.exchange_to_move();
		}
	} else if (cmd == "genmove") {
		if (txt_input >> cmd) {
			int color, vtx;
			if (cmd == "black" || cmd == "b") {
				color = Board::BLACK;
			} else if (cmd == "white" || cmd == "w") {
				color = Board::WHITE;
			} else {
				if(!quite){
					printf("Unknown command! \n");		
				}
				return;
			}
			state.board.set_to_move(color);
			vtx = search -> think();
			if (vtx == Board::NOVERTEX) {
				if(!quite){
					printf("gameover! \n");		
				}
				return;
			}
			state.play_move(color, vtx);
			state.board.exchange_to_move();
		}
	} else if (cmd == "auto") {
		int color, vtx = 0;
		while (vtx != Board::NOVERTEX ) {
			color = state.board.get_to_move();
			vtx = search -> think();
			state.play_move(color, vtx);
			state.board.exchange_to_move();
			state.display();
		}
		if(!quite){
			printf("gameover! \n");		
		}
		

	} else {
		if(!quite){
			printf("Unknown command! \n");		
		}
	} 
}
