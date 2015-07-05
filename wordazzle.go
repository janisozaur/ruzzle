// wordazzle.go
package main

import (
	"fmt"
	"math/rand"
	"unicode/utf8"
)

type EMove int

const (
	N EMove = 1 + iota
	NE
	E
	SE
	S
	SW
	W
	NW
)

func move(start, w, h int, m EMove) int {
	switch m {
	case N:
		if !(start < w) {
			return start - w
		}
	case NE:
		if !(start < w || ((start+1)%w == 0)) {
			return start - w + 1
		}
	case E:
		if !((start+1)%w == 0) {
			return start + 1
		}
	case SE:
		if !(((start + w) > (w*h - 1)) || ((start+1)%w == 0)) {
			return start + w + 1
		}
	case S:
		if !((start + w) > (w*h - 1)) {
			return start + w
		}
	case SW:
		if !((start+w) > (w*h-1) || (start%w == 0)) {
			return start + w - 1
		}
	case W:
		if !(start%w == 0) {
			return start - 1
		}
	case NW:
		if !((start%w == 0) || (start < w)) {
			return start - w - 1
		}
	}
	return -1
}

type pair_lf struct {
	letter rune
	freq   float32
}

func populate(grid []rune, w, h int, letter_freqs []pair_lf, word string, pos int) ([]rune, bool) {
	valid := false
	var newgrid []rune
	if pos == -1 {
		grid_size := w * h
		positions := make([]int, grid_size)
		for i := 0; i < grid_size; i++ {
			positions[i] = i
		}
		for !valid && len(positions) > 0 {
			r := int(rand.Float32() * float32(len(positions)))
			position := positions[r]
			positions_new := append(positions[:r], positions[r+1:]...)
			positions = positions_new
			newgrid, valid = populate(grid, w, h, letter_freqs, word, position)
		}
		return newgrid, valid
	}
	if grid[pos] != 0 {
		return nil, false
	}
	newgrid = make([]rune, len(grid))
	copy(newgrid, grid)
	newrune, runesize := utf8.DecodeRuneInString(word)
	newgrid[pos] = newrune
	if utf8.RuneCountInString(word) == 1 {
		valid = true
		for y := 0; y < h; y++ {
			for x := 0; x < w; x++ {
				idx := y*w + x
				if newgrid[idx] == 0 {
					sample := rand.Float32()
					newgrid[idx] = sample_letter(letter_freqs, sample)
				}
			}
		}
		return newgrid, valid
	}
	newword := word[runesize:]
	positions := make([]int, 0)
	for m := N; m < NW; m++ {
		idx := move(pos, w, h, m)
		if idx == -1 {
			continue
		}
		positions = append(positions, idx)
	}
	for len(positions) > 0 && !valid {
		var resultgrid []rune
		r := int(rand.Float32() * float32(len(positions)))
		position := positions[r]
		positions = append(positions[:r], positions[r+1:]...)
		resultgrid, valid = populate(newgrid, w, h, letter_freqs, newword, position)
		if valid {
			newgrid = resultgrid
		}
	}
	return newgrid, valid
}

func sample_letter(letters []pair_lf, sample float32) rune {
	bucket := 0
	for sample > letters[bucket].freq {
		bucket++
	}
	return letters[bucket].letter
}

func print_grid(letters []rune, w, h int) {
	for y := 0; y < h; y++ {
		fmt.Printf("%q\n", letters[y * w: y * w + w])
	}
}

func verify_word(grid []rune, w, h int, visited []bool, word string, start int) bool {
	if start == -1 {
		if !utf8.ValidString(word) {
			return false
		}
		size := utf8.RuneCountInString(word)
		if size > len(grid) {
			return false
		}
		for y := 0; y < h; y++ {
			for x := 0; x < w; x++ {
				idx := y * w + x
				found := verify_word(grid, w, h, visited, word, idx)
				if found {
					return found
				}
			}
		}
		return false
	}
	if visited[start] {
		return false
	}
	newrune, runesize := utf8.DecodeRuneInString(word)
	if grid[start] == newrune {
		if utf8.RuneCountInString(word) == 1 {
			return true
		}
		newword := word[runesize:]
		newvisited := make([]bool, len(visited))
		copy(newvisited, visited)
		newvisited[start] = true
		for m := N; m < NW; m++ {
			new_start := move(start, w, h, m)
			if new_start == -1 {
				continue
			}
			found := verify_word(grid, w, h, newvisited, newword, new_start)
			if found {
				return true
			}
		}
	}
	return false
}

func main() {
	var count int
	fmt.Scanln(&count)
	letter_freqs := make([]pair_lf, 0, count)
	var rolling_sum float32
	for i := 0; i < count; i++ {
		var freq float32
		var input string
		fmt.Scanln(&input, &freq)
		if !(utf8.ValidString(input) && utf8.RuneCountInString(input) == 1) {
			panic(input + " is not a valid input")
		}
		r, _ := utf8.DecodeRuneInString(input)
		rolling_sum += freq / 100
		newletter := pair_lf{r, rolling_sum}
		letter_freqs = append(letter_freqs, newletter)
	}
	var w, h, grid_size int
	fmt.Scanln(&w, &h)
	grid_size = w * h
	grid_template := make([]rune, grid_size)
	var word string
	fmt.Scanln(&word)
	seed := int64(0)
	rand.Seed(seed)
	if utf8.RuneCountInString(word) > grid_size {
		panic("Word " + word + " is too large for given grid dimensions")
	}
	fmt.Printf("Trying to put word %s\n", word)
	grid, valid := populate(grid_template, w, h, letter_freqs, word, -1)
	if valid != true {
		panic("No placement for word " + word + " and seed " + string(seed) + " could be found")
	}
	print_grid(grid, 4, 4)
	visited := make([]bool, grid_size)
	found := verify_word(grid, w, h, visited, word, -1)
	if !found {
		panic("word " + word + " was NOT found")
	}
	var str string
	var err error
	err = nil
	for ; err == nil ; _, err = fmt.Scanln(&str) {
		found := verify_word(grid, w, h, visited, str, -1)
		if found {
			fmt.Println("found word " + str)
		}
	}
}
