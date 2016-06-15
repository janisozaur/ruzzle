A simple word game.

Compile:
* C++: `g++ wordazzle.cpp -o wordazzle -std=c++11`
* Go: `go build`

Launch (same for both versions):
	`(cat input ; echo redistribution ; cat words) | ./wordazzle`

Input is of following format:
```
N
n_1 f_1
n_2 f_2
…
n_N f_N
W H
emplacement
words
```
Where:
* `N` – count of letters
* `n_i` – _i_th letter
* `f_i` – relative frequency of _i_th letter
* `W` – output grid width
* `H` – output grid height
* `emplacement` – word to emplace into the board
* `words` – words to look for in grid

Included are sample input file with [letter frequencies for english](https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language) and first 100 words from Arch's words package.

To prepare your dictionary file use:
	`grep "^[[:alpha:]]*$" /usr/share/dict/american-english | tr "[[:upper:]]" "[[:lower:]]" | sort -u > words`

See it in action (with pre-selected seed): [https://ideone.com/0MI2wh](https://ideone.com/0MI2wh)
