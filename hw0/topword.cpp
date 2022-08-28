#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>

int main(int argc, char *argv[])
{
	if (argc == 1) {
		std::cout << \
			"Please provide an argument of the form \"input=<input file name>.txt;output=<output file name>.txt\"" \
			<< std::endl;
		return 0;
	}
	std::string input(argv[1]);
	std::string input_file(
		input.substr(
			input.find('=') + 1,
			input.find(';') - input.find('=') - 1
		)
	);
	std::string second_part = input.substr(input.find(';') + 1);
	std::string output_file(
		second_part.substr(second_part.find('=') + 1)
	);
	std::ifstream input_file_stream(input_file);
	// this hash table keeps track of the count of each word
	std::map<std::string, int> count;
	// This while loop goes through all of the lines of the file until it reaches EOF (end of file)
	while (!input_file_stream.eof() && input_file_stream.is_open()) {
		std::string line(200, '\0');
		input_file_stream >> line;
		if (line[0] != '\0') {
			{
				// this scope turns the string to lowercase
				for (char &c : line)
					c = std::tolower(c);
			}
			std::vector<std::string> words;
			{
				// this checks to see if there are any strings that are separated by punctuation (any character that isn't a number or letter)
				// and counts that
				int last = 0;
				bool split = false;
				for (int i = 0; i < line.length(); ++i) {
					if (line[i] < 48 || line[i] > 122) {
						split = true;
						words.push_back(line.substr(last, i - last));
						while (i < line.length() && (line[i] < 48 || line[i] > 122)) ++i;
						last = i;
					}
					if (i == line.length() - 1 && last < line.length()) {
						words.push_back(line.substr(last));
					}
				}
			}

			// this checks to see if any numbers are present, if so then that means we ignore
			for (std::string s : words) {
				bool has_digit = false;
				for (int i = 0; i < s.length(); ++i) {
					if (s[i] >= 48 && s[i] <= 57) {
						has_digit = true;
					}
				}
				if (has_digit) {
					continue;
				}
				count[s] += 1;
			}


		}
	}
	std::ofstream output_file_stream(output_file);
	// using a priority queue so that every time we push an <int, string> pair, it takes log n time which
	// ends up being nlogn for n pairs.

	// In C++, priority_queues by default sorts by descending order and pairs are compared by their first element,
	// which means that words with a higher count will be higher in the priority queue which then means that they will come out first.
	std::priority_queue<std::pair<int, std::string>> q;
	// pushing onto priority_queue
	for (auto p : count) {
		q.push(std::pair<int, std::string>{p.second, p.first});
	}
	// we keep track of the greatest_count so that we can print out all of the tied "most frequent word".
	int greatest_count = q.size() > 0 ? q.top().first : 0;
	// vector that holds all of the most frequent tied words and sorted them alphabetically
	std::vector<std::pair<std::string, int>> top_words_sorted;
	while (q.size() > 0 && q.top().first == greatest_count) {
		top_words_sorted.push_back(std::pair<std::string, int>{q.top().second, q.top().first});
		q.pop();
	}
	// C++ uses "introsort" which is a combination of sorts that are used depending on the size of the input.
	std::sort(top_words_sorted.begin(), top_words_sorted.end());
	for (auto p : top_words_sorted) {
		output_file_stream << p.first << ' ' << p.second  << '\n';
	}
	// we close the file streams after we are done with them.
	input_file_stream.close();
	output_file_stream.close();
	return 0;
}
