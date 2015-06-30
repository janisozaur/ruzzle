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

struct GridResult
{
	bool valid;
	vector<char> grid;
};

struct RNGBase {
	virtual float operator()() = 0;
	virtual ~RNGBase() {};
};

template< class RNG >
struct SmartRNG : RNGBase {
	RNG gen;

	SmartRNG(RNG &gen) : gen(gen) {}
	virtual float operator()() {
		uniform_real_distribution<float> distribution(0.f, 1.f);
		return distribution(gen);
	}
};

char sample_letter(const vector<pair_lf> letters, float sample)
{
	int bucket = 0;
	while (sample > letters[bucket].second)
	{
		bucket++;
	}
	assert(bucket < letters.size());
	const auto letter = letters[bucket].first;
	return letter;
}

GridResult populate(const vector<char> &grid, const unsigned int &w, const unsigned int &h, const vector<pair_lf> &letter_freqs, const string &word, const int pos, RNGBase &gen)
{
	GridResult res;
	res.valid = false;
	if (pos == -1)
	{
		const unsigned int grid_size = w * h;
		vector<int> positions(grid_size);
		for (int i = 0; i < grid_size; i++)
		{
			positions[i] = i;
		}
		while (!res.valid && !positions.empty())
		{
			int rand = gen() * positions.size();
			int position = positions[rand];
			positions.erase(positions.begin() + rand);
			res = populate(grid, w, h, letter_freqs, word, position, gen);
		}
		return res;
	}
	if (grid[pos] != '\0')
	{
		return res;
	}
	vector<char> new_grid = grid;
	new_grid[pos] = word[0];
	if (word.length() == 1)
	{
		res.grid = new_grid;
		res.valid = true;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				if (res.grid[y * w + x] == '\0')
				{
					res.grid[y * w + x] = sample_letter(letter_freqs, gen());
				}
			}
		}
		return res;
	}
	const string new_word = word.substr(1);
	vector<int> positions;
	for (const EMove m : EMove())
	{
		int idx = move(pos, w, h, m);
		if (idx == -1)
		{
			continue;
		}
		positions.push_back(idx);
	}
	while (!positions.empty() && !res.valid)
	{
		int rand = gen() * positions.size();
		int position = positions[rand];
		positions.erase(positions.begin() + rand);
		res = populate(new_grid, w, h, letter_freqs, new_word, position, gen);
	}
	return res;
}

void print_grid(const vector<char> &grid, const int w, const int h)
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			cout << grid[y * w + x] << ' ';
		}
		cout << endl;
	}
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
	SmartRNG<mt19937> g(gen);
	vector<char> sampled_letters;
	int w, h;
	cin >> w >> h;
	const int grid_size = w * h;
	//print(bucketed_letters);
	for (int i = 0; i < grid_size; i++)
	{
		const float sample = generate_canonical<float, 10>(gen);
		const auto letter = sample_letter(bucketed_letters, sample);
		sampled_letters.push_back(letter);
	}
	//print_grid(sampled_letters, w, h);
	vector<bool> visited(grid_size, false);
	vector<char> grid(grid_size, '\0');
	string word;
	cin >> word;
	GridResult res;
	res = populate(grid, w, h, bucketed_letters, word, -1, g);
	if (res.valid)
	{
		print_grid(res.grid, w, h);
	} else {
		cout << "solution for word '" << word << "' not found." << endl;
	}
	while (cin)
	{
		string word;
		cin >> word;
		const int length = word.length();
		if (length >= 4 && length < grid_size)
		{
			bool found = verify_word(res.grid, w, h, visited, word.c_str(), -1);
			if (found)
			{
				cout << "found " << word << endl;
			}
		}
	}
	return 0;
}
