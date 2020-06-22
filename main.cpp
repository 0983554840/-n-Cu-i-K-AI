#include<iostream>
#include<conio.h>
#include<Windows.h>
#include "Support.h"
#include <fstream>
#include <chrono>
#include<string.h>
#include <string>

#define CLOCKS_PER_SPMLSEC 1000000
#define n 3
int spPos[9] = { 100000000,10000000,1000000,100000,10000,1000,100,10,1 };

int micros()
{
	int ms = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
		now().time_since_epoch()).count();
	return ms;
}
void translateTime(int input)
{
	std::cout << " = " << input / 1000000 << ".";
	for (int i = 6; i > 0; i--)
		std::cout << (input % spPos[8 - i]) / spPos[8 - i + 1];
	std::cout << "s";
}

struct Node
{
	int key, mainPos;
	int cmd;
	int depth;
	int h, cost;
	Node* next;
	Node* undo;
	Node* child;
	Node* parent;
};

struct NodeList
{
	int length = 0;
	Node* head;
	Node* tail;
};

// I.Graphics
void paintCell(int x, int y, int number)
{
	gotoxy(x, y); printf("%c%c%c%c%c%c", 201, 205, 205, 205, 205, 187);
	gotoxy(x, y + 1); printf("%c %2d %c", 186, number, 186);
	gotoxy(x, y + 2); printf("%c%c%c%c%c%c", 200, 205, 205, 205, 205, 188);
}
void earseCell(int x, int y)
{
	gotoxy(x, y); printf("       ");
	gotoxy(x, y + 1); printf("       ");
	gotoxy(x, y + 2); printf("       ");
}
void showTable(int key, int x, int y)
{
	int table[9];
	for (int i = 0; i < 9; i++)
		table[i] = key / spPos[i] % 10;
	int run = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (table[run] != 0)
				paintCell((y + j) * 7, (x + i) * 3, table[run]);
			else earseCell((y + j) * 7, (x + i) * 3);
			run++;
		}
	}
}
void showAllStep(Node* input, int x, int y, int time)
{
	system("cls");
	int i = 0;
	int j = 1;
	int k = 7;
	input->cmd = -1;
	showTable(input->key, x, y);
	int count = 0;
	while (input != NULL)
	{
		Sleep(time);
		showTable(input->key, x, y);
		gotoxy(y * 7 + i, (x + 3) * 3 + count + j);
		std::cout << "Step " << count << ":";
		input->depth = count++;
		std::cout << ((input->cmd == -1) ? "NONE" : (input->cmd == 0) ? "UP" : (input->cmd == 1) ? "DOWN" : (input->cmd == 2) ? "LEFT" : "RIGHT");
		if (input->depth % k == 0 && input->depth > 0)
		{
			i += 15;
			j -= k;
		}
		input = input->child;
	}
	gotoxy(0, (x + 3) * 3 + k + 3);
	std::cout << "Pree any key to out..";
	_getch();
}

