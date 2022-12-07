#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <bitset>
#include <Windows.h>
#include <filesystem>
#include <stdio.h>

#define BITSNUMBYTE 8

using namespace std;

// 허프만 트리를 구성할 노드
class Node 
{
public:
    char Character; // 글자
    int Freq; // 빈도수(합)
    Node* left = NULL; // 왼쪽가지
    Node* right = NULL; // 오른쪽가지

    // 노드 생성
    Node(char character, int value, Node* nLeft = NULL, Node* nRight = NULL) {
        Character = character;
        Freq = value;
        left = nLeft;
        right = nRight;
    }
};

// 우선순위 큐를 사용하기 위한 비교함수
struct cmp 
{
    // 내림차순 만들기
    bool operator()(Node& a, Node& b) {
        return a.Freq > b.Freq;
    }
};

void Make_tree(priority_queue<Node, vector<Node>, cmp>& p) 
{
    while (true) {
        // size_t == unsigned int == 부호없는 32(or 64)bit 정수, 메모리/문자열 길이 구할때 사용
        size_t size = p.size(); // 노드의 개수
        if (size < 2) break; // 노드의 개수가 2개 이상이여야 가능

        // p.top이 가장 작은 값을 가짐을 이용 -> 노드를 생성후 pop
        Node* firstMin_node = new Node(p.top().Character, p.top().Freq, p.top().left, p.top().right);
        p.pop();

        Node* secondMin_node = new Node(p.top().Character, p.top().Freq, p.top().left, p.top().right);
        p.pop();

        int newValue = firstMin_node->Freq + secondMin_node->Freq; // 새로 생긴 부모노드의 Value값 만들기
        
        Node* newNode = new Node(NULL, newValue); // 두 노드의 부모노드를 생성
        newNode->left = firstMin_node; // 자식노드와 연결
        newNode->right = secondMin_node;
        p.push(*newNode); // 우선순위 큐에 넣어주기
    }
}

void Character_mapping(string str, Node node, unordered_map<char, string>& characterMap) 
{
    if (node.Character != NULL) // Character가 있다 == 마지막 노드이다(부모노드를 만들때 character은 Null)
    {
        characterMap[node.Character] = str; // 이동하면서 만들어진 코드를 저장
        return; // 재귀함수 종료
    }

    str += "0"; // 왼쪽 노드로 갈땐 0을
    Character_mapping(str, *node.left, characterMap);
    str[str.size() - 1] = '1'; // 오른쪽 노드로 갈땐 1을
    Character_mapping(str, *node.right, characterMap);
}

//1 byte를 입력으로 받아, 길이 len 만큼 binary string을 만들어 반환
string Byte_To_BitsString(unsigned char oneByte, int len) {
    string bits_string = "";
    // binary string 만들고 반환
    for (int i = 7; i >= 0; i--) { // 8비트를 각각 string으로 바꾸는 과정
        if (len <= i) // 비트수가 적을 것을 대비
            continue;
        unsigned char po = pow(2, i); // 비트 자리수 이동
        if (oneByte >= po) {
            bits_string += '1'; // 해당 비트 자리수가 1이면
            oneByte -= po;
        }
        else {
            bits_string += '0'; // 해당 비트 자리수가 0이면
        }
    }
    return bits_string;
}

void Encoder(const string& txt, unordered_map<char, string>& characterMap) {
    ofstream fout; //파일에 쓰기
    string encoded_txt = ""; //인코딩될 내용을 저장

    printf("\n===================== Encoder시작 =======================\n");
    fout.open("./encoding_text.txt", ios::out | ios::binary); // 압축할 파일 선택(write위한 type binary 파일 open)

    size_t size = characterMap.size(); // 존재하는 문자 수 구하기
    unsigned char uc_count = size; 
    fout << uc_count; // 문자가 몇개 있는지 write
    cout << "uc_count : " << bitset<4>(uc_count) << "\n";
    //encoding 시작
    unsigned char uc = 0;
    cout << "bit_len : ";
    for (auto& i : characterMap) {
        uc = i.first; //characterMap의 첫번째부터
        fout << uc;  // 해당 문자를 write
        unsigned char bit_len = i.second.size();
        fout << bit_len;  // 비트 길이 기록

        
        unsigned char bit = 0; 
        for (size_t j = 0; j < bit_len;) { 
            for (int k = 0; k < BITSNUMBYTE; k++, j++) { //BITSNUMBYTE = 8
                if (j > bit_len - 1) break; //문자의 길이를 초과하기전 반복 종료
                bit = (bit << 1) | (i.second[j] & 1); // 비트값을 비트연산자를 통해 구함
            }
            fout << bit; // 비트값 기록
            cout << bitset<4>(bit_len) << "|";
            bit = 0;
        }
    }

    printf("\n");
    //encoding된 encodedTxt 만들기
    size = txt.size(); 
    for (size_t i = 0; i < size; i++) {
        char cur_char = txt[i]; //기존의 txt파일을 하나하나 얻어서
        encoded_txt += characterMap[cur_char]; //해당 문자를 encodedTxt에 추가(비트)   
    }

    // encodedTxt의 8bits를 1byte로
    unsigned char remain_bit = 0; // bit 개수가 8의 배수가 아닐 경우, 나머지 bit처리
    size = encoded_txt.size(); 
    unsigned char str_bit = 0;
    for (size_t i = 0; i < size - 1;) {
        for (int k = 0; k < BITSNUMBYTE; k++, i++) {
            if (i > size - 1) {
                remain_bit = (unsigned char)k; // remain_bit은 마지막에 있는 8비트중 실제 비트가 몇비트인지
                break;
            }
            str_bit = (str_bit << 1) | (encoded_txt[i] & 1); // 8비트씩 비트를 저장
        }
        fout << str_bit; // 8비트씩 저장된 비트를 기록
        cout << bitset<8>(str_bit) << "|";
        str_bit = 0;
    }
    printf("\n");
    // remainBit는 file의 마지막에 기록
    fout << remain_bit;
    cout << "remain_bit : " << bitset<4>(remain_bit) << "\n";
    fout.close();
}



