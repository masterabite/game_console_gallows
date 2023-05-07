#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

//структура слова с подсказкой
struct word_hint {
	string word;	//слово
	string topic;	//тема
	string hint;	//подсказка
};

//структура игры
struct game {
	string word;				//текущее слово, которое видит игрок игрока
	vector <word_hint> words;	//базовый массив слов с подсказками
	string mistakes;			//кол-во ошибок игрока

	//конструктор по умолчанию для игры
	game() {
		words = vector<word_hint>();
		word = "";
		mistakes = "";
		loadFromFile("words_hints.txt");
	}

	//функция считывает из файла все базовые слова и подсказки
	void loadFromFile(string fname) {
		words.clear();						//очищаем базовый массив слов и подсказок
		ifstream fin(fname);				//пытаемся открыть файл с указанным названием
		word_hint temp;						//буферная переменная для заполнения базового массива слов с подсказками
		if (fin.is_open()) {				//если удалось открыть файл
			while (!fin.eof()) {			//пока не конец файла
				fin >> temp.word;			//считываем само слово
				fin >> temp.topic;			//считываем тему слова
				getline(fin, temp.hint);	//далее считываем до конца строки позсказку
				words.push_back(temp);		//добавляем данные в базу
			}
		}
	}

	//функция вывода виселицы в соответствтие с кол-во ошибок
	void printGallows(size_t m) {
								cout << "---------------\n";
								cout << "              |\n";
		if (m >= 1 && m <= 2)	cout << "              O\n"; else 
		if (m == 3)				cout << "             \\O\n"; else 
		if (m > 3)				cout << "             \\O/\n"; else cout << '\n';
		if (m >= 2)				cout << "              |\n"; else cout << '\n';
		if (m == 5)				cout << "             /\n"; else
		if (m == 6)				cout << "             / \\\n"; else cout << '\n';
								cout << '\n';

	}

	//функция вывода состояния игры для слова с индексом i из базы, с учетом уровня сложности
	void printState(int ind, int lvl) {
		cout << '\n';
		printGallows(mistakes.size());

		//выводим тему
		if (lvl < 3) {
			cout << "Тема раунда: " << words[ind].topic << '\n';
		}
		
		//выводим слово
		cout << "Слово: ";
		for (char c : word) { cout << c << " "; } //перебираем все символы и выводим
		cout << '\n'; //обозначаем конец строки

		//выводим ошибки
		cout << "Ошибки (" << mistakes.size() << "): ";
		for (size_t i = 0; i < mistakes.size(); ++i) {
			cout << mistakes[i] << ' ';
		}
		cout << '\n';

		//выводим напоминание о подсказке
		if (lvl < 2) {
			cout << "Чтобы использовать подсказку введите '?'\n";
		}
		cout << "Буква: ";
	}

	char toUpper(char c) {
		if (c <= -1 && c >= -32) c -= 32;
		return c;
	}

	//главная функция игрового процесса
	void play() {
		srand(time(NULL));											//устанавливаем случайное значение для генерации
		int ind = rand() % int(words.size());						//случайно выбираем слово из базы
		
		mistakes = "";												//очищаем все ошибки
		word = "";													//очищаем текущее слово пользователя
		for (size_t i = 0; i < words[ind].word.size(); ++i) {		//в цикле заполняем вместо всех букв знаки _
			word += '_';
		}
		

		int lvl;													//уровень сложности
		cout << "(1:легко 2:средне 3:сложно)\n";
		cout << "Введите уровень сложности: ";
		cin >> lvl;													//вводится уровень

		string input;												//строка, которую вводит пользователь
		
		while (true) {
			printState(ind, lvl);									//выводим текущую информацию об игре
			cin >> input;											//пользователь вводит букву
			if (lvl == 1 && input[0] == '?') {						//если пользователь запрашивает подсказку
				cout << "Подсказка:" << words[ind].hint << '\n';	//выводим подсказку
			} 

			//проверка на корректный ввод
			bool check = true;
			if (input[0] > -1 || input[0] < -64) check = false;		//если ввели не букву, ввод не корректный
			for (size_t i = 0; i < word.size(); ++i) {
				if (toUpper(input[0]) == word[i]) check = false;	//если ввели уже открытую букву
			}
			for (size_t i = 0; i < mistakes.size(); ++i) {
				if (toUpper(input[0]) == mistakes[i]) check= false;	//если ввели букву, которая уже числится в ошибках
			}
			if (!check) continue;
			/////////////////////////////

			
			bool miss = true;								
			for (size_t i = 0; i < words[ind].word.size(); ++i) {	//перебираем все буквы соответствующего слова из базы
				if (toUpper(input[0]) == words[ind].word[i]) {		//если нашли совпадение буквы пользователя с i'ой буквой слова
					word[i] = words[ind].word[i];					//открываем эти буквы в слове пользователя
					miss = false;									//если нашли хотя бы одно совпадение, значит ошибки не было
				}
			}

			if (miss) {												//если соответствующих букв в слове нет, добавляем к ошибкам
				mistakes += toUpper(input[0]);
			}

			//если игрок совершает 6 ошибок, то он проиграл
			if (mistakes.size() == 6) {
				cout << "Вы проиграли!\n";
				cout << "Загаданное слово: " << words[ind].word;
				break;
			}

			//игрок победил, если в слове win не осталось пустых букв _
			if (word.find('_') == string::npos) {
				cout << "Вы выйграли!\n";
				cout << "Загаданное слово: " << words[ind].word;
				break;
			}
		}
		
	}
};

int main() {
	//устанавливаем кодировку для кириллицы
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
	//создаем игру и запускаем ее
	game g;
	g.play();

	return 0;
}
