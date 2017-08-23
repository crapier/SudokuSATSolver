#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

// output formatting 
string negation = "-";
string num_seperator = "_";
string lit_seperator = " ";
string conj_seperator = " 0\n";
string prefix = "cell";
string comment_prefix = "c";

string zchaff_problem = "p";
string zchaff_type = "cnf";

// output extension
char * cnf_file_extension = "_CNF.kb";

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Please provide sudoku input file.\n");
		return 1;
	}

	// open input sudoku file
	fstream file_in;
	file_in.open(argv[1], fstream::in);

	// error check
	if (!file_in.is_open()) {
		printf("Cannot open input file %s.\n", argv[1]);
		return 1;
	}

	// read file in
	stringstream buffer;
	buffer << file_in.rdbuf();

	// copy string stream to string for parsing
	string input = buffer.str();

	// close input file
	file_in.close();

	// copy input filename
	char * output_filename = new char[strlen(argv[1]) + strlen(cnf_file_extension) + 1];
	strcpy(output_filename, argv[1]);
	// remove previous extension
	char * extension = strchr(output_filename, '.');
	if (extension != NULL) {
		*extension = NULL;
	}
	// add cnf extension
	strcat(output_filename, cnf_file_extension);


	// maps for lookup when creating conjunctions
	map<pair<int, int>, int> existing_values;
	map<int, vector<int>> existing_values_row;
	map<int, vector<int>> existing_values_column;
	map<pair<int, int>, vector<int>> existing_values_box;

	// parse input
	int row = 1;
	int column = 1;
	int grid_size = 0;


	// find value for n and grid size
	string first_row = input.substr(0, input.find('\n'));
	for (size_t i = 0; i < first_row.length(); i++) {
		if (first_row[i] == '.' || (input[i] >= '1' && input[i] <= '9') || (input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z')) {
			// increment grid size
			grid_size++;
		}
		// ignore everything else
	}
	int n = (int)sqrt(grid_size);

	if (n*n != grid_size) {
		printf("Input is not n^2 x n^2.\n");
		return 1;
	}

	// output string
	string output = "";
	int conjunction_count = 0;

	for (size_t i = 0; i < input.length(); i++) {
		if (input[i] == '.') {
			// increment column
			column++;
		}
		else if (input[i] == '\n') {
			// increment row
			row++;
			// reset column
			column = 1;
		}
		else if (input[i] >= '1' && input[i] <= '9') {
			// add value to maps
			existing_values[pair<int, int>(row, column)] = input[i] - '1' + 1;
			existing_values_row[row].push_back(input[i] - '1' + 1);
			existing_values_column[column].push_back(input[i] - '1' + 1);
			existing_values_box[pair<int, int>((row - 1) / n + 1, (column - 1) / n + 1)].push_back(input[i] - '1' + 1);

			// print conjunction to file
			//file_out << prefix << num_seperator << row << num_seperator << column << num_seperator << input[i] - '1' + 1 << conj_seperator;

			char lit[21];
			itoa(grid_size * (grid_size * (row - 1) + (column - 1)) + (input[i] - '1' + 1), lit, 10);
			output += lit;
			output += conj_seperator;
			conjunction_count++;

			// increment column
			column++;
		}
		else if (input[i] >= 'a' && input[i] <= 'z') {
			// add value to maps
			existing_values[pair<int, int>(row, column)] = input[i] - 'a' + 10;
			existing_values_row[row].push_back(input[i] - 'a' + 10);
			existing_values_column[column].push_back(input[i] - 'a' + 10);
			existing_values_box[pair<int, int>((row - 1) / n + 1, (column - 1) / n + 1)].push_back(input[i] - 'a' + 10);

			// print conjunction to file
			//file_out << prefix << num_seperator << row << num_seperator << column << num_seperator << input[i] - 'a' + 10 << conj_seperator;

			char lit[21];
			itoa(grid_size * (grid_size * (row - 1) + (column - 1)) + (input[i] - 'a' + 10), lit, 10);
			output += lit;
			output += conj_seperator;
			conjunction_count++;

			// increment column
			column++;
		}
		else if (input[i] >= 'A' && input[i] <= 'Z') {
			// add value to maps
			existing_values[pair<int, int>(row, column)] = input[i] - 'A' + 10;
			existing_values_row[row].push_back(input[i] - 'A' + 10);
			existing_values_column[column].push_back(input[i] - 'A' + 10);
			existing_values_box[pair<int,int>((row - 1) / n + 1, (column - 1) / n + 1)].push_back(input[i] - 'A' + 10);

			// print conjunction to file
			//file_out << prefix << num_seperator << row << num_seperator << column << num_seperator << input[i] - 'A' + 10 << conj_seperator;

			char lit[21];
			itoa(grid_size * (grid_size * (row - 1) + (column - 1)) + (input[i] - 'A' + 10), lit, 10);
			output += lit;
			output += conj_seperator;
			conjunction_count++;

			// increment column
			column++;
		}
		// ignore everything else
	}

	// sort row, column vectors in maps
	for (map<int, vector<int>>::iterator it = existing_values_row.begin(); it != existing_values_row.end(); it++) {
		sort(it->second.begin(), it->second.end());
	}
	for (map<int, vector<int>>::iterator it = existing_values_column.begin(); it != existing_values_column.end(); it++) {
		sort(it->second.begin(), it->second.end());
	}

	// each cell must have at least 1 value
	for (int r = 1; r <= grid_size; r++) {
		for (int c = 1; c <= grid_size; c++) {
			if (existing_values.find(pair<int,int>(r,c)) == existing_values.end()) {
				for (int val = 1; val <= grid_size; val++) {
					//file_out << prefix << num_seperator << r << num_seperator << c << num_seperator << val;

					char lit[21];
					itoa(grid_size * (grid_size * (r - 1) + (c - 1)) + val, lit, 10);
					output += lit;	

					if (val == grid_size) {
						//file_out << conj_seperator;
						output += conj_seperator;

						conjunction_count++;
					}
					else {
						//file_out << lit_seperator;
						output += lit_seperator;
					}

				}
			}
		}
	}

	// each cell should only contain one value
	for (int r = 1; r <= grid_size; r++) {
		for (int c = 1; c <= grid_size; c++) {
			if (existing_values.find(pair<int, int>(r, c)) == existing_values.end()) {
				for (int val = 1; val <= grid_size; val++) {
					for (int val2 = val + 1; val2 <= grid_size; val2++) {
						//file_out << negation << prefix << num_seperator << r << num_seperator << c << num_seperator << val << lit_seperator;

						char lit[21];
						itoa(grid_size * (grid_size * (r - 1) + (c - 1)) + val, lit, 10);
						output += negation;
						output += lit;
						output += lit_seperator;

						//file_out << negation << prefix << num_seperator << r << num_seperator << c << num_seperator << val2 << conj_seperator;

						char lit2[21];
						itoa(grid_size * (grid_size * (r - 1) + (c - 1)) + val2, lit2, 10);
						output += negation;
						output += lit2;
						output += conj_seperator;

						conjunction_count++;
					}
				}
			}
		}
	}

	// each row should only contain each value once
	for (int r = 1; r <= grid_size; r++) {
		for (int c = 1; c <= grid_size; c++) {
			map<pair<int, int>, int>::iterator cell_check = existing_values.find(pair<int, int>(r, c));
			if (cell_check == existing_values.end()) {
				map<int, vector<int>>::iterator check_column = existing_values_column.find(c);
				for (int val = 1; val <= grid_size; val++) {
					if (check_column == existing_values_column.end() || !binary_search(check_column->second.begin(), check_column->second.end(), val)) {
						for (int c2 = c + 1; c2 <= grid_size; c2++) {
							//file_out << negation << prefix << num_seperator << r << num_seperator << c << num_seperator << val << lit_seperator;

							char lit[21];
							itoa(grid_size * (grid_size * (r - 1) + (c - 1)) + val, lit, 10);
							output += negation;
							output += lit;
							output += lit_seperator;

							//file_out << negation << prefix << num_seperator << r << num_seperator << c2 << num_seperator << val << conj_seperator;

							char lit2[21];
							itoa(grid_size * (grid_size * (r - 1) + (c2 - 1)) + val, lit2, 10);
							output += negation;
							output += lit2;
							output += conj_seperator;

							conjunction_count++;
						}
					}
				}
			}
			else {
				int val = cell_check->second;

				for (int c2 = c + 1; c2 <= grid_size; c2++) {
					//file_out << negation << prefix << num_seperator << r << num_seperator << c2 << num_seperator << val << conj_seperator;

					char lit[21];
					itoa(grid_size * (grid_size * (r - 1) + (c2 - 1)) + val, lit, 10);
					output += negation;
					output += lit;
					output += conj_seperator;

					conjunction_count++;
				}
			}
		}
	}


	// each column should only contain each value once
	for (int r = 1; r <= grid_size; r++) {
		for (int c = 1; c <= grid_size; c++) {
			map<pair<int, int>, int>::iterator cell_check = existing_values.find(pair<int, int>(r, c));
			if (cell_check == existing_values.end()) {
				map<int, vector<int>>::iterator check_row = existing_values_row.find(r);
				for (int val = 1; val <= grid_size; val++) {
					if (check_row == existing_values_row.end() || !binary_search(check_row->second.begin(), check_row->second.end(), val)) {
						for (int r2 = r + 1; r2 <= grid_size; r2++) {
							//file_out << negation << prefix << num_seperator << r << num_seperator << c << num_seperator << val << lit_seperator;

							char lit[21];
							itoa(grid_size * (grid_size * (r - 1) + (c - 1)) + val, lit, 10);
							output += negation;
							output += lit;
							output += lit_seperator;

							//file_out << negation << prefix << num_seperator << r2 << num_seperator << c << num_seperator << val << conj_seperator;

							char lit2[21];
							itoa(grid_size * (grid_size * (r2 - 1) + (c - 1)) + val, lit2, 10);
							output += negation;
							output += lit2;
							output += conj_seperator;

							conjunction_count++;
						}
					}
				}
			}
			else {
				int val = cell_check->second;

				for (int r2 = r + 1; r2 <= grid_size; r2++) {
					//file_out << negation << prefix << num_seperator << r2 << num_seperator << c << num_seperator << val << conj_seperator;

					char lit[21];
					itoa(grid_size * (grid_size * (r2 - 1) + (c - 1)) + val, lit, 10);
					output += negation;
					output += lit;
					output += conj_seperator;

					conjunction_count++;
				}
			}
		}
	}

	// each nxn grid
	for (int r = 1; r <= grid_size; r++) {
		for (int c = 1; c <= grid_size; c++) {
			map<pair<int, int>, int>::iterator cell_check = existing_values.find(pair<int, int>(r, c));
			if (cell_check == existing_values.end()) {
				map<pair<int, int>, vector<int>>::iterator check_box = existing_values_box.find(pair<int, int>((r - 1) / n + 1, (c - 1) / n + 1));
				for (int val = 1; val <= grid_size; val++) {
					if (check_box == existing_values_box.end() || !binary_search(check_box->second.begin(), check_box->second.end(), val)) {
						for (int r2 =  r - (r - 1) % n; r2 <  r - (r - 1) % n + n; r2++) {
							for (int c2 = c - (c - 1) % n; c2 < c - (c - 1) % n + n; c2++) {
								if (r != r2 && c != c2) {
									//file_out << negation << prefix << num_seperator << r << num_seperator << c << num_seperator << val << lit_seperator;

									char lit[21];
									itoa(grid_size * (grid_size * (r - 1) + (c - 1)) + val, lit, 10);
									output += negation;
									output += lit;
									output += lit_seperator;

									//file_out << negation << prefix << num_seperator << r2 << num_seperator << c2 << num_seperator << val << conj_seperator;

									char lit2[21];
									itoa(grid_size * (grid_size * (r2 - 1) + (c2 - 1)) + val, lit2, 10);
									output += negation;
									output += lit2;
									output += conj_seperator;

									conjunction_count++;
								}
							}
						}
					}
				}
			}
			else {
				int val = cell_check->second;

				for (int r2 = r - (r - 1) % n; r2 < r - (r - 1) % n + n; r2++) {
					for (int c2 = c - (c - 1) % n; c2 < c - (c - 1) % n + n; c2++) {
						if (r != r2 && c != c2) {
							//file_out << negation << prefix << num_seperator << r2 << num_seperator << c2 << num_seperator << val << conj_seperator;

							char lit[21];
							itoa(grid_size * (grid_size * (r2 - 1) + (c2 - 1)) + val, lit, 10);
							output += negation;
							output += lit;
							output += conj_seperator;

							conjunction_count++;
						}
					}
				}
			}
		}
	}

	output += conj_seperator;

	// open file
	fstream file_out;
	file_out.open(output_filename, fstream::out);

	// error check
	if (!file_out.is_open()) {
		printf("Cannot open output file.\n");
		return 1;
	}

	string problem_statment = "";
	problem_statment += zchaff_problem;
	problem_statment += " ";
	problem_statment += zchaff_type;
	problem_statment += " ";
	char total_var[21];
	itoa(grid_size * grid_size * grid_size, total_var, 10);
	problem_statment += total_var;
	problem_statment += " ";
	char total_conj[21];
	itoa(conjunction_count, total_conj, 10);
	problem_statment += total_conj;
	problem_statment += "\n";

	output = problem_statment + output;
	

	file_out << output;
	
	// close output file
	file_out.close();

	return 0;
}