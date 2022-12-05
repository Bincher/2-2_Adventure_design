#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <bitset>

#define BITSNUMBYTE 8

using namespace std;

// 허프만 트리를 구성할 노드
class Node 
{
public:
    char mCharacter; // 글자
    int mFreq; // 빈도수(합)
    Node* left = NULL; // 왼쪽가지
    Node* right = NULL; // 오른쪽가지

    // 노드 생성
    Node(char character, int value, Node* nLeft = NULL, Node* nRight = NULL) {
        mCharacter = character;
        mFreq = value;
        left = nLeft;
        right = nRight;
    }
};


// 우선순위 큐를 사용하기 위한 비교함수
struct cmp 
{
    // 내림차순 만들기
    bool operator()(Node& a, Node& b) {
        return a.mFreq > b.mFreq;
    }
};


// ********************
// @ makeTree
// @ 우선순위 큐(priority_queue)를 이용해 tree 만듦
// @ 1. 최소 빈도수 노드 두 개 고르고, 우선순위 큐에서 pop
// @ 2. 최소 빈도수 노드 두 개의 합으로 노드를 하나 만들고, 두 개의 노드는 자식으로 만듦
// @ 3. 만든 노드를 우선순위 큐에 push
// ********************
/*
* 우선순위큐
* priority_queue<자료형, 구조체, 비교함수>
*/
void makeTree(priority_queue<Node, vector<Node>, cmp>& p) 
{
    while (true) {
        // size_t == unsigned int == 부호없는 32(or 64)bit 정수, 메모리/문자열 길이 구할때 사용
        size_t size = p.size(); // 노드의 개수
        if (size < 2) break; // 노드의 개수가 2개 이상이여야 가능

        // p.top이 가장 작은 mValue값을 가짐을 이용 -> 노드를 생성후 pop
        Node* firstMinNode = new Node(p.top().mCharacter, p.top().mFreq, p.top().left, p.top().right);
        //printf("%c |", p.top().mCharacter);
        p.pop();
        Node* secondMinNode = new Node(p.top().mCharacter, p.top().mFreq, p.top().left, p.top().right);
        //printf("%c ||", p.top().mCharacter);
        p.pop();
        int newValue = firstMinNode->mFreq + secondMinNode->mFreq; // 새로 생긴 부모노드의 mValue값 만들기
        
        Node* newNode = new Node(NULL, newValue); // 두 노드의 부모노드를 생성
        newNode->left = firstMinNode; // 자식노드와 연결
        newNode->right = secondMinNode;
        p.push(*newNode); // 우선순위 큐에 넣어주기
    }
}


// ********************
// @ characterMapping
// @ mapping char-string. using unordered_map<char, string> characterMap
// @ tree(node)를 받아서 tree를 순회하면서 mapping 수행
// ********************
void characterMapping(string str, Node node, unordered_map<char, string>& characterMap) 
{
    // 맨 처음 str은 공백
    if (node.mCharacter != NULL) // mCharacter가 있다 == 마지막 노드이다(부모노드를 만들때 mcharacter은 Null)
    {
        characterMap[node.mCharacter] = str; // 이동하면서 만들어진 코드를 저장
        //cout << node.mCharacter << endl;
        //cout << str << endl;
        return; // 재귀함수 종료
    }

    str += "0"; // 왼쪽 노드로 갈땐 0을
    characterMapping(str, *node.left, characterMap);
    str[str.size() - 1] = '1'; // 오른쪽 노드로 갈땐 1을
    characterMapping(str, *node.right, characterMap);
}


// ********************
// @ byteToBitsString
// @ 1 byte를 입력으로 받아, 길이 len 만큼 binary string을 만들어 반환
// ********************
string byteToBitsString(unsigned char oneByte, int len) {
    string bitsString = ""; 
    // binary string 만들고 반환
    for (int i = 7; i >= 0; i--) {
        if (len <= i) 
            continue;
        unsigned char po = pow(2, i);
        if (oneByte >= po) {
            bitsString += '1';
            oneByte -= po;
        }
        else {
            bitsString += '0';
        }
    }
    return bitsString;
}


// ********************
// @ encoder
// @ mapping된 characterMap, HuffmanInput.txt에서 읽은 string txt를 가져옴
// @ txt에서 한글자씩 읽어 characterMap과 비교.
// @ 각 글자에 해당하는 binary를 encoding_text.txt에 write
// ********************
void encoder(const string& txt, unordered_map<char, string>& characterMap) {
    ofstream fout; //파일에 쓰기
    string encodedTxt = ""; //인코딩될 내용을 저장

    printf("===================== encoder시작 =======================\n");
    fout.open("./encoding_text.txt", ios::out | ios::binary); // 압축할 파일 선택(write위한 type binary 파일 open)

    // mapping된 characterMap 기록
    size_t size = characterMap.size(); // 존재하는 문자 수 구하기
    unsigned char ucSize = size; 
    fout << ucSize; // 문자가 몇개 있는지 write
    printf("문자 개수 = %d\n", ucSize);

    // key, value bits len, value bits 기록
    unsigned char uc = 0;
    //encoding 시작
    for (auto& i : characterMap) {
        uc = i.first; //characterMap의 첫번째부터
        fout << uc;  // 해당 문자를 write
        //printf("문자 %c / ", uc);
        unsigned char len = i.second.size();
        fout << len;  // value bits 길이 기록
        
        //printf("%d", i.second.size());

        // value bits 구하기
        unsigned char summ = 0; 
        for (size_t j = 0; j < len;) { 
            //BITSNUMBYTE = 8
            for (int k = 0; k < BITSNUMBYTE; k++, j++) {
                if (j > len - 1) break; //문자의 길이를 초과하기전 반복 종료
                summ = (summ << 1) | (i.second[j] & 1); // value bits 계산 과정
            }
            fout << summ; // value bits 기록
            printf("summ = %d\n", summ);
            summ = 0;
        }
    }

    // 허프만 기법으로 encoding된 binary encodedTxt 만들기
    size = txt.size(); 
    for (size_t i = 0; i < size; i++) {
        char currentCharacter = txt[i]; //기존의 txt파일을 하나하나 얻어서
        encodedTxt += characterMap[currentCharacter]; //해당 문자를 encodedTxt에 추가(비트)   
    }

    // encodedTxt의 8bits를 1byte로 기록
    unsigned char remainBit = 0; // bit 개수가 8의 배수가 아닐 경우, 나머지 bit처리
    size = encodedTxt.size(); 
    unsigned char sum = 0;
    for (size_t i = 0; i < size - 1;) {
        for (int k = 0; k < BITSNUMBYTE; k++, i++) {
            if (i > size - 1) {
                remainBit = (unsigned char)k;
                break;
            }
            sum = (sum << 1) | (encodedTxt[i] & 1); 
        }
        fout << sum;
        cout << "sum : " << bitset<4>(sum) << "\n";
        sum = 0;
    }

    // remainBit는 file의 마지막에 기록
    fout << remainBit;
    cout << "reaminBit : " << bitset<4>(remainBit) << "\n";
    fout.close();
}


