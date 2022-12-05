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

// ������ Ʈ���� ������ ���
class Node 
{
public:
    char mCharacter; // ����
    int mFreq; // �󵵼�(��)
    Node* left = NULL; // ���ʰ���
    Node* right = NULL; // �����ʰ���

    // ��� ����
    Node(char character, int value, Node* nLeft = NULL, Node* nRight = NULL) {
        mCharacter = character;
        mFreq = value;
        left = nLeft;
        right = nRight;
    }
};


// �켱���� ť�� ����ϱ� ���� ���Լ�
struct cmp 
{
    // �������� �����
    bool operator()(Node& a, Node& b) {
        return a.mFreq > b.mFreq;
    }
};


// ********************
// @ makeTree
// @ �켱���� ť(priority_queue)�� �̿��� tree ����
// @ 1. �ּ� �󵵼� ��� �� �� ����, �켱���� ť���� pop
// @ 2. �ּ� �󵵼� ��� �� ���� ������ ��带 �ϳ� �����, �� ���� ���� �ڽ����� ����
// @ 3. ���� ��带 �켱���� ť�� push
// ********************
/*
* �켱����ť
* priority_queue<�ڷ���, ����ü, ���Լ�>
*/
void makeTree(priority_queue<Node, vector<Node>, cmp>& p) 
{
    while (true) {
        // size_t == unsigned int == ��ȣ���� 32(or 64)bit ����, �޸�/���ڿ� ���� ���Ҷ� ���
        size_t size = p.size(); // ����� ����
        if (size < 2) break; // ����� ������ 2�� �̻��̿��� ����

        // p.top�� ���� ���� mValue���� ������ �̿� -> ��带 ������ pop
        Node* firstMinNode = new Node(p.top().mCharacter, p.top().mFreq, p.top().left, p.top().right);
        //printf("%c |", p.top().mCharacter);
        p.pop();
        Node* secondMinNode = new Node(p.top().mCharacter, p.top().mFreq, p.top().left, p.top().right);
        //printf("%c ||", p.top().mCharacter);
        p.pop();
        int newValue = firstMinNode->mFreq + secondMinNode->mFreq; // ���� ���� �θ����� mValue�� �����
        
        Node* newNode = new Node(NULL, newValue); // �� ����� �θ��带 ����
        newNode->left = firstMinNode; // �ڽĳ��� ����
        newNode->right = secondMinNode;
        p.push(*newNode); // �켱���� ť�� �־��ֱ�
    }
}


// ********************
// @ characterMapping
// @ mapping char-string. using unordered_map<char, string> characterMap
// @ tree(node)�� �޾Ƽ� tree�� ��ȸ�ϸ鼭 mapping ����
// ********************
void characterMapping(string str, Node node, unordered_map<char, string>& characterMap) 
{
    // �� ó�� str�� ����
    if (node.mCharacter != NULL) // mCharacter�� �ִ� == ������ ����̴�(�θ��带 ���鶧 mcharacter�� Null)
    {
        characterMap[node.mCharacter] = str; // �̵��ϸ鼭 ������� �ڵ带 ����
        //cout << node.mCharacter << endl;
        //cout << str << endl;
        return; // ����Լ� ����
    }

    str += "0"; // ���� ���� ���� 0��
    characterMapping(str, *node.left, characterMap);
    str[str.size() - 1] = '1'; // ������ ���� ���� 1��
    characterMapping(str, *node.right, characterMap);
}


