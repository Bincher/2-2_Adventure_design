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

// ������ Ʈ���� ������ ���
class Node 
{
public:
    char Character; // ����
    int Freq; // �󵵼�(��)
    Node* left = NULL; // ���ʰ���
    Node* right = NULL; // �����ʰ���

    // ��� ����
    Node(char character, int value, Node* nLeft = NULL, Node* nRight = NULL) {
        Character = character;
        Freq = value;
        left = nLeft;
        right = nRight;
    }
};

// �켱���� ť�� ����ϱ� ���� ���Լ�
struct cmp 
{
    // �������� �����
    bool operator()(Node& a, Node& b) {
        return a.Freq > b.Freq;
    }
};

void Make_tree(priority_queue<Node, vector<Node>, cmp>& p) 
{
    while (true) {
        // size_t == unsigned int == ��ȣ���� 32(or 64)bit ����, �޸�/���ڿ� ���� ���Ҷ� ���
        size_t size = p.size(); // ����� ����
        if (size < 2) break; // ����� ������ 2�� �̻��̿��� ����

        // p.top�� ���� ���� ���� ������ �̿� -> ��带 ������ pop
        Node* firstMin_node = new Node(p.top().Character, p.top().Freq, p.top().left, p.top().right);
        p.pop();

        Node* secondMin_node = new Node(p.top().Character, p.top().Freq, p.top().left, p.top().right);
        p.pop();

        int newValue = firstMin_node->Freq + secondMin_node->Freq; // ���� ���� �θ����� Value�� �����
        
        Node* newNode = new Node(NULL, newValue); // �� ����� �θ��带 ����
        newNode->left = firstMin_node; // �ڽĳ��� ����
        newNode->right = secondMin_node;
        p.push(*newNode); // �켱���� ť�� �־��ֱ�
    }
}

void Character_mapping(string str, Node node, unordered_map<char, string>& characterMap) 
{
    if (node.Character != NULL) // Character�� �ִ� == ������ ����̴�(�θ��带 ���鶧 character�� Null)
    {
        characterMap[node.Character] = str; // �̵��ϸ鼭 ������� �ڵ带 ����
        return; // ����Լ� ����
    }

    str += "0"; // ���� ���� ���� 0��
    Character_mapping(str, *node.left, characterMap);
    str[str.size() - 1] = '1'; // ������ ���� ���� 1��
    Character_mapping(str, *node.right, characterMap);
}

//1 byte�� �Է����� �޾�, ���� len ��ŭ binary string�� ����� ��ȯ
string Byte_To_BitsString(unsigned char oneByte, int len) {
    string bits_string = "";
    // binary string ����� ��ȯ
    for (int i = 7; i >= 0; i--) { // 8��Ʈ�� ���� string���� �ٲٴ� ����
        if (len <= i) // ��Ʈ���� ���� ���� ���
            continue;
        unsigned char po = pow(2, i); // ��Ʈ �ڸ��� �̵�
        if (oneByte >= po) {
            bits_string += '1'; // �ش� ��Ʈ �ڸ����� 1�̸�
            oneByte -= po;
        }
        else {
            bits_string += '0'; // �ش� ��Ʈ �ڸ����� 0�̸�
        }
    }
    return bits_string;
}

void Encoder(const string& txt, unordered_map<char, string>& characterMap) {
    ofstream fout; //���Ͽ� ����
    string encoded_txt = ""; //���ڵ��� ������ ����

    printf("\n===================== Encoder���� =======================\n");
    fout.open("./encoding_text.txt", ios::out | ios::binary); // ������ ���� ����(write���� type binary ���� open)

    size_t size = characterMap.size(); // �����ϴ� ���� �� ���ϱ�
    unsigned char uc_count = size; 
    fout << uc_count; // ���ڰ� � �ִ��� write
    cout << "uc_count : " << bitset<4>(uc_count) << "\n";
    //encoding ����
    unsigned char uc = 0;
    cout << "bit_len : ";
    for (auto& i : characterMap) {
        uc = i.first; //characterMap�� ù��°����
        fout << uc;  // �ش� ���ڸ� write
        unsigned char bit_len = i.second.size();
        fout << bit_len;  // ��Ʈ ���� ���

        
        unsigned char bit = 0; 
        for (size_t j = 0; j < bit_len;) { 
            for (int k = 0; k < BITSNUMBYTE; k++, j++) { //BITSNUMBYTE = 8
                if (j > bit_len - 1) break; //������ ���̸� �ʰ��ϱ��� �ݺ� ����
                bit = (bit << 1) | (i.second[j] & 1); // ��Ʈ���� ��Ʈ�����ڸ� ���� ����
            }
            fout << bit; // ��Ʈ�� ���
            cout << bitset<4>(bit_len) << "|";
            bit = 0;
        }
    }

    printf("\n");
    //encoding�� encodedTxt �����
    size = txt.size(); 
    for (size_t i = 0; i < size; i++) {
        char cur_char = txt[i]; //������ txt������ �ϳ��ϳ� ��
        encoded_txt += characterMap[cur_char]; //�ش� ���ڸ� encodedTxt�� �߰�(��Ʈ)   
    }

    // encodedTxt�� 8bits�� 1byte��
    unsigned char remain_bit = 0; // bit ������ 8�� ����� �ƴ� ���, ������ bitó��
    size = encoded_txt.size(); 
    unsigned char str_bit = 0;
    for (size_t i = 0; i < size - 1;) {
        for (int k = 0; k < BITSNUMBYTE; k++, i++) {
            if (i > size - 1) {
                remain_bit = (unsigned char)k; // remain_bit�� �������� �ִ� 8��Ʈ�� ���� ��Ʈ�� ���Ʈ����
                break;
            }
            str_bit = (str_bit << 1) | (encoded_txt[i] & 1); // 8��Ʈ�� ��Ʈ�� ����
        }
        fout << str_bit; // 8��Ʈ�� ����� ��Ʈ�� ���
        cout << bitset<8>(str_bit) << "|";
        str_bit = 0;
    }
    printf("\n");
    // remainBit�� file�� �������� ���
    fout << remain_bit;
    cout << "remain_bit : " << bitset<4>(remain_bit) << "\n";
    fout.close();
}



