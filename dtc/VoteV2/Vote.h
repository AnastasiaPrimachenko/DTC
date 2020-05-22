#include <iostream>
#include <string>
#include <vector>
#include <math.h>
using namespace std;

struct line {
	vector <string> attributes;
	string type;
	line(string& t, vector<string>& att) {
		this->type = t;
		this->attributes = att;
	}
	void print() {
		int i = 0;
		while (i < attributes.size()) {
			cout << attributes[i] << ", ";
			++i;
		}
		cout << type << endl;
	}
};

int count_att(string str) {
	int i = 0;
	int count = 0;
	if (str.size() == 0)
		throw - 1;
	while (i < str.size()) {
		if (str[i] == ',') { ++count; }
		++i;
	}
	return count;
}

bool unique(vector<string> temp, string a) {
	for (int i = 0; i < temp.size(); ++i) {
		if (a == temp[i]) {
			return false;
			break;
		}
	}
}

vector<string> count_att_types(vector<line> data, int j) {
	int i = 0;
	vector <string> temp;
	while (i < data.size()) {
		if (unique(temp, data[i].attributes[j])) temp.push_back(data[i].attributes[j]);
		++i;
	}
	return temp;
}

void read_data(vector<line>& data, string& str, int& count_comma) {
	string temp_s = "";
	vector<string> temp_a;
	int r = 0;
	while (str[r] != ',') {
		if (str[r] == 'r' && temp_s == "") temp_s = "republican";
		else if (str[r] == 'd' && temp_s == "") temp_s = "democrat";
		++r;
	}
	int i = r + 1;
	int j = 1;
	while (i < str.size()) {
		string temp;
		while (str[i] != ',' && i != str.size()) {
			temp.push_back(str[i]);
			++i;
		}
		if (i == str.size() || str[i] == ',') {
			if (temp == "?") temp_a.push_back("n");
			else temp_a.push_back(temp);
			++i;
		}
		++j;
	}
	data.push_back(line(temp_s, temp_a));
}

vector<string> find_types(vector<line> data) {
	if (data.size() == 0) {
		return vector<string>();
	}
	vector<string> unique;
	unique.push_back(data[0].type);
	int i = 0;
	bool type2 = false;
	while (i < data.size()) {
		if (data[0].type != data[i].type) {
			unique.push_back(data[i].type);
			break;
		}
		++i;
	}
	return unique;
}

struct node {
	vector <line> data;
	vector <string> unique_types;
	string decision;
	string a_label;
	string label;
	double compare;
	int attribute;
	node* parent;
	vector <node*> children;
	int level;
	node(vector<line> d, vector<string> u_t) {
		this->data = d;
		this->unique_types = u_t;
		this->decision = "Root";
		this->a_label = "";
		this->level = 0;
		this->parent = nullptr;
	}
	~node() {
	}

	vector<int> instances_of_each_type(vector<line> d) {
		int type1 = 0, type2 = 0, i = 0;
		while (i < d.size()) {
			if (d[i].type == this->unique_types[0]) type1++;
			else if (d[i].type == this->unique_types[1]) type2++;
			++i;
		}
		return vector<int>{type1, type2};
	}

	double entropy(vector<line> data) {
		double e = 0, p = 0;
		vector<int> type_count = instances_of_each_type(data);
		for (int i = 0; i < type_count.size(); ++i) {
			if (type_count[i] != 0) {
				p = (double)type_count[i] / (double)data.size();
				e += -p * log2(p);
			}
		}
		return e;
	}

