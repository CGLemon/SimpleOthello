#ifndef CONFIG_H_INCLUDE
#define CONFIG_H_INCLUDE


static constexpr int BOARD_SIZE = 8;
static constexpr int NUM_INTERSECTIONS = BOARD_SIZE * BOARD_SIZE;
static constexpr int EBOARDSIZE = BOARD_SIZE + 2;
static constexpr int NUM_VERTICES = EBOARDSIZE * EBOARDSIZE;

static_assert(BOARD_SIZE%2 ==0 ,"");

#endif
