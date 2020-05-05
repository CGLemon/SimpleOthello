#ifndef PARSER_H_INCLUDE
#define PARSER_H_INCLUDE

#include "GameState.h"
#include <string>

struct ParserInformation {


};

class Parser {
	
/*
	template 

[Event "?"]
[Site "PlayOK"]
[Date "2020.05.05"]
[Round "-"]
[Black "t7m4o1k9a"]
[White "cafego"]
[Result "38-26"]
[Time "11:24:48"]
[TimeControl "300"]
[BlackElo "1073"]
[WhiteElo "1183"]

1. f5 d6 2. c3 f3 3. f4 d3 4. c4 g3 5. e6 c6 6. c5 e7 7. f6 g4 8. g5 h4 9. e8
d7 10. d2 b5 11. d8 e3 12. e2 f2 13. h6 h5 14. h3 b4 15. g6 e1 16. g1 f7 17. f8
g7 18. a5 f1 19. d1 c1 20. b1 c2 21. h8 g2 22. a4 b3 23. a3 a6 24. a7 b6 25. c7
c8 26. b2 b7 27. a8 b8 28. h1 h2 29. -- g8 30. h7 a2 31. a1 38-26


*/

public:
	static std::string simple_pgn(GameState & state);

	static std::string parser_vertex(const int vtx, const int boardsize);

private:





};

#endif