	double find_split_point(vector <line> data) {
		double gain = 0, cutoff = 0, attribute_cutoff = 0;
		for (int i = 0; i < data[0].attributes.size(); ++i) {
			vector<string> temp_cutoff = count_att_types(data, i);
			vector< vector<line>>child(temp_cutoff.size());
			for (int k = 0; k < data.size(); ++k) {
				for (int j = 0; j < temp_cutoff.size(); ++j) {
					if (data[k].attributes[i] == temp_cutoff[j]) child[j].push_back(data[k]);
				}
			}
			double e1 = entropy(data);
			vector<double>e(temp_cutoff.size());
			for (int k = 0; k < e.size(); ++k) {
				e[k] = entropy(child[k]);
			}
			double temp_gain = 0;
			for (int k = 0; k < e.size(); ++k) {
				temp_gain = temp_gain + (double)child[k].size() / (double)data.size() * e[k];
			}
			temp_gain = e1 - temp_gain;
			if (temp_gain > gain) {
				attribute_cutoff = i;
				gain = temp_gain;
			}
		}
		return attribute_cutoff;
	}
	void unite_leaves(node* leaf) {
		if (leaf->parent->children[0]->label == leaf->parent->children[1]->label) {
			leaf->parent->label = leaf->parent->children[0]->label;
			leaf->parent->attribute = -1;
			leaf->parent->children.clear();
			for (int i = 0; i < leaf->parent->data.size(); ++i) {
				leaf->parent->data[i].type = leaf->label;
			}
			unite_leaves(leaf->parent);
		}
	}

	void split() {
		if (data.size() > 10) {
			vector<int> type_count = instances_of_each_type(this->data);
			int f = 0;
			for (int i = 0; i < type_count.size(); ++i) if (type_count[i] != 0) f++;
			if (f <= 1) {
				attribute = -1;
				label = data[0].type;
				return;
			}
			int split_point = find_split_point(this->data);
			vector <string> cutoff = count_att_types(data, split_point);
			vector< vector<line>>child(cutoff.size());
			for (int i = 0; i < data.size(); ++i) {
				for (int j = 0; j < cutoff.size(); ++j) {
					if (data[i].attributes[split_point] == cutoff[j]) child[j].push_back(data[i]);
				}
			}
			string s_temp = ("Attribute-" + to_string(split_point + 1));
			this->decision = s_temp;
			attribute = split_point;
			for (int i = 0; i < child.size(); ++i) {
				children.resize(child.size());
				children[i] = new node(child[i], this->unique_types);
				children[i]->a_label = cutoff[i];
				children[i]->parent = this;
				children[i]->level = this->level + 1;
			}
			for (int i = 0; i < children.size(); ++i) children[i]->split();
		}
		else {
			vector<int> type_count = instances_of_each_type(this->data);
			if (type_count[0] >= type_count[1]) {
				attribute = -1;
				label = unique_types[0];
				for (int i = 0; i < data.size(); ++i) {
					data[i].type = label;
				}
			}
			else {
				attribute = -1;
				label = unique_types[1];
				for (int i = 0; i < data.size(); ++i) {
					data[i].type = label;
				}
			}
			unite_leaves(this);
		}
	}
	void node_print() {
		string space = "";
		for (int i = 0; i < this->level; ++i) {
			space += "  ";
		}
		if (children.size() == 0) {
			vector <int> type_count = instances_of_each_type(data);
			cout << space << this->a_label << endl;
			if (type_count[0] != 0) {
				cout << space << "Leaf: " << unique_types[0] << "(" << type_count[0] << ")" << endl;
			}
			if (type_count[1] != 0) {
				cout << space << "Leaf: " << unique_types[1] << "(" << type_count[1] << ")" << endl;
			}
			return;
		}
		cout << space << this->a_label << endl;
		cout << space << decision << endl;
		for (int i = 0; i < children.size(); ++i) {
			children[i]->node_print();
		}
	}
};

bool classify(line data, node* att) {
	bool correct = false;
	if (att->attribute != -1) {
		for (int i = 0; i < att->children.size(); ++i) {
			if (data.attributes[att->attribute] == att->children[i]->a_label) correct = classify(data, att->children[i]);
		}
	}
	else {
		if (data.type == att->label) return true;
		else return false;
	}
	return correct;
}
