# aim
- to make a working chess engine
- learn chess programming in the process
- use time effectively

# not aim
- defeat stockfish
- clone stockfish
- waste time

# log

## 30 May 2022
### night
- how to write chess engines?
- read some code
- which language to choose?
	- `python`: easy, ready-made libraries, slow
	- `js`: almost same, nice gui
	- `c++`: hard, have to learn
	- `c`: hard and complex, hard to maintain, boring?
## 31 May 2022
### night
- read about chess programming
- uci protocol
- numba+python vs c/c++? what about julia?
- started coding
- c or c++?
- c arrays vs stl arrays/vectors for 2d
- printing unicode pieces or ascii?


## 1 Jun 2022
### after 12 am
- ditching color output for now
- reading

### evening, night
- more reading
- modularizing, exceptions

## 2 Jun 2022
### after 12 am
- started writing this blog lol

### 
- started writing move generation function

## 16 Jun 2022
### after a long break 
(made a snake clone in c++ during this time, check it out!)
- still coding move generation
- moved hard-coded numbers to Enums (`Direction` and `Player`)
- refactored sliding moves (Q, B, R)
- added `pgn` output (ugly but atleast lichess can parse)
- now engine generates moves from both sides, yay!
- separated `main.cpp` into several header and cpp files

## 17 Jun 2022
### after 2 am
- added `fen` output, generates direct url to lichess analysis!

### afternoon
- repaired enpassant and castling moves
- refactored move generation function
- minor `san` notation improvements
- started legal move generation function

## 22 Jun 2022
### after 2 am
- almost completed legal move generation function
- __issue__: sometimes switches black and white during random play
- __issue__: handling castling rights and enpassant squares while undoing a move

## 23 Jun 2022
### after 12 am
- __bugfix__: `san` notation doesn't show promotion
- added `perft` and `divide` function
- now we can find corner cases easily!
## 2 am
- wasted 1 hour on why I'm getting 1 move less than expected
	reason: code generates kingside `else` queenside castling moves 🥲
## 3 am
- __bugfix__: now engine checks threatened squares during castling
- improved `perft` function
- still a lot of things to do (getting wrong `perft` results)

## 27 Jun 2022
### 3 pm
- found one corner case: castling is still allowed after rook capture!

## 9 Jul 2022
### afternoon
- fixed a corner case with promotion (earlier it always promoted to white pieces)

## 10 Jul 2022
### after 12 am
- fixed a few corner cases with castling (pawns don't threaten blank squares)
- move count fixed
- planning to automate perft
- graphics???

### afternoon
- another castling bug (capturing rook with king cleared only king's castling rights)
	reason: `else`


## 10 Sept 2022
### after a loooonngg time!!!
### 4:50 am
- there's a lot to tell you, but firstly, i'm very happy now because i just teached my ai to capture pieces and guess what, it doesn't just look one move into the future. yes guys, i wrote the negamax algorithm!! it's much more fun to play with it now. but still it has to prioritize other things besides captures, like promotions, checks, pins etc. and obviously, mates!
- now i'm going to sleep as i got many works to do :(

## 11 Oct 2022
### long time no see
### 3 am
- well, there's a lot of small and big changes. i'll try to explain by checking the diff.
- AI part:
	- replaced minimax with alpha-beta (similar to negamax version)
	- __TODO__: implement quiescence search
	- added piece-square table scoring in eval function
	- reduced branching in code
- move generation part:
	- __bugfix__: moves like `e1c1` were always interpreted as castling, fixed
	- changed board representation from `string` to `char[64]`
	- now king positions are incrementally saved, so faster to find kings
	- several optimizations in move generation
- general:
	- now engine trys to avoid moves leading to repetitions (but it is not always optimal, so should do something better)
	- fixed several gui bugs in promotion and flipping
	- gui supports (some) premoves
	- fixed some bugs in move hints, still some remaining ig
	- faster makefile
- BIGGEST UPDATES:
	- uci protocol working!
	- made a [lichess bot](https://lichess.org/@/SohamChessBot) running this engine!

## 28 Oct 2022

### 3 am
- bugfixes:
	- fixed hard to find enpassant bug!
- AI part:
	- code refactorings
	- engine understands clock!!
	- BIGGEST UPDATES:
		- iterative deepening
		- mate distance pruning
		- null move pruning
		- late move reduction
- others:
	- simplified position hash
	- engine now avoids draws!
- TODO: verify all of them, i guess i'm rushing into adding more and more features.
- i've spent around one week on this commit, now it's time for me to focus on my academics :|

## 18 Dec 2022

### 2:40 am
- planning to make a major change in the codebase: using `enum` instead of `char` for pieces. This is to
	- elude branching in code
	- improve the performance of the engine.