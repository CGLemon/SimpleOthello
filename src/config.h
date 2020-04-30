#ifndef CONFIG_H_INCLUDE
#define CONFIG_H_INCLUDE


static constexpr int BOARDSIZE = 8;
static constexpr int INTERSECTION = BOARDSIZE * BOARDSIZE;
static constexpr int EBOARDSIZE = BOARDSIZE + 2;
static constexpr int NUMVERTICS = EBOARDSIZE * EBOARDSIZE;

static_assert(BOARDSIZE%2 ==0 ,"");

#endif