void Decoder() {
    printf("\n===================== Decoder시작 =======================\n");

    ifstream fin;
    ofstream fout;
    fin.open("./encoding_text.txt", ios::in | ios::binary); //binary파일 읽기
    fout.open("./HuffmanOutput.txt"); //파일 쓰기

    // file의 길이 구하기
    fin.seekg(0, ios::end); //파일의 첫위치에서 마지막으로 이동
    int file_len = fin.tellg(); //마지막의 위치를 fileLen에 저장
    fin.seekg(0, ios::beg); //초기화
    cout << "file_len : " << file_len << endl;

    // character 개수 세기
    int map_count = 0; 
    map_count = fin.get(); //복구할 파일의 총 문자수 구하기
    int hash_len = 1; //이후 사용됨

    // characterMap 복구
    unordered_map<string, char> character_map;
    for (int i = 0; i < map_count; i++) {
        char character = 0;
        fin.get(character); //각각 character의 개수 구하기

        hash_len += 2;
        int len = 0;
        len = fin.get(); //기록했던 bit_len 얻어냄
        unsigned char value = 0;
        string value_str = "";
        for (int j = 0; j < len;) {
            value = fin.get(); //기록했던 bit값을 "8bit씩" 얻어냄
            //cout << "(" << bitset<8>(value) << ")";
            if (len > BITSNUMBYTE) { 
                value_str += Byte_To_BitsString(value, BITSNUMBYTE); //만약 비트수가 많으면 8비트씩 나눠서
                //cout << value_str << " | ";
            }
            else {
                value_str += Byte_To_BitsString(value, len);
                //cout << value_str << " || ";
            }
            hash_len++;
            len -= BITSNUMBYTE;
        }

        character_map[value_str] = character; //character_map을 복구하면서 decoding할 수 있도록
    }

    // encoding.txt 를 읽어 binary string인 txt를 만듦
    file_len -= hash_len; //encoding_text.txt가 어디까지 읽었는지 파악하기 위함
    cout << "file_len 2 : " << file_len << endl;
    unsigned char uc_char = 0;
    string txt = "";
    while (true) {
        uc_char = fin.get(); //기록했던 bit 얻어냄
        cout << "ucCharacter : " << bitset<8>(uc_char) << "| file_len : " << file_len << endl;
        if (fin.fail()) break;
        if (file_len > 2) {
            txt += Byte_To_BitsString(uc_char, BITSNUMBYTE); //8비트씩 txt에 string형식으로 저장
        }
        else {
            unsigned char remain_bit = fin.get();
            cout << "remain_bit : " << bitset<8>(remain_bit) << " -> ";
            cout << Byte_To_BitsString(uc_char, (int)remain_bit) << endl;
            txt += Byte_To_BitsString(uc_char, (int)remain_bit); //remain_bit 역시 저장
        }
        file_len -= 1;
    }

    // characterMap과 binary string인 txt를 비교하여 HuffmanOutput을 기록
    printf("\n");
    string tmp_txt = "";
    for (auto& i : txt) {
        tmp_txt += i; //맨 처음글자부터 
        if (character_map.count(tmp_txt)) { //character_map에 있는지 확인
            fout << character_map[tmp_txt]; //그에 맞게
            cout << character_map[tmp_txt];
            tmp_txt = "";
        }
    }

    fin.close();
    fout.close();
}

int main()
{
    printf("AAAAAAABBCCCDEEEEFFFFFFG\n");
    ifstream fin;// input file sream == 파일을 프로그램에 넣을 수 있게 해준다
    fin.open("./HuffmanInput.txt");// test.txt를 열기

    // unorderd_map == map보다 더 빠른 해시테이블로 구현한 자료구조, 중복된 데이터 허용x, 데이터양 많을수록 유리, 유사데이터 처리는 힘듦
    unordered_map<char, int> freq_map; // 빈도수를 저장해줄 hashmap
    unordered_map<char, string> character_map; // 문자를 저장해줄 charactermap
    string txt = "";

    char character; 
    while (true) {
        fin.get(character); // get을 이용해 character를 얻고
        if (fin.fail()) break; // 모든 문자를 다 읽으면 종료
        freq_map[character]++; // hashmap을 이용해 해당 character의 빈도수를 추가
        txt += character; // txt에 읽은 character를 넣어 최종적으로 file 내용 옮기기
    }

    priority_queue<Node, vector<Node>, cmp> p; // 우선순위 큐
    // hashmap에 있는 character들을 2개씩 노드화 해서 넣어준다 -> 오름차순 정렬
    // 이론적으론 내림차순이지만 큐의 특성상 pop의 사용을 위해 오름차순으로 정렬해준다.
    for (auto& i : freq_map) {
        Node node(i.first, i.second); 
        p.push(node);
    }

    Make_tree(p); // 허프만트리 생성
    Character_mapping("", p.top(), character_map); //character를 맵핑

    // 어떻게 압축?
    for (auto& i : character_map) {
        printf("%c=", i.first);
        cout << i.second << "|";
    }

    Encoder(txt, character_map);

    Decoder();

}
