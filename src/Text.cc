#include "Text.h"
#include "Zobrist.h"
#include "GameState.h"
#include "Search.h"
#include "Board.h"
#include "config.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <iostream>


std::vector<std::string> Text::first_cmd = 
{
	"quit"   ,
	"play"   ,
	"genmove",
	"auto"   ,
	"clear"  ,
	"undo"   ,
	"resign" ,
	"pass"
};

void Text::init_all() {
	Zobrist::init_zobrist();
}


void Text::text(std::string & input, GameState & state, bool quite){

	static auto search = std::make_shared<Search>(state);

	std::stringstream txt_input(input);
	std::string cmd;
	if(!(txt_input >> cmd)){
		if(!quite){
			printf("No input! \n");	
		}
		return;
	}
	
	if (cmd == "quit" || cmd == "q") {
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
		} else {
			const auto vtx = search -> think();
			const auto nowcolor = state.board.get_to_move();
			if (vtx == Board::NOVERTEX) {
				if(!quite){
					printf("gameover! \n");		
				}
				return;
			}
			state.play_move(nowcolor, vtx);
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

	} else if (cmd == "PASS" || cmd == "pass") {
		const auto nowcolor = state.board.get_to_move();
		if (!state.play_move(nowcolor, Board::PASS) && !quite) {
			printf("disable pass!\n");
		}

	} else if (cmd == "Resign" || cmd == "resign") {
		if(!quite){
			printf("gameover! \n");		
		}
		const auto color = state.board.get_to_move();
		state.one_resign(color);

	} else if (cmd == "Undo" || cmd == "undo") {
		const auto res = state.undo_move();
		if (!res && !quite) {
			printf("disable undo!\n");
		}
	} else if (cmd == "clear" || cmd == "reset") {
		state.init_game(BOARDSIZE);
		search -> clear();
	} else if (cmd == "settime") {

		auto time_str = std::string{};
		int seconds; 
		if (txt_input >> cmd) {
			const int cmd_size = cmd.size();
			for (int i = 0; i < cmd_size; ++i) {
				if (cmd[i] >= 48 && cmd[i] <= 57) {
					time_str += cmd[i];
				} else {
					return;			
				}
			}
			seconds = std::stoi(time_str);
		}
		if (txt_input >> cmd) {
			if(!quite){
				printf("Unknown command! \n");		
			}
			return;
		}
		search -> set_time(seconds);

	} else if (cmd == "setdeep") {

		auto search_deep_str = std::string{};
		int search_deep; 
		if (txt_input >> cmd) {
			const int cmd_size = cmd.size();
			for (int i = 0; i < cmd_size; ++i) {
				if (cmd[i] >= 48 && cmd[i] <= 57) {
					search_deep_str += cmd[i];
				} else {
					return;			
				}
			}
			search_deep = std::stoi(search_deep_str);
		}
		if (txt_input >> cmd) {
			if(!quite){
				printf("Unknown command! \n");		
			}
			return;
		}	
		search -> set_maxdeep(search_deep);

	} else {
		if(!quite){
			printf("Unknown command! \n");		
		}
	} 
}
