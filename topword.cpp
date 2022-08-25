#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc == 1) {
		std::cout << "Please provide an argument of the form \"input=<input file name>.txt;output=<output file name>.txt\"" << std::endl;
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
			count[line] += 1;
		}
	}
	std::ofstream output_file_stream(output_file);
	for (auto p : count) {
		output_file_stream << p.first << ' ' << p.second << '\n';
	}
	input_file_stream.close();
	output_file_stream.close();
	return 0;
}