// II.Support for Program
bool isLegal(int mainPos, int cmd)
{
	if (cmd == -1)
		return false;
	if (mainPos < 3 && cmd == 0)
		return false;
	else if (mainPos > 5 && cmd == 1)
		return false;
	else if (mainPos % 3 == 0 && cmd == 2)
		return false;
	else if (mainPos % 3 == 2 && cmd == 3)
		return false;
	return true;
}
int executeCmd(int& key, int& mainPos, int cmd)
{
	int newMain = mainPos + ((cmd == 0) ? -3 : (cmd == 1) ? 3 : (cmd == 2) ? -1 : 1);
	int exc = key / spPos[newMain] % 10;
	key = key + spPos[mainPos] * exc - spPos[newMain] * exc;
	mainPos = newMain;
	return exc;
}
bool isOpposite(int cmd1, int cmd2)
{
	if (cmd1 == -1 || cmd2 == -1 || cmd1 == cmd2)
		return false;
	if ((cmd1 < 2 && cmd2 < 2) || (cmd1 > 1 && cmd2 > 1))
		return true;
	return false;
}
int Heuristic(int start, int goal)
{
	int startPos[9] = { 0 };
	int goalPos[9] = { 0 };
	for (int i = 8; i >= 0; i--)
	{
		startPos[start % 10] = goalPos[goal % 10] = i;
		start /= 10;
		goal /= 10;
	}
	int total = 0;
	for (int i = 1; i < 9; i++)
	{
		int temp = (startPos[i] > goalPos[i]) ? (startPos[i] - goalPos[i]) : (-startPos[i] + goalPos[i]);
		total += i * (temp / 3 + temp % 3);
	}
	return total;
}
void save(int key1, int key2)
{
	std::fstream f;
	f.open("save.txt", std::ios::out);
	for (int i = 0; i < 9; i++)
	{
		f << key1 / spPos[i];
		key1 %= spPos[i];
	}
	f << "\n";
	for (int i = 0; i < 9; i++)
	{
		f << key2 / spPos[i];
		key2 %= spPos[i];
	}
	f.close();
}
void takeData(int& key1, int& key2)
{
	key1 = key2 = 0;
	std::fstream f;
	f.open("save.txt", std::ios::in);
	char temp[10];
	f >> temp;
	for (int i = 0; i < 9; i++)
		key1 = key1 * 10 + (temp[i] - 48);
	f >> temp;
	for (int i = 0; i < 9; i++)
		key2 = key2 * 10 + (temp[i] - 48);

	f.close();
}

// III.Support Node
Node* newNode()
{
	Node* temp = new Node;
	temp->key = 0;
	temp->cost = temp->depth = temp->h = 0;
	temp->cmd = temp->mainPos = -1;
	temp->next = temp->undo = temp->child = temp->parent = NULL;
	return temp;
}
Node* setNode(int key)
{
	Node* temp = newNode();
	temp->key = key;

	temp->mainPos = 8;
	while (key % 10 != 0)
	{
		key /= 10;
		temp->mainPos--;
	}

	return temp;
}
Node* cloneNode(Node* input)
{
	Node* temp = newNode();
	temp->key = input->key;
	temp->mainPos = input->mainPos;
	return temp;
}
Node* childNode(Node* input, int cmd)
{
	Node* temp = newNode();
	temp->cmd = cmd;
	temp->mainPos = input->mainPos;
	temp->key = input->key;

	temp->cost = executeCmd(temp->key, temp->mainPos, temp->cmd) + input->cost - input->h;

	temp->parent = input;
	temp->depth = input->depth + 1;
	return temp;
}
void genocideNode(Node* input, bool All)
{
	Node* run = input->child;
	while (run->child != NULL)
	{
		Node* temp = run;
		run = run->child;
		delete temp;
	}
	if (All)
	{
		delete input;
		delete run;
	}
}
// IV.Support NodeList
NodeList* newNodeList()
{
	NodeList* temp = new NodeList;
	temp->head = temp->tail = NULL;
	temp->length = 0;
	return temp;

}
void checkList(NodeList* List)
{
	int count = 0;
	Node* run = List->head;
	while (run != NULL)
	{
		count++;
		run = run->next;
	}
	if (List->length - count > 0)
		printf("\n%d", List->length - count);
}
//  - add Node to tail NodeList
void addNodeList(Node* input, NodeList* List)
{
	List->length++;
	if (List->head == NULL)
	{
		List->head = input;
	}
	else if (List->tail == NULL)
	{
		List->tail = input;
		List->tail->undo = List->head;
		List->head->next = List->tail;
	}
	else
	{
		List->tail->next = input;
		input->undo = List->tail;
		List->tail = input;
	}
}
//  - insert Node (min)
void insertNodeList(Node* input, NodeList* List)
{
	List->length++;
	if (List->head == NULL)
	{
		List->head = input;
	}
	else if (List->tail == NULL)
	{
		if (input->cost < List->head->cost)
		{
			List->tail = List->head;
			List->head = input;
		}
		else List->tail = input;
		List->tail->undo = List->head;
		List->head->next = List->tail;
	}
	else
	{
		if (input->cost >= List->tail->cost)
		{
			List->tail->next = input;
			input->undo = List->tail;
			List->tail = input;
		}
		else if (input->cost <= List->head->cost)
		{
			List->head->undo = input;
			input->next = List->head;
			List->head = input;
		}
		else if (input->cost > (List->tail->cost + List->head->cost) / 2)
		{
			Node* run = List->tail;
			while (input->cost <= run->cost)
			{
				run = run->undo;
			}
			input->undo = run;
			input->next = run->next;
			run->next->undo = input;
			run->next = input;
		}
		else
		{
			Node* run = List->head;
			while (input->cost >= run->cost)
			{
				run = run->next;
			}
			input->next = run;
			input->undo = run->undo;
			run->undo->next = input;
			run->undo = input;
		}
	}
}
Node* popNodeList(NodeList* List)
{
	if (List->head == NULL)
		return NULL;
	List->length--;
	Node* temp = List->head;
	List->head = List->head->next;
	if (List->head != NULL)
		List->head->undo = NULL;
	temp->next = NULL;
	return temp;
}
bool checkExist(Node* input, NodeList* List)
{
	Node* run = List->head;
	while (run != NULL)
	{
		if (run->key == input->key)
			return true;
		run = run->next;
	}
	return false;
}
void deleteNodeList(NodeList* List)
{
	while (List->head != NULL)
	{
		delete popNodeList(List);
	}
}
//  - add to Tree
void addNodeTree(Node* input, Node*& Tree)
{
	if (Tree == NULL)
	{
		Tree = input;
		return;
	}
	Node* run = Tree;
	while (1)
	{
		if (run->key <= input->key)
		{
			if (run->next != NULL)
				run = run->next;
			else
			{
				run->next = input;
				return;
			}
		}
		else
		{
			if (run->undo != NULL)
				run = run->undo;
			else
			{
				run->undo = input;
				return;
			}
		}
	}
}
//  - add to Tree with cost
void insertNodeTree(Node* input, Node*& Tree)
{
	if (Tree == NULL)
	{
		Tree = input;
		return;
	}
	Node* run = Tree;
	while (1)
	{
		if (run->cost <= input->cost)
		{
			if (run->next != NULL)
				run = run->next;
			else
			{
				run->next = input;
				return;
			}
		}
		else
		{
			if (run->undo != NULL)
				run = run->undo;
			else
			{
				run->undo = input;
				return;
			}
		}
	}
}

