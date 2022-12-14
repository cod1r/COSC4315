#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>

bool validate_input(std::string input)
{
	if (input.find('=') == std::string::npos 
			|| input.find(';') == std::string::npos 
			|| input.find("input") == std::string::npos 
			|| input.find("output") == std::string::npos) {
		return false;
	}
	int semicolon_count = 0;
	int equalsign_count = 0;
	for (char c : input) {
		if (c == ';')
			++semicolon_count;
		else if (c == '=')
			++equalsign_count;
	}
	std::string::size_type input_start = input.find("input");
	std::string::size_type output_start = input.find("output");
	if (input.find(';') + 1 >= input.length() 
			|| input.find('=') + 1 >= input.length() 
			|| input.find(';') - input.find('=') + 1 <= 0 
			|| input.find(';') < input_start 
			|| input.find('=') < input_start) {
		return false;
	}
	std::string input_file_name = input.substr(input.find('=') + 1, input.find(';') - input.find('=') - 1);
	std::string second_part = input.substr(input.find(';') + 1);
	if (second_part.length() == 0 || second_part.find('=') + 1 >= second_part.length()) {
		return false;
	}
	std::string output_file_name = second_part.substr(second_part.find('=') + 1);
	if (equalsign_count != 2 || semicolon_count != 1 || input_start >= output_start || input_file_name.length() == 0 || output_file_name.length() == 0) {
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << \
			"Please provide an argument of the form \"input=<input file name>.txt;output=<output file name>.txt\" and run the program as ./topword and provide an argument in the form previously described" \
			<< std::endl;
		return 0;
	}
	std::string input(argv[1]);
	// checks to see if the argument provided is in the form specified
	if (!validate_input(input)) {
		std::cout << \
			"Please provide an argument of the form \"input=<input file name>.txt;output=<output file name>.txt\" and run the program as ./topword and provide an argument in the form previously described" \
			<< std::endl;
		return 0;
	}
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
				for (int i = 0; i < line.length(); ++i) {
					if (line[i] < 48 || (line[i] > 57 && line[i] < 65) || (line[i] > 90 && line[i] < 97) || line[i] > 122) {
						if (i - last > 0) {
							words.push_back(line.substr(last, i - last));
						}
						while (i < line.length() && (line[i] < 48 || (line[i] > 57 && line[i] < 65) || (line[i] > 90 && line[i] < 97) || line[i] > 122)) ++i;
						last = i;
					}
					if (i == line.length() - 1 && last < line.length()) {
						words.push_back(line.substr(last));
					}
				}
			}

			// this checks to see if any numbers are present in the strings that we have found in the line, if so then that means we ignore
			// if not, we increment the counter
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