// ********************
// @ byteToBitsString
// @ 1 byte�� �Է����� �޾�, ���� len ��ŭ binary string�� ����� ��ȯ
// ********************
string byteToBitsString(unsigned char oneByte, int len) {
    string bitsString = ""; 
    // binary string ����� ��ȯ
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
// @ mapping�� characterMap, HuffmanInput.txt���� ���� string txt�� ������
// @ txt���� �ѱ��ھ� �о� characterMap�� ��.
// @ �� ���ڿ� �ش��ϴ� binary�� encoding_text.txt�� write
// ********************
void encoder(const string& txt, unordered_map<char, string>& characterMap) {
    ofstream fout; //���Ͽ� ����
    string encodedTxt = ""; //���ڵ��� ������ ����

    printf("===================== encoder���� =======================\n");
    fout.open("./encoding_text.txt", ios::out | ios::binary); // ������ ���� ����(write���� type binary ���� open)

    // mapping�� characterMap ���
    size_t size = characterMap.size(); // �����ϴ� ���� �� ���ϱ�
    unsigned char ucSize = size; 
    fout << ucSize; // ���ڰ� � �ִ��� write
    printf("���� ���� = %d\n", ucSize);

    // key, value bits len, value bits ���
    unsigned char uc = 0;
    //encoding ����
    for (auto& i : characterMap) {
        uc = i.first; //characterMap�� ù��°����
        fout << uc;  // �ش� ���ڸ� write
        //printf("���� %c / ", uc);
        unsigned char len = i.second.size();
        fout << len;  // value bits ���� ���
        
        //printf("%d", i.second.size());

        // value bits ���ϱ�
        unsigned char summ = 0; 
        for (size_t j = 0; j < len;) { 
            //BITSNUMBYTE = 8
            for (int k = 0; k < BITSNUMBYTE; k++, j++) {
                if (j > len - 1) break; //������ ���̸� �ʰ��ϱ��� �ݺ� ����
                summ = (summ << 1) | (i.second[j] & 1); // value bits ��� ����
            }
            fout << summ; // value bits ���
            printf("summ = %d\n", summ);
            summ = 0;
        }
    }

    // ������ ������� encoding�� binary encodedTxt �����
    size = txt.size(); 
    for (size_t i = 0; i < size; i++) {
        char currentCharacter = txt[i]; //������ txt������ �ϳ��ϳ� ��
        encodedTxt += characterMap[currentCharacter]; //�ش� ���ڸ� encodedTxt�� �߰�(��Ʈ)   
    }

    // encodedTxt�� 8bits�� 1byte�� ���
    unsigned char remainBit = 0; // bit ������ 8�� ����� �ƴ� ���, ������ bitó��
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

    // remainBit�� file�� �������� ���
    fout << remainBit;
    cout << "reaminBit : " << bitset<4>(remainBit) << "\n";
    fout.close();
}


// ********************
// @ decoder
// @ encoding_text.txt�� �ҷ���
// @ encoding_text.txt���� �� bit�� �о� �� �κ��� map �а�, map ����. 
// @ map ���ĺ��� encoding data ����.
// @ �������� 8bits�� �� ���� ������ �� ����. ���� bits��ŭ �б�.
// ********************
void decoder() {
    ifstream fin;
    ofstream fout;
    fin.open("./encoding_text.txt", ios::in | ios::binary); //binary���� �б�
    fout.open("./HuffmanOutput.txt"); //���� ����

    // file�� ���� ���ϱ�
    fin.seekg(0, ios::end); //������ ù��ġ���� ���������� �̵�
    int fileLen = fin.tellg(); //�������� ��ġ�� fileLen�� ����
    fin.seekg(0, ios::beg); //�ʱ�ȭ

    // character ���� ����
    int mapCount = 0; 
    mapCount = fin.get(); //������ ������ �� ���ڼ� ���ϱ�
    int hashLen = 1;

    // characterMap ����
    unordered_map<string, char> characterMap;
    for (int i = 0; i < mapCount; i++) {
        char character = 0;
        fin.get(character); //���� character�� ���� ���ϱ�

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

    // encoding.txt �� �о� binary string�� txt�� ����
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

    // characterMap�� binary string�� txt�� ���Ͽ� HuffmanOutput�� ���
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
    
    ifstream fin;// input file sream == ������ ���α׷��� ���� �� �ְ� ���ش�
    fin.open("./HuffmanInput.txt");// test.txt�� ����

    // unorderd_map == map���� �� ���� �ؽ����̺�� ������ �ڷᱸ��, �ߺ��� ������ ���x, �����;� �������� ����, ���絥���� ó���� ����
    unordered_map<char, int> freqMap; // �󵵼��� �������� hashmap
    unordered_map<char, string> characterMap; // ���ڸ� �������� charactermap
    string txt = "";

    char character; 
    while (true) {
        fin.get(character); // get�� �̿��� character�� ���
        if (fin.fail()) break; // ��� ���ڸ� �� ������ ����
        freqMap[character]++; // hashmap�� �̿��� �ش� character�� �󵵼��� �߰�
        txt += character; // txt�� ���� character�� �־� ���������� file ���� �ű��
    }

    priority_queue<Node, vector<Node>, cmp> p; // �켱���� ť
    // hashmap�� �ִ� character���� 2���� ���ȭ �ؼ� �־��ش� -> �������� ����
    // �̷������� �������������� ť�� Ư���� pop�� ����� ���� ������������ �������ش�.
    for (auto& i : freqMap) {
        Node node(i.first, i.second); 
        p.push(node);
    }
    printf("\n");

    makeTree(p); // ������Ʈ�� ����
    characterMapping("", p.top(), characterMap); //character�� ����

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