bool checkExistOnTree(Node* input, Node* Tree)
{
	Node* run = Tree;
	while (run != NULL)
	{
		if (run->key < input->key)
		{
			run = run->next;
		}
		else if (run->key > input->key)
		{
			run = run->undo;
		}
		else return true;
	}
	return false;
}
Node* popMinOnTree(Node*& Tree)
{
	Node* temp;
	if (Tree->undo == NULL)
	{
		temp = Tree;
		Tree = Tree->next;
	}
	else
	{
		Node* run = Tree;
		while (run->undo->undo != NULL)
			run = run->undo;
		temp = run->undo;
		run->undo = run->undo->next;
	}
	temp->next = NULL;
	return temp;
}
void deleteTree(Node* Tree)
{
	if (Tree == NULL)
		return;
	deleteTree(Tree->undo);
	deleteTree(Tree->next);
	delete Tree;
}
void showTree(Node* Tree)
{
	if (Tree == NULL)
		return;
	showTree(Tree->undo);
	printf("%d ", Tree->cost);
	showTree(Tree->next);
}

// V.Algorithm
void connect(Node*& start, Node* goal)
{
	Node* run = goal->parent;
	while (run != NULL)
	{
		goal->parent = cloneNode(run);
		goal->parent->depth = run->depth;
		goal->parent->cmd = run->cmd;
		goal->parent->child = goal;
		goal = goal->parent;
		run = run->parent;
	}
	start = goal;
}
bool BFS(Node*& startInput, Node*& goalInput)
{
	Node* start = cloneNode(startInput);
	Node* goal = cloneNode(goalInput);
	if (start->key == goal->key)
	{
		start->depth = 0;
		return true;
	}

	NodeList* rank = newNodeList();
	Node* used = cloneNode(start);

	int timeS = micros();

	bool canFind = false;
	addNodeList(start, rank);
	Node* run;
	do
	{
		run = popNodeList(rank);
		for (int i = 0; i < 4; i++)
		{
			if (isLegal(run->mainPos, i) && isOpposite(run->cmd, i) == false)
			{
				Node* child = childNode(run, i);
				if (checkExistOnTree(child, used) == false)
				{
					if (child->key == goal->key)
					{
						delete goal; goal = child;
						canFind = true;
						goto last;
					}
					addNodeList(child, rank);
				}
				else delete child;
			}
		}
		addNodeTree(run, used);
	} while (run->depth < 30);

last:
	int time = (micros() - timeS);
	if (canFind)
	{
		connect(start, goal);
		std::cout << "\nBFS  :" << goal->depth << " in " << time << "us";
		translateTime(time);

		printf("\nPress [V] to view...");
		int c = _getch();
		if (c == 'V' || c == 'v')
			showAllStep(start, 0, 0, 50);
		startInput = start;
		goalInput = goal;
	}
	deleteNodeList(rank);
	deleteTree(used);
	return canFind;
}
bool AStar(Node*& start, Node*& goal)
{
	if (start->key == goal->key)
	{
		start->depth = 0;
		return true;
	}

	NodeList* rank = newNodeList();
	Node* used = cloneNode(start);

	int time = micros();

	bool canFind = false;
	addNodeList(start, rank);
	Node* run;
	do
	{
		run = popNodeList(rank);
		for (int i = 0; i < 4; i++)
		{
			if (isLegal(run->mainPos, i) && isOpposite(run->cmd, i) == false)
			{
				Node* child = childNode(run, i);
				if (checkExistOnTree(child, used) == false)
				{
					if (child->key == goal->key)
					{
						delete goal; goal = child;
						canFind = true;
						goto last;
					}
					child->h = Heuristic(child->key, goal->key);
					child->cost += child->h;
					insertNodeList(child, rank);
				}
				else delete child;
			}
		}
		addNodeTree(run, used);
	} while ((micros() - time) < 3000000);

last:
	time = (micros() - time);
	if (canFind)
	{
		std::cout << "\nAstar  :" << goal->depth << " in " << time << "us";
		translateTime(time);
		if (BFS(start, goal) == false)
		{
			connect(start, goal);
			printf("\nPress [V] to view...");
			int c = _getch();
			if (c == 'V' || c == 'v')
				showAllStep(start, 0, 0, 50);
		}
	}
	deleteNodeList(rank);
	deleteTree(used);
	return canFind;
}

