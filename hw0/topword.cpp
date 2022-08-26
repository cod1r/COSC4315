#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>

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
	while (!input_file_stream.eof()) {
		std::string line(200, '\0');
		input_file_stream >> line;
		if (line[0] != '\0') {
			{
				// this scope checks to see if numbers and letters are mixed
				bool has_alpha = false;
				bool has_digit = false;
				for (int i = 0; i < line.length(); ++i) {
					if (line[i] >= 97 && line[i] <= 122)
						has_alpha = true;
					else if (line[i] >= 48 && line[i] <= 57) {
						has_digit = true;
					}
				}
				if (has_alpha && has_digit)
					continue;
			}

			{
				// this scope turns the string to lowercase
				for (char &c : line)
					c = std::tolower(c);
			}

			{
				// this checks to see if there are any strings that are separated by punctuation (any character that isn't a number or letter)
				// and counts that
				int last = 0;
				bool split = false;
				for (int i = 0; i < line.length(); ++i) {
					if (line[i] < 48 || line[i] > 122) {
						split = true;
						count[line.substr(last, i - last)] += 1;
						while (line[i] < 48 || line[i] > 122) ++i;
						last = i;
					}
					if (i == line.length() - 1) {
						count[line.substr(last)] += 1;
					}
				}
				if (!split)
					count[line] += 1;
			}
		}
	}
	std::ofstream output_file_stream(output_file);
	// vector so that can sort the words in O(nlogn) time
	std::vector<std::pair<std::string, int>> words_sorted;
	// puts words in vector so we can sort
	for (auto p : count) {
		words_sorted.push_back(std::pair<std::string, int>{p.first, p.second});
	}
	// sorts the words alphabetically
	std::sort(words_sorted.begin(), words_sorted.end());
	for (auto p : words_sorted) {
		output_file_stream << p.first << ' ' << p.second << '\n';
	}
	input_file_stream.close();
	output_file_stream.close();
	return 0;
}
