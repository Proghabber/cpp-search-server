// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:

// Закомитьте изменения и отправьте их в свой репозиторий.
// Закомитьте изменения и отправьте их в свой репозиторий.
#include <iostream>

using namespace std;
int main() {

	int answer = 0;
	for (int i = 0; i <= 100; i++) if (i == 3 || i / 10 == 3 || i % 10 == 3) {
		cout << i << endl;
		answer++;

	};
	cout << "From 0 to 100 number 3 have " << answer << " numbers";
	return 0;

}