// VI. GamePlay 
bool PvE = true;
int Type = 15;
int stateS;
int stateG;

void createFromMove()
{
	int key = 0;
	bool test[9] = { false };
	for (int i = 0; i < 9; i++)
	{
		int temp;
		do
		{
			temp = rand() % 9;
		} while (test[temp] == true);
		test[temp] = true;
		key = key * 10 + temp;
	}
	std::cout << "Goal: " << key;
	stateG = key;
	Node* Tree = setNode(stateG);
	Node* start = setNode(stateG);
	int run = Type;
	float timeSleep = 1000 / Type;
	while (run >= 0)
	{
		showTable(start->key, 1, 0);
		printf("\n%4d", run--);
		int bestChoice = -1, maxH = -9999;
		bool test[4] = { false }; int beUsed = 4;
		do
		{
			int i;
			do
				i = rand() % 4;
			while (test[i] == true);
			beUsed--;
			if (isLegal(start->mainPos, i))
			{
				Node* temp = childNode(start, i);
				if (checkExistOnTree(temp, Tree) == false)
				{
					int h = Heuristic(start->key, key);
					if (h > maxH)
					{
						h = maxH;
						bestChoice = i;
					}
				}
				delete temp;
			}
		} while (beUsed > 0);
		if (bestChoice == -1)
		{
			do
				bestChoice = rand() % 4;
			while (isLegal(start->mainPos, bestChoice) == false);
		}
		start = childNode(start, bestChoice);
		addNodeTree(start, Tree);
		Sleep(timeSleep);
	}
	showTable(start->key, 1, 0);
	std::cout << "\nStart: " << start->key << "\n";
	stateS = start->key;
	deleteTree(Tree);
}
void inputTable(int input)
{
	int state[9];
	while (1)
	{
		bool test[9] = { false };
		for (int i = 0; i < 9; i++)
		{
			std::cin >> state[i];
			if (state[i] < 0 || state[i]>8 || test[state[i]] == true)
			{
				std::cout << "\nWrong, input again:";
				break;
			}
			if (i == 8)
			{
				int key = 0;
				for (int i = 0; i < 9; i++)
					key += state[i] * spPos[i];
				if (input == 0) stateS = key;
				else stateG = key;
				return;
			}
		}
	}
}
int createFromTable(int key)
{
	Node* temp = setNode(key);
	showTable(temp->key, 1, 0);
	while (1)
	{
		int cmd = -1;
		switch (_getch())
		{
		case'w':
		case'W':cmd = 0;
			break;
		case's':
		case'S':cmd = 1;
			break;
		case'A':
		case'a':cmd = 2;
			break;
		case'D':
		case'd':cmd = 3;
			break;
		case 13: key = temp->key;
			delete temp;
			return key;
		}
		if (isLegal(temp->mainPos, cmd))
		{
			executeCmd(temp->key, temp->mainPos, cmd);
			showTable(temp->key, 1, 0);
		}
	}
}
void customGame();
bool setUpGame();
void optionGame();

