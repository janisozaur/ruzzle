A simple word game.

Compile:
	`g++ wordazzle.cpp -o wordazzle -std=c++11`

Launch:
	`cat input words100 | ./wordazzle`

Input is of following format:
```
N
n_1 f_1
n_2 f_2
…
n_N f_N
W H
words
```
Where:
* `N` – count of letters
* `n_i` – _i_th letter
* `f_i` – relative frequency of _i_th letter
* `W` – output grid width
* `H` – output grid height
* `words` – words to look for in grid

Included are sample input file with [letter frequencies for english](https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language) and first 100 words from Arch's words package.

To prepare your dictionary file use:
	`grep "^[[:alpha:]]*$" /usr/share/dict/american-english | tr "[[:upper:]]" "[[:lower:]]" | sort -u > words`

See it in action (with pre-selected seed): [https://ideone.com/0MI2wh](https://ideone.com/0MI2wh)