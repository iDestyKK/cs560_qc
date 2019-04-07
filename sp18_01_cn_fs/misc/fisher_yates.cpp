#include <bits/stdc++.h>

using namespace std;

int main() {
	vector<int> ya(10);
	for (int i = 0; i < ya.size(); i++)
		ya[i] = i;

	//Sort
	for (int i = ya.size() - 1; i > 0; i--) {
		int j = rand() % (i + 1);

		swap(ya[i], ya[j]);
	}

	for (int i = 0; i < ya.size(); i++)
		printf("%d ", ya[i]);
	printf("\n");
}