int computerMove(int x, int y, Node*& start, Node*& goal);
int playerMove(int x, int y, Node* start, Node* goal);
void playGame(Node* start, Node* goal, int gotoP);

void main()
{
	srand((int)time(NULL));
	hidecursor();
	fontSize(22);
	while (1)
	{
		bool New = setUpGame();
		if (New == true)
		{
			system("cls");
			if (Type != 0)
			{
				createFromMove();
			}
			else Type = 15;
			Node* start = setNode(stateS);
			Node* goal = setNode(stateG);

			AStar(start, goal);

			playGame(start, goal, 0);
		}
	}
	_getch();
}

bool setUpGame()
{
	system("cls");
	int x = takeConsoleColumns(), y = takeConsoleRows();
	gotoxy(x / 2 - 7, y * 1 / 4);
	printf("GAME: 8_PUZZLE");
	gotoxy(x / 2 - 8, y * 1 / 4 + 1);
	printf("-MADE BY TEAM 1-");
	gotoxy(x / 2 - 8, y * 1 / 3 + 1);
	printf("[1.NEW GAME     ]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 2);
	printf("[2.CONTINUE GAME]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 3);
	printf("[3.OPTION       ]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 4);
	printf("[4.EXIT(ESC)    ]");

	int i = 1;
	while (1)
	{
		gotoxy(x / 2 - 9, y * 1 / 3 + i); std::cout << ">";
		gotoxy(x / 2 + 9, y * 1 / 3 + i); std::cout << "<";
		while (1)
		{
			bool okay = false;
			if (_kbhit())
			{
				switch (_getch())
				{
				case'w':
				case'W':
					gotoxy(x / 2 - 9, y * 1 / 3 + i); std::cout << " ";
					gotoxy(x / 2 + 9, y * 1 / 3 + i); std::cout << " ";
					i--; if (i < 1) i = 4; okay = true;
					break;
				case'S':
				case's':
					gotoxy(x / 2 - 9, y * 1 / 3 + i); std::cout << " ";
					gotoxy(x / 2 + 9, y * 1 / 3 + i); std::cout << " ";
					i++; if (i > 4) i = 1; okay = true;
					break;
				case 13: okay = true;
					if (i == 3)
					{
						optionGame();
						return true;
					}
					else if (i == 1)
						return true;
					else if (i == 2)
					{
						std::fstream f;
						takeData(stateS, stateG);
						Type = 0;
						return true;
					}
					else exit(0);
					break;
				case  27:exit(0);
				}
			}
			if (okay == true)
				break;
		}
	}
}
void optionGame()
{
	system("cls");
	int x = takeConsoleColumns(), y = takeConsoleRows();
	gotoxy(x / 2 - 5, y * 1 / 4 + 1);
	printf("OPTION");
	gotoxy(x / 2 - 5, y * 1 / 4 + 2);
	std::cout << (Type == 7 ? "EASY  " : (Type == 15) ? "NORMAL" : (Type == 30) ? "HARD  " : "CUSTOM");
	gotoxy(x / 2 - 8, y * 1 / 3 + 1);
	printf("[1.EASY(7)   ]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 2);
	printf("[2.MEDIUM(15)]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 3);
	printf("[3.HARD(30)  ]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 4);
	printf("[4.CUSTOM    ]");
	gotoxy(x / 2 - 8, y * 1 / 3 + 5);
	std::cout << "[5." << (PvE == true ? "PvE" : "PvP") << "       ]";
	gotoxy(x / 2 - 8, y * 1 / 3 + 6);
	printf("[6.OKAY(ESC) ]");

	int i = 1;
	while (1)
	{
		gotoxy(x / 2 - 9, y * 1 / 3 + i); std::cout << ">";
		gotoxy(x / 2 + 7, y * 1 / 3 + i); std::cout << "<";
		while (1)
		{
			bool okay = false;
			if (_kbhit())
			{
				switch (_getch())
				{
				case'w':
				case'W':
					gotoxy(x / 2 - 9, y * 1 / 3 + i); std::cout << " ";
					gotoxy(x / 2 + 7, y * 1 / 3 + i); std::cout << " ";
					i--; if (i < 1) i = 6; okay = true;
					break;
				case'S':
				case's':
					gotoxy(x / 2 - 9, y * 1 / 3 + i); std::cout << " ";
					gotoxy(x / 2 + 7, y * 1 / 3 + i); std::cout << " ";
					i++; if (i > 6) i = 1; okay = true;
					break;
				case 13: okay = true;
					if (i == 4)
					{
						Type = 0;
						customGame();
						return;
					}
					else if (i == 5)
					{
						PvE = (PvE == true) ? false : true;
						gotoxy(x / 2 - 8, y * 1 / 3 + 5);
						std::cout << "[5." << (PvE == true ? "PVE" : "PvP") << "       ]";
					}
					else if (i == 6)
					{
						setUpGame();
						return;
					}
					else
					{
						Type = pow(2, i - 1) * 7.5;
						gotoxy(x / 2 - 5, y * 1 / 4 + 2);
						std::cout << (Type == 7 ? "EASY  " : (Type == 15) ? "NORMAL" : (Type == 30) ? "HARD  " : "CUSTOM");
					}
					break;
				case 27:
					setUpGame();
					return;
				}
			}
			if (okay == true)
				break;
		}
	}
}
void customGame()
{
	system("cls");
	int x = takeConsoleColumns(), y = takeConsoleRows();
	gotoxy(x / 2 - 7, y * 1 / 4 + 1);
	printf("CUSTOM GAME");
	gotoxy(x / 2 - 13, y * 1 / 3 + 1);
	printf("Let Computer Set Table. (1/else)");
	gotoxy(x / 2 - 13, y * 1 / 3 + 2);
	printf("Set Table Your Self.    (2)");
	gotoxy(x / 2 - 13, y * 1 / 3 + 3);
	printf("Input table.            (3)");
	gotoxy(x / 2 - 13, y * 1 / 3 + 4);
	printf("Exit.                   (4/ESC)");

	char i = _getch();
	if (i != '2' && i != '3' && i != 27 && i != '4')
	{
		system("cls");
		printf("Input number move :");
		std::cin >> Type;
	}
	else if (i == '2')
	{
		while (1)
		{
			system("cls");
			printf("Input goal  :"); inputTable(1);
			system("cls");
			printf("Create goal :");
			stateS = createFromTable(stateG);
			Node* start = setNode(stateS);
			Node* goal = setNode(stateG);
			std::cout << "\n\n";
			if (AStar(start, goal) == false)
			{
				std::cout << "Can't Find.";
				_getch();
			}
			else
			{
				_getch();
				break;
			}
		}
	}
	else if (i == '3')
	{
		while (1)
		{
			system("cls");
			printf("Input start :"); inputTable(0);
			printf("\nInput goal  :"); inputTable(1);
			Node* start = setNode(stateS);
			Node* goal = setNode(stateG);
			std::cout << "\n\n";
			if (AStar(start, goal) == false)
			{
				std::cout << "Can't Find.";
				_getch();
			}
			else
			{
				_getch();
				break;
			}
		}
	}
	optionGame();
}

int shakeCoin(int x, int y, bool isComputer)
{
	gotoxy((x) * 7 + 14, (y + 3) * 4 + 1);	std::cout << "---";
	if (isComputer)
		Sleep(1000);
	else
	{
		while (1)
		{
			char c = _getch();
			if (c == 32)
				break;
			else if (c == 27)
				return 2;
		}
	}
	for (int i = 0; i < 30; i++)
	{
		gotoxy((x) * 7 + 14, (y + 3) * 4 + 1); std::cout << (i % 3 == 0 ? "---" : i % 3 == 1 ? "500" : "VND");
		Sleep(10);
	}
	gotoxy((x) * 7 + 14, (y + 3) * 4 + 1);
	bool is500 = rand() % 2;
	std::cout << ((is500 == 0) ? "VND" : "500");
	return is500;

}
void executeMove(Node*& start, int cmd)
{
	if (cmd == start->child->cmd)
	{
		Node* temp = start;
		start = start->child;
		delete temp;
	}
	else
	{
		if (start->child == NULL)
			executeCmd(start->key, start->mainPos, cmd);
		else
		{
			Node* parent = childNode(start, cmd);
			parent->parent = NULL;
			parent->child = start;
			start->cmd = (cmd == 0) ? 1 : (cmd == 1) ? 0 : (cmd == 2) ? 3 : 2;
			start->parent = parent;
			start = parent;
		}
	}
}
void playGame(Node* start, Node* goal, int gotoP)
{
	system("cls");
	int x = takeConsoleColumns() / 27, y = takeConsoleRows() / 30;
	start->cmd = -1;

	showTable(start->key, y, x); showTable(goal->key, y, x + 5);
	gotoxy((x) * 7, (y + 3) * 3); std::cout << "START";
	gotoxy((x + 5) * 7, (y + 3) * 3); std::cout << "GOAL";
	gotoxy((x + 4) * 7 - 2, (y + 1) * 3);  std::cout << "=>>";
	gotoxy((x + 4) * 7 - 2, (y + 2) * 3);  std::cout << "=>>";

	gotoxy((x) * 7, (y + 3) * 4); std::cout << "LAST MOVE:";
	gotoxy((x) * 7 + 11, (y + 3) * 4); std::cout << "";
	std::cout << ((start->cmd == -1) ? "NONE " : (start->cmd == 0) ? "UP   " :
		(start->cmd == 1) ? "DOWN " : (start->cmd == 2) ? "LEFT " : "RIGHT");
	gotoxy((x) * 7, (y + 3) * 4 + 1); std::cout << "SHAKE COIN: < --- >";
	gotoxy((x) * 7, (y + 3) * 4 + 2); std::cout << " PLAYER 1 : ";
	gotoxy((x) * 7, (y + 3) * 4 + 3); std::cout << " " << (PvE == true ? "COMPUTER" : "PLAYER 2") << " : ";
	gotoxy((x) * 7, (y + 3) * 4 + 4); std::cout << "SPACE to SHAKE COIN, ESC to BACK TO MENU";

	int turn = (gotoP == 2) ? 1 : 0;
	int resultShake;

	while (1)
	{
		gotoxy((x) * 7 + 11, (y + 3) * 4); std::cout << "";
		std::cout << ((start->cmd == -1) ? "NONE " : (start->cmd == 0) ? "UP   " :
			(start->cmd == 1) ? "DOWN " : (start->cmd == 2) ? "LEFT " : "RIGHT");
		gotoxy((x) * 7, (y + 3) * 4 + 2 + turn); std::cout << ">";
		gotoxy((x) * 7 + 9, (y + 3) * 4 + 2 + turn); std::cout << "<";


		if (gotoP == 1)
		{
			gotoP = 0;
			goto P1;
		}
		else if (gotoP == 2)
		{
			gotoP = 0;
			goto P2;
		}
		resultShake = shakeCoin(x, y, (PvE == true && turn == 1) ? 1 : 0);;

		if (resultShake == 1)
		{
			int cmd;
			if (turn == 0)
			{
			P1:
				cmd = playerMove((x) * 7 + 12, (y + 3) * 4 + 2 + turn, start, goal);
			}
			else
			{
				if (PvE == false)
				{
				P2:
					cmd = playerMove((x) * 7 + 12, (y + 3) * 4 + 2 + turn, start, goal);
				}
				else cmd = computerMove((x) * 7 + 12, (y + 3) * 4 + 2 + turn, start, goal);
			}
			if (cmd == -1) return;
			executeMove(start, cmd);
			showTable(start->key, y, x);
			if (start->key == goal->key)
			{
				std::cout << "STOP";
			}
		}
		else if (resultShake == 0)
		{
			gotoxy((x) * 7, (y + 3) * 4 + 2 + turn); std::cout << " ";
			gotoxy((x) * 7 + 9, (y + 3) * 4 + 2 + turn); std::cout << " ";
			turn = (turn == 0) ? 1 : 0;
		}
		else
		{
			save(start->key, goal->key);
			return;
		}
	}
}

int playerMove(int x, int y, Node* start, Node* goal)
{
	int cmd = -1;
	while (1)
	{
		gotoxy(x, y);
		std::cout << ((cmd == -1) ? "NONE " : (cmd == 0) ? "UP   " :
			(cmd == 1) ? "DOWN " : (cmd == 2) ? "LEFT " : "RIGHT");
		int temp = -1;
		switch (_getch())
		{
		case'w':
		case'W':temp = 0;
			break;
		case's':
		case'S':temp = 1;
			break;
		case'a':
		case'A':temp = 2;
			break;
		case'D':
		case'd':temp = 3;
			break;
		case'V':
		case'v':
			showAllStep(start, 0, 0, 0);
			playGame(start, goal, (y % 2 == 0 ? 1 : 2));
			return -1;
			break;
		case 13:
			if (cmd != -1)
			{
				gotoxy(x, y);
				std::cout << "     ";
				return cmd;
			}
		case 27:
			return -1;
		}
		if (temp != -1)
		{
			if (isLegal(start->mainPos, temp))
				cmd = temp;
			else cmd = -1;
		}
	}
}
int computerMove(int x, int y, Node*& start, Node*& goal)
{
	int cmd;
	if (goal->cmd != -1)
	{
		int step = goal->depth - start->depth;
		if (step < 3 || step>4 || rand() % 7 == 0)
			cmd = start->child->cmd;
		else
		{
			do
			{
				cmd = rand() % 4;
			} while (cmd == start->child->cmd || isLegal(start->mainPos, cmd) == false);
		}
	}
	else
	{
		if (AStar(start, goal) == true)
			cmd = start->child->cmd;
		else
		{
			int cmd = -1, minH = 9999;
			for (int i = 0; i < 4; i++)
			{
				if (isLegal(start->mainPos, i))
				{
					int key = start->key;
					int mainPos = start->mainPos;
					executeCmd(key, mainPos, i);
					int h = Heuristic(key, goal->key);
					if (h < minH)
					{
						minH = h;
						cmd = i;
					}
				}
			}
		}
	}
	gotoxy(x, y);
	std::cout << ((cmd == -1) ? "NONE " : (cmd == 0) ? "UP   " :
		(cmd == 1) ? "DOWN " : (cmd == 2) ? "LEFT " : "RIGHT");
	Sleep(300);
	gotoxy(x, y); std::cout << "     ";
	return cmd;
}

