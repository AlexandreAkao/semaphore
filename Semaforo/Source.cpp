#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "Semaphore.h"

using namespace std;

int produtos[20];
int produtosSize = 20;
int quantidadeProdutos = 0;
Semaphore* semaforoProducer;
Semaphore* semaforoConsumer;

struct thread_info {
	int begin;
	int end;
	bool endend;
};

vector<thread_info> producerList;
vector<thread_info> consumerList;

//void print(string acao) {
//
//	cout << acao << ": [";
//
//	for (int i = 0; i < tamanho; i++) {
//		if (produtos[i] == 0) {
//			cout << "_";
//
//		}
//		else {
//			cout << produtos[i];
//		}
//
//
//		if (i != tamanho - 1) {
//			cout << ", ";
//		}
//	}
//
//	cout << "]" << endl;
//}

void print() {

	while (true) {
		cout << "[";

		for (int i = 0; i < produtosSize; i++) {
			if (produtos[i] == 0) {
				cout << "_";

			}
			else {
				cout << produtos[i];
			}


			if (i != produtosSize - 1) {
				cout << ", ";
			}
		}

		cout << "]" << endl;

		this_thread::sleep_for(chrono::seconds(1));
	}
}

bool verifyThreadsInfo(bool key_consumer) {

	vector<thread_info> vec = (key_consumer) ? consumerList : producerList;
	bool result = true;

	for (int i = 0; i < vec.size(); i++) {
		result = result && vec[i].endend;

		if (!result)
			break;
	}

	return result;
}

void resetThreads(bool key_consumer) {
	if (key_consumer) {
		for (int i = 0; i < consumerList.size(); i++) {
			consumerList[i].endend = false;

		}
	} else {
		for (int i = 0; i < producerList.size(); i++) {
			producerList[i].endend = false;
		}
	}


}

void produtor(int position) {
	//semaforo->down(position);

	bool aux = false;

	int beginPosition = producerList[position].begin;
	int endPosition = producerList[position].end;

	int currentPosition = beginPosition;

	while (true) {
		if (aux) {
			//semaforoProducer->down();

			while (producerList[position].endend) {
				if (verifyThreadsInfo(false)) {
					semaforoProducer->up();
				}

				semaforoProducer->down();
			}

			currentPosition = beginPosition;
			aux = false;
		}

		if (currentPosition < endPosition) {
			//cout << "PRODUZINDO********************" << endl;
			//produtos[currentPosition] = rand() % 9 + 1;
			produtos[currentPosition] = position + 1;

			/*thread::id this_id = this_thread::get_id();
			cout << "THREAD " << this_id << " PRODUZINDO: " << produtos[currentPosition] << " posicao: " << currentPosition << endl;*/

			currentPosition++;
			quantidadeProdutos++;

		} else {
			aux = true;

			producerList[position].endend = true;

			if (verifyThreadsInfo(false)) {

				resetThreads(true);

				for (int i = 0; i < consumerList.size(); i++) {
					semaforoConsumer->up();
				}
			}
		}

		this_thread::sleep_for(chrono::seconds(1));
	}
}

void consumidor(int position) {
	//semaforo->down(position);

	bool aux = true;
	int beginPosition = consumerList[position].begin;
	int endPosition = consumerList[position].end;

	int currentPosition = endPosition;
	
	while (true) {
		if (aux) {
			//semaforoConsumer->down();

			while (consumerList[position].endend) {
				if (verifyThreadsInfo(true)) {
					semaforoConsumer->up();
				}
				semaforoConsumer->down();
			}

			currentPosition = endPosition;
			aux = false;
		}

		if (currentPosition > beginPosition) {
			//cout << "-----------------------CONSUMINDO" << endl;
			/*thread::id this_id = this_thread::get_id();
			cout << "THREAD " << this_id << " CONSUMINDO: " << produtos[currentPosition - 1] << " posicao: " << currentPosition << endl;*/

			produtos[currentPosition - 1] = 0;
			quantidadeProdutos--;
			currentPosition--;

		} else {
			aux = true;

			consumerList[position].endend = true;

			if (verifyThreadsInfo(true)) {
				resetThreads(false);
				for (int i = 0; i < producerList.size(); i++) {
					semaforoProducer->up();
				}
			}
		}

		this_thread::sleep_for(chrono::seconds(1));
	}
}

void fillVector(int theadsCount, bool isConsumer) {
	int value = ceil((float) produtosSize / theadsCount);

	for (int i = 0; i < theadsCount; i++) {
		thread_info newInfo;

		if (i == 0)
			newInfo.begin = i;
		else
			if (isConsumer) 
				newInfo.begin = consumerList[i - 1].begin + value;
			else
				newInfo.begin = producerList[i - 1].begin + value;

		if (newInfo.begin > produtosSize)
			newInfo.endend = true;
		else
			newInfo.endend = isConsumer;

		if (newInfo.begin + value > produtosSize)
			newInfo.end = produtosSize;
		else
			newInfo.end = newInfo.begin + value;

		if (isConsumer)
			consumerList.push_back(newInfo);
		else
			producerList.push_back(newInfo);
	}
}

int main() {
	srand(time(NULL));

	int producerCount = 3;
	int consumerCount = 3;

	semaforoProducer = new Semaphore(producerCount, 0);
	semaforoConsumer = new Semaphore(consumerCount, 0);

	fillVector(producerCount, false);
	fillVector(consumerCount, true);

	vector<thread> threads;

	for (int i = 0; i < producerCount; i++) {
		threads.push_back(thread(produtor, i));
	}

	for (int i = 0; i < consumerCount; i++) {
		threads.push_back(thread(consumidor, i));
	}

	thread printProdutos(print);
	printProdutos.join();

	for (int i = 0; i < producerCount + consumerCount; i++) {
		threads[i].join();
	}

	return 0;
}