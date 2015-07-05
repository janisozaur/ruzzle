#include <iostream>
#include <vector>
#include <random>
#include <cassert>

using namespace std;

// Letter-Frequency pair
typedef pair<char, float> pair_lf;

void print(const vector<pair_lf> &vec)
{
	for (const pair_lf item : vec)
	{
		cout << item.first << ": " << item.second << endl;
	}
}

enum class EMove
{
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW,
	Last,
	First = N,
};

inline EMove operator++( EMove& x ) { return x = (EMove)(std::underlying_type<EMove>::type(x) + 1); }
EMove operator*(EMove c) {return c;} 
EMove begin(EMove r) {return EMove::First;}
EMove end(EMove r)   {return EMove::Last;}

int move(const unsigned int &start, const unsigned int &w, const unsigned int &h, const EMove &m)
{
	switch (m)
	{
		case EMove::N:
			if (!(start < w))
				return start - w;
			break;
		case EMove::NE:
			if (!(start < w || ((start + 1) % w == 0)))
				return start - w + 1;
			break;
		case EMove::E:
			if (!((start + 1) % w == 0))
				return start + 1;
			break;
		case EMove::SE:
			if (!(((start + w) > (w * h - 1)) || ((start + 1) % w == 0)))
				return start + w + 1;
			break;
		case EMove::S:
			if (!((start + w) > (w * h - 1)))
				return start + w;
			break;
		case EMove::SW:
			if (!((start + w) > (w * h - 1) || (start % w == 0)))
				return start + w - 1;
			break;
		case EMove::W:
			if (!(start % w == 0))
				return start - 1;
			break;
		case EMove::NW:
			if (!((start % w == 0) || (start < w)))
				return start - w - 1;
			break;
	}
	return -1;
}

bool verify_word(const vector<char> &grid, const unsigned int &w,
		const unsigned int &h, const vector<bool> &visited,
		const char * const word, int start)
{
	if (start == -1)
	{
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				const int new_start = y * w + x;
				bool found = verify_word(grid, w, h, visited, word, new_start);
				if (found)
				{
					return true;
				}
			}
		}
		return false;
	}
	if (visited[start])
	{
		return false;
	}
	if (grid[start] == word[0])
	{
		if (word[1] == '\0')
		{
			return true;
		}
		const char * const new_word = word + 1;
		vector<bool> new_visited = visited;
		new_visited[start] = true;
		for (const EMove m : EMove())
		{
			int new_start = move(start, w, h, m);
			if (new_start == -1)
			{
				continue;
			}
			bool found = verify_word(grid, w, h, new_visited, new_word, new_start);
			if (found)
			{
				return true;
			}
		}
		return false;
	}
	return false;
}

int main()
{
	vector<pair_lf> letter_freqs;
	int count;
	cin >> count;
	for (int i = 0; i < count; i++)
	{
		char letter;
		float freq;
		cin >> letter >> freq;
		letter_freqs.push_back(make_pair(letter, freq / 100.f));
	}
	vector<pair_lf> bucketed_letters;
	float sum = 0.f;
	for (const pair_lf &item : letter_freqs)
	{
		sum += item.second;
		bucketed_letters.push_back(make_pair(item.first, sum));
	}
	random_device rd;
	unsigned int seed = rd();
	//seed = -740863471;
	cout << "seed: " << seed << endl;
	mt19937 gen(seed);
	vector<char> sampled_letters;
	int w, h;
	cin >> w >> h;
	const int grid_size = w * h;
	//print(bucketed_letters);
	for (int i = 0; i < grid_size; i++)
	{
		const float sample = generate_canonical<float, 10>(gen);
		int bucket = 0;
		while (sample > bucketed_letters[bucket].second)
		{
			bucket++;
		}
		assert(bucket < 26);
		const auto letter = bucketed_letters[bucket].first;
		sampled_letters.push_back(letter);
	}
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			cout << sampled_letters[y * w + x] << ' ';
		}
		cout << endl;
	}
	vector<bool> visited(grid_size, false);
	while (cin)
	{
		string word;
		cin >> word;
		const int length = word.length();
		if (length >= 4 && length < grid_size)
		{
			bool found = verify_word(sampled_letters, w, h, visited, word.c_str(), -1);
			if (found)
			{
				cout << "found " << word << endl;
			}
		}
	}
	return 0;
}
