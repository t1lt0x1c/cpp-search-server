// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:
#include <iostream>
#include <string>

using namespace std;

int main() {
	int n = 0; // счётчик
	for (int i = 1; i < 1001; i++) {
		string num = to_string(i);
		for (const char& s : num) {
			if (s == '3') {
				n++;
				break;
			}
		}
	}
	cout << n << endl;
}
// Закомитьте изменения и отправьте их в свой репозиторий.