// ********************
// @ decoder
// @ encoding_text.txt를 불러옴
// @ encoding_text.txt에서 한 bit씩 읽어 앞 부분의 map 읽고, map 복구. 
// @ map 이후부터 encoding data 읽음.
// @ 마지막이 8bits로 안 나눠 떨어질 수 있음. 남은 bits만큼 읽기.
// ********************
void decoder() {
    ifstream fin;
    ofstream fout;
    fin.open("./encoding_text.txt", ios::in | ios::binary); //binary파일 읽기
    fout.open("./HuffmanOutput.txt"); //파일 쓰기

    // file의 길이 구하기
    fin.seekg(0, ios::end); //파일의 첫위치에서 마지막으로 이동
    int fileLen = fin.tellg(); //마지막의 위치를 fileLen에 저장
    fin.seekg(0, ios::beg); //초기화

    // character 개수 세기
    int mapCount = 0; 
    mapCount = fin.get(); //복구할 파일의 총 문자수 구하기
    int hashLen = 1;

    // characterMap 복구
    unordered_map<string, char> characterMap;
    for (int i = 0; i < mapCount; i++) {
        char character = 0;
        fin.get(character); //각각 character의 개수 구하기

        hashLen += 2; 
        int len = 0;
        len = fin.get();

        unsigned char value = 0;
        string valueString = "";
        for (int j = 0; j < len;) {
            value = fin.get();
            if (len > BITSNUMBYTE) {
                valueString += byteToBitsString(value, BITSNUMBYTE);
            }
            else {
                valueString += byteToBitsString(value, len);
            }
            hashLen++;
            len -= BITSNUMBYTE;
        }

        characterMap[valueString] = character;
        //printf("%c ", character);
    }
    printf("\n");

    // encoding.txt 를 읽어 binary string인 txt를 만듦
    fileLen -= hashLen;
    unsigned char ucCharacter = 0;
    string txt = "";
    while (true) {
        ucCharacter = fin.get();
        if (fin.fail()) break;
        if (fileLen > 2) {
            txt += byteToBitsString(ucCharacter, BITSNUMBYTE);
        }
        else {
            unsigned char remainBit = fin.get();
            txt += byteToBitsString(ucCharacter, (int)remainBit);
        }
        fileLen -= 1;
    }

    // characterMap과 binary string인 txt를 비교하여 HuffmanOutput을 기록
    string tmpTxt = "";
    for (auto& i : txt) {
        tmpTxt += i;
        if (characterMap.count(tmpTxt)) {
            fout << characterMap[tmpTxt];
            tmpTxt = "";
        }
    }

    fin.close();
    fout.close();
}


int main()
{
    
    ifstream fin;// input file sream == 파일을 프로그램에 넣을 수 있게 해준다
    fin.open("./HuffmanInput.txt");// test.txt를 열기

    // unorderd_map == map보다 더 빠른 해시테이블로 구현한 자료구조, 중복된 데이터 허용x, 데이터양 많을수록 유리, 유사데이터 처리는 힘듦
    unordered_map<char, int> freqMap; // 빈도수를 저장해줄 hashmap
    unordered_map<char, string> characterMap; // 문자를 저장해줄 charactermap
    string txt = "";

    char character; 
    while (true) {
        fin.get(character); // get을 이용해 character를 얻고
        if (fin.fail()) break; // 모든 문자를 다 읽으면 종료
        freqMap[character]++; // hashmap을 이용해 해당 character의 빈도수를 추가
        txt += character; // txt에 읽은 character를 넣어 최종적으로 file 내용 옮기기
    }

    priority_queue<Node, vector<Node>, cmp> p; // 우선순위 큐
    // hashmap에 있는 character들을 2개씩 노드화 해서 넣어준다 -> 오름차순 정렬
    // 이론적으론 내림차순이지만 큐의 특성상 pop의 사용을 위해 오름차순으로 정렬해준다.
    for (auto& i : freqMap) {
        Node node(i.first, i.second); 
        p.push(node);
    }
    printf("\n");

    makeTree(p); // 허프만트리 생성
    characterMapping("", p.top(), characterMap); //character를 맵핑

    /*
    for (auto& i : characterMap) {
        printf("%c", i.first);
        cout << i.second << endl;
    }
    */

    encoder(txt, characterMap);
    decoder();

    fin.close();
}
