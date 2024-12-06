#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <fstream>
using namespace std;

// Node structure
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Custom comparator for min-heap
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Traverse Huffman Tree and store codes in a map
void generateCodes(Node* root, string str, unordered_map<char, string>& huffmanTable) {
    if (!root) return;

    if (root->ch != '\0') { // Leaf node
        huffmanTable[root->ch] = str;
    }

    generateCodes(root->left, str + "0", huffmanTable);
    generateCodes(root->right, str + "1", huffmanTable);
}

void encode(string file, unordered_map<char, string> huffmanTable){
    ifstream f(file);
    ofstream outfile ("encoded_output.txt");

    if (!f.is_open()) {
        cerr << "Failed to open the file." << endl;
        return;
    }

    // Read the file line by line
    string line;
    while (getline(f, line)) { // Read each line from the file
        for (char c : line) {     // Iterate over each character in the line
            string code = huffmanTable[c];
            outfile << code;
        }
        outfile << endl;
    }

    outfile.close();
    f.close();
}

void decode(string file, unordered_map<char, string> huffmanTable) {
    ifstream f(file);
    ofstream outfile("decoded_output.txt");

    if (!f.is_open()) {
        cerr << "Failed to open the encoded file." << endl;
        return;
    }

    // Reverse the Huffman table
    unordered_map<string, char> reverseHuffmanTable;
    for (auto& pair : huffmanTable) {
        reverseHuffmanTable[pair.second] = pair.first;
    }

    // Read the encoded file line by line
    string line;
    while (getline(f, line)) {
        string currentCode = "";
        for (char bit : line) {
            currentCode += bit;

            // Check if the current code matches a character
            if (reverseHuffmanTable.find(currentCode) != reverseHuffmanTable.end()) {
                outfile << reverseHuffmanTable[currentCode];
                currentCode = ""; // Reset for the next character
            }
        }
        outfile << endl; // Add a newline at the end of each line
    }

    outfile.close();
    f.close();
}

void test(string og_file, string out_file) {
    ifstream original(og_file);
    ifstream decoded(out_file);

    if (!original.is_open() || !decoded.is_open()) {
        cerr << "Failed to open one or both files." << endl;
        return;
    }

    char originalChar, decodedChar;
    int totalChars = 0;
    int correctChars = 0;

    // Compare files character by character
    while (original.get(originalChar) && decoded.get(decodedChar)) {
        totalChars++;
        if (originalChar == decodedChar) {
            correctChars++;
        }
    }

    // Ensure both files are fully read
    if (original.peek() != EOF || decoded.peek() != EOF) {
        cerr << "Files have different lengths." << endl;
    }

    original.close();
    decoded.close();

    // Calculate and display percentage of correctness
    double correctness = (totalChars == 0) ? 0 : (static_cast<double>(correctChars) / totalChars) * 100.0;
    cout << "Correctness: " << correctness << "% (" << correctChars << "/" << totalChars << " characters match)" << endl;
}


int main() {
    // Open the text file
    ifstream file("test.txt");
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    // Frequency map
    unordered_map<char, int> freq;

    // Read the file line by line
    string line;
    while (getline(file, line)) { // Read each line from the file
        for (char c : line) {     // Iterate over each character in the line
            freq[c]++;            // Increment the frequency of the character
        }
    }
    file.close();

    // Convert the unordered_map to a vector of pairs for processing
    vector<pair<char, int>> freqVec(freq.begin(), freq.end());

    // Min-heap to build Huffman Tree
    priority_queue<Node*, vector<Node*>, Compare> minHeap;

    // Insert all characters into minHeap
    for (auto& p : freq) {
        minHeap.push(new Node(p.first, p.second));
        // cout << "char:" << p.first << "freq:" << p.second << endl;
    }

    // Build Huffman Tree
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;

        minHeap.push(newNode);
    }

    // Root of Huffman Tree
    Node* root = minHeap.top();

    // Generate Huffman Codes
    unordered_map<char, string> huffmanTable;
    generateCodes(root, "", huffmanTable);

    // Print Huffman Codes
    // for (auto& p : huffmanTable) {
    //     cout << p.first << ": " << p.second << endl;
    // }

    encode("test.txt",huffmanTable);
    decode("encoded_output.txt",huffmanTable);
    test("test.txt","decoded_output.txt");

    return 0;
}


