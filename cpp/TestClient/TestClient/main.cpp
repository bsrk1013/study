#include <iostream>
#include <thread>
#include <vector>
#include <stack>
#include <queue>
#include "DBBD\TcpClient.h"

using namespace std;

void program1() {
    vector<string> names{"soda", "soda", "soda", "soda", "soda" };
    int answer = 0;

    map<char, int> d;
    for (auto cookie : names) {
        char first = cookie.at(0);
        d[first] += 1;
    }

    for (auto pair : d) {
        int result = pair.second % 2;
        if (result == 0) {
            int a = pair.second / 2;

        }
    }
}

void program2() {
    vector<int> snowballs{ -5, 5 };

    stack<int> left;
    queue<int> right;

    for (size_t i = 0; i < snowballs.size(); i++) {
        int snow = snowballs[i];
        if (snow > 0) {
            if (right.size() > 0
                && left.size() > 0) {
                int rightSnow = right.front();
                if (rightSnow == snow) {
                    right.pop();
                }
                else if (rightSnow < snow) {
                    i--;
                    right.pop();
                }
            }
            else {
                left.push(snow);
            }
        }
        else {
            if (left.size() > 0) {
                int leftSnow = left.top();
                snow = snow * -1;
                if (leftSnow == snow) {
                    left.pop();
                }
                else if (leftSnow < snow) {
                    i--;
                    left.pop();
                }
            }
            else {
                right.push(snow * -1);
            }
        }
    }

    vector<int> answer;
    if (right.size() > 0) {
        while (right.size() > 0) {
            int snow = right.front() * -1;
            right.pop();
            answer.push_back(snow);
        }
    }

    if (left.size() > 0) {
        vector<int> result;
        while (left.size() > 0) {
            int snow = left.top();
            left.pop();
            result.push_back(snow);
        }

        for (auto iter = result.rbegin(); iter != result.rend(); iter++) {
            answer.push_back(*iter);
        }
    }
}

void program3() {
    vector<int> A{1, 3, 2, 4, 2, 3, 5};
    int K = 10;
    int answer = 0;

    int count = 0;
    while (true) {
        int first = 0;
        for (auto it = A.begin(); it != A.end();) {
            int value = *it;
            if (first == 0 && value != 0) {
                first = value;
                it = A.erase(it);
            }
            else if(value != 0){
                if (first + 1 == value) {
                    first++;
                    it = A.erase(it);
                }
                else {
                    it++;
                }
            }
            else {
                it++;
            }
        }
        answer++;

        if (A.size() <= 0) {
            break;
        }
    }

    std::cout << answer << std::endl;
}

int main() {
    
    /*program1();
    program2();
    program3();*/
	try {
		DBBD::TcpClient client("127.0.0.1", 8100);
		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				break;
			}

			client.send(a);
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}