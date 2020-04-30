# SimpleOthello
A simple Othello AI with Alpha-Beta pruning.
A simple text gui just for fun!
![](https://github.com/CGLemon/SimpleOthello/blob/master/Screenshot%20from%202020-04-30%2001-46-59.png)

# Features
- [ ] With Alpha-Beta pruning.
- [ ] With Zobrist hash.
- [ ] With incomplete text protocols like GTP.

# How to compile (Ubuntu)
## Install CMake
    $ sudo apt install cmake
## compile
    $ cd SimpleOthello
    $ mldir build
    $ cd build
    $ cmake ..
    $ make 
## or use ohter compiler
    $ g++ ../src/*.cc -o SimpleOthello -O3 -lpthread
    $ clang++ ../src/*.cc -o SimpleOthello -O3 -lpthread
    
    
# How to play

    $ ./SimpleOthello
    
## Play black move
    $ play black d3
    $ play black d3
    $ play d3
    
## Play white move
    $ play white e3
    $ play white e3
    $ play e3
    
## AI move 
    $ genmove black
    $ genmove white
    
## AI Selfpaly
    $ auto
## Quit 
    $ quit

# Todo
- [ ] Add more patten to improve evaluation function.
- [ ] Add multi threads.
- [ ] Add other pruning motheds to speed up.
- [ ] Using bitboard.