void Decoder() {
    printf("\n===================== Decoder���� =======================\n");

    ifstream fin;
    ofstream fout;
    fin.open("./encoding_text.txt", ios::in | ios::binary); //binary���� �б�
    fout.open("./HuffmanOutput.txt"); //���� ����

    // file�� ���� ���ϱ�
    fin.seekg(0, ios::end); //������ ù��ġ���� ���������� �̵�
    int file_len = fin.tellg(); //�������� ��ġ�� fileLen�� ����
    fin.seekg(0, ios::beg); //�ʱ�ȭ
    cout << "file_len : " << file_len << endl;

    // character ���� ����
    int map_count = 0; 
    map_count = fin.get(); //������ ������ �� ���ڼ� ���ϱ�
    int hash_len = 1; //���� ����

    // characterMap ����
    unordered_map<string, char> character_map;
    for (int i = 0; i < map_count; i++) {
        char character = 0;
        fin.get(character); //���� character�� ���� ���ϱ�

        hash_len += 2;
        int len = 0;
        len = fin.get(); //����ߴ� bit_len ��
        unsigned char value = 0;
        string value_str = "";
        for (int j = 0; j < len;) {
            value = fin.get(); //����ߴ� bit���� "8bit��" ��
            //cout << "(" << bitset<8>(value) << ")";
            if (len > BITSNUMBYTE) { 
                value_str += Byte_To_BitsString(value, BITSNUMBYTE); //���� ��Ʈ���� ������ 8��Ʈ�� ������
                //cout << value_str << " | ";
            }
            else {
                value_str += Byte_To_BitsString(value, len);
                //cout << value_str << " || ";
            }
            hash_len++;
            len -= BITSNUMBYTE;
        }

        character_map[value_str] = character; //character_map�� �����ϸ鼭 decoding�� �� �ֵ���
    }

    // encoding.txt �� �о� binary string�� txt�� ����
    file_len -= hash_len; //encoding_text.txt�� ������ �о����� �ľ��ϱ� ����
    cout << "file_len 2 : " << file_len << endl;
    unsigned char uc_char = 0;
    string txt = "";
    while (true) {
        uc_char = fin.get(); //����ߴ� bit ��
        cout << "ucCharacter : " << bitset<8>(uc_char) << "| file_len : " << file_len << endl;
        if (fin.fail()) break;
        if (file_len > 2) {
            txt += Byte_To_BitsString(uc_char, BITSNUMBYTE); //8��Ʈ�� txt�� string�������� ����
        }
        else {
            unsigned char remain_bit = fin.get();
            cout << "remain_bit : " << bitset<8>(remain_bit) << " -> ";
            cout << Byte_To_BitsString(uc_char, (int)remain_bit) << endl;
            txt += Byte_To_BitsString(uc_char, (int)remain_bit); //remain_bit ���� ����
        }
        file_len -= 1;
    }

    // characterMap�� binary string�� txt�� ���Ͽ� HuffmanOutput�� ���
    printf("\n");
    string tmp_txt = "";
    for (auto& i : txt) {
        tmp_txt += i; //�� ó�����ں��� 
        if (character_map.count(tmp_txt)) { //character_map�� �ִ��� Ȯ��
            fout << character_map[tmp_txt]; //�׿� �°�
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
    ifstream fin;// input file sream == ������ ���α׷��� ���� �� �ְ� ���ش�
    fin.open("./HuffmanInput.txt");// test.txt�� ����

    // unorderd_map == map���� �� ���� �ؽ����̺�� ������ �ڷᱸ��, �ߺ��� ������ ���x, �����;� �������� ����, ���絥���� ó���� ����
    unordered_map<char, int> freq_map; // �󵵼��� �������� hashmap
    unordered_map<char, string> character_map; // ���ڸ� �������� charactermap
    string txt = "";

    char character; 
    while (true) {
        fin.get(character); // get�� �̿��� character�� ���
        if (fin.fail()) break; // ��� ���ڸ� �� ������ ����
        freq_map[character]++; // hashmap�� �̿��� �ش� character�� �󵵼��� �߰�
        txt += character; // txt�� ���� character�� �־� ���������� file ���� �ű��
    }

    priority_queue<Node, vector<Node>, cmp> p; // �켱���� ť
    // hashmap�� �ִ� character���� 2���� ���ȭ �ؼ� �־��ش� -> �������� ����
    // �̷������� �������������� ť�� Ư���� pop�� ����� ���� ������������ �������ش�.
    for (auto& i : freq_map) {
        Node node(i.first, i.second); 
        p.push(node);
    }

    Make_tree(p); // ������Ʈ�� ����
    Character_mapping("", p.top(), character_map); //character�� ����

    // ��� ����?
    for (auto& i : character_map) {
        printf("%c=", i.first);
        cout << i.second << "|";
    }

    Encoder(txt, character_map);

    Decoder();

}
