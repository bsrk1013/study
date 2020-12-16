#include <iostream>
#include <thread>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <list>
#include <algorithm>
#include "DBBD/TcpClient.h"
#include "DBBD/Request.h"
#include "DBBD/Serialize.h"
#include "DBBD/Deserialize.h"
#include "DBBD/Cell.h"

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

void baram1() {
    string s = "avgcshavcghxzvghcavsghczazxcaxxacxzcaz";

    int answer = 0;

    int index1 = 0;
    int index2 = 1;
    while (true) {
        if (index1 + index2 >= s.length()) {
            break;
        }

        char a = s[index1];
        if (a != 'a'
            && a != 'z') {
            index1++;
            continue;
        }

        bool isZ = a == 'z';

        if (index1 + index2 >= s.length()) {
            index2 = 1;
        }

        char b = s[index1 + index2];
        if (b != 'a'
            && b != 'z') {
            index2++;
            continue;
        }

        index1 = index1 + index2;
        index2 = 1;
        if (isZ && b != 'z'
            || !isZ && b == 'z') {
            answer++;
        }
    }

    std::cout << "answer1 : " << answer << endl;
}

void baram2() {
    vector<string> answer;
    vector<string> card{"ABACDEFG", "NOPQRSTU", "HIJKLKMM"};
    vector<string> word{"GPQM", "GPMZ", "EFU", "MMNA"};

    for (auto wordData : word) {
        bool result = true;
        vector<string> tempCard(card);
        int one = 0, two = 0, three = 0;
        for (size_t i = 0; i < wordData.size(); i++) {
            char charData = wordData[i];

            //map<int, int> result;

            size_t a = tempCard[0].find(charData);
            size_t b = tempCard[1].find(charData);
            size_t c = tempCard[2].find(charData);

            // 만들 수 없음
            if (a >= tempCard[0].size()
                && b >= tempCard[1].size()
                && c >= tempCard[2].size()) {
                result = false;
                break;
            }
            
            if (a < tempCard[0].size()) { tempCard[0].erase(a, 1); one++; continue; }
            if (b < tempCard[1].size()) { tempCard[1].erase(b, 1); two++;  continue; }
            if (c < tempCard[2].size()) { tempCard[2].erase(c, 1); three++;  continue; }
        }

        if (result
            && one > 0
            && two > 0
            && three > 0) {
            answer.push_back(wordData);
        }
    }

    if (answer.size() <= 0) {
        answer.push_back("-1");
    }
}

void baram3() {
	vector<int> stats{ 5, 3, 4, 6, 2, 1 };
	vector<stack<int>> teams;

	for (auto stat : stats) {
		bool insert = true;
		for (auto &team : teams) {
			if (team.top() < stat) {
				team.push(stat);
				insert = false;
				break;
			}
		}

		if (insert) {
			stack<int> team;
			team.push(stat);
			teams.push_back(team);
		}
	}

    std::cout << "answer3 : " << teams.size() << endl;
}

void baram4() {
    int n = 7;
    vector<vector<int>> quests{ vector<int>{7, 1}, vector<int>{7, 2}, vector<int>{6, 5}, vector<int>{3, 2} };

    typedef set<int, greater<int>> questSet;
    map<int, questSet> questMap;
    int questArray[5001]{ 0 };
    vector<int> answer;
     
    for (int i = 1; i <= n; i++) {
        questMap[i];
        for (auto quest : quests) {
            if (quest[1] == i) {
                questMap[i].insert(quest[0]);
            }
        }
    }

    int index = 1;
    while (true) {
        if (answer.size() == n) { break; }
        if (index > n) {
            index = 1;
        }

        auto a = questMap[index];
        if (questArray[index] == 1) { index++; continue; }
        if (a.size() <= 0) {
            answer.push_back(index);
            questArray[index] = 1;
            index = 1;
            continue;
        }

        bool possible = true;
        for (auto b : a) {
            if (questArray[b] == 0) {
                possible = false;
                break;
            }
        }

        if (possible) {
            questMap[index].clear();
            answer.push_back(index);
            questArray[index] = 1;
            index = 1;
            continue;
        }

        index++;
    }

    std::cout << "answer4 : " << endl;
}

void baram5() {
    int N = 5;
    vector<vector<int>> relation = { vector<int>{1, 2}, vector<int>{4, 2}, vector<int>{3, 1}, vector<int>{4, 5} };
    vector<int> answer;

    for (int i = 1; i <= N; i++) {
        int total = 0;
        for (auto data : relation) {
            // 볼 필요 없음(둘 다 다름)
            if(data[0] != i
                && data[1] != i) {
                continue;
            }
            
            int target = data[0] == i ? data[1] : data[0];
            total++;

            for (auto data2 : relation) {
                // 볼 필요 없음
                if(data[0] == data2[0]
                    && data[1] == data2[1]) {
                    continue;
                }

                if (data2[0] != target
                    && data2[1] != target) {
                    continue;
                }
                total++;
            }
        }
        answer.push_back(total);
    }

    std::cout << "answer5 : " << endl;
}

class ChattingReq : DBBD::Request {
public:
    ChattingReq() { typeId = 1; }
    virtual ~ChattingReq() {}

    // Request을(를) 통해 상속됨
    virtual void serialize(DBBD::Buffer& buffer)
    {
        writeHeader(buffer, getLength());
        DBBD::Serialize::write(buffer, msg);
    }
    virtual void deserialize(DBBD::Buffer& buffer)
    {
        readHeader(buffer);
        DBBD::Deserialize::read(buffer, msg);
    }

    virtual size_t getLength() {
        return Request::getLength() + sizeof(size_t) + msg.size();
    }

public:
    std::string getMsg() { return msg; }
    void setMsg(std::string value) { msg = value; }

private:
    std::string msg;
};

int main() {
    /*baram1();
    baram2();
    baram3();
    baram4();
    baram5();*/
    /*program1();
    program2();
    program3();*/

    std::vector<DBBD::TcpClient*> clientList;
    std::vector<std::thread*> threadList(100);

	try {

        for (size_t i = 0; i < 10; i++) {
            std::thread* pThread = new std::thread([&clientList](){
                for (size_t j = 0; j < 5; j++) {
                    DBBD::TcpClient* client = new DBBD::TcpClient("127.0.0.1", 8100);
                    clientList.push_back(client);
                }

                while (true) {}
            });

            threadList.push_back(pThread);
		}

		// DBBD::TcpClient client("127.0.0.1", 8100);
		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				break;
			}

            ChattingReq chatReq;
            chatReq.setMsg(a);

            for (auto client : clientList) {
                client->send((DBBD::Cell*)&chatReq);
            }
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}