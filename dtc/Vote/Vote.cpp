#include "Vote.h"
#include <fstream>
int main()
{
	ifstream file("C:\\Users\\User\\Documents\\MVS\\курсовая\\datasets\\vote.txt");
	int comma_count = 0;
	vector <line> data;
	vector <line> test;
	if (!file.is_open()) printf("Could not open the file\n");
	else {
		string line;
		bool done = false;
		while (getline(file, line)) {
			if (line != "") {
				if (!done) {
					comma_count = count_att(line);
					done = true;
				}
				read_data(data, line, comma_count);
			}
		}
		cout << "DATA COUNT: " << data.size() << endl;
		node* root = new node(data, find_types(data));
		root->split();
		cout << "ROOT:" << endl;
		root->node_print();
		file.close();
		ifstream file("C:\\Users\\User\\Documents\\MVS\\курсовая\\datasets\\vote_test.txt");
		int correct = 0;
		int incorrect = 0;
		if (!file.is_open()) printf("Could not open the file\n");
		else {
			string line;
			bool done = false;
			while (getline(file, line)) {
				if (line != "") {
					if (!done) {
						comma_count = count_att(line);
						done = true;
					}
					read_data(test, line, comma_count);
				}
			}
			for (int i = 0; i < test.size(); ++i) {
				if (classify(test[i], root) == true) correct++;
				else incorrect++;
			}
			cout << "Accuracy:" << (double)correct / (double)test.size();
		}
	}
}

