#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stack>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

class stackDataStructue
{
public:
    string operation;
    int linNumber;
    string line;
    int wordNumber;
    stackDataStructue() {}

    stackDataStructue(string o, int ln, string s, int wn)
    {
        this->operation = o;
        this->linNumber = ln;
        this->line = s;
        this->wordNumber = wn;
    }
};

class buffer
{
public:
    vector<string> words;
    buffer *next;
    buffer *prev;
    buffer(const string &line)
    {
        stringstream ss(line);
        string word;

        while (ss >> word)
        {
            words.push_back(word);
        }
        this->prev = nullptr;
        this->next = nullptr;
    }
};

class line_editor
{
private:
    buffer *head = nullptr, *tail = nullptr, *cursorAddress;
    int cursorPosition;
    stack<stackDataStructue> undo;
    stack<stackDataStructue> redo;

public:
    // void undo_redo_Menu(string choice, int lineNumber, string line, int wordNumber)
    // {
    // }

    string findLineByNumber(int linNumber)
    {
        string str = "";
        buffer *temp = head;
        int i = 1;
        while (i != linNumber && temp != nullptr)
        {
            temp = temp->next;
            i += 1;
        }
        if (i == linNumber && temp != nullptr)
        {
            str = reconstruct(temp);
        }
        else
        {
            cout << "Can't perform undo redo operation" << endl;
        }
        return str;
    }

    string findWordByNumber(int lineNumber, int wordNumber)
    {
        int i = 1;
        buffer *temp = head;
        string str = "";
        while (i != lineNumber && temp != nullptr)
        {
            temp = temp->next;
            i += 1;
        }
        if (temp == nullptr)
        {
            cout << "Can't perform undo redo operation" << endl;
        }
        else
        {
            str = temp->words[wordNumber - 1];
        }
        return str;
    }

    void runMenu()
    {
        int choice;
        do
        {
            cout << endl;
            cout << "\n-------------Line Editor----------------" << endl;
            cout << "1.Save the Fie" << endl;
            cout << "2.Move the cursor position" << endl;
            cout << "3.Insert a new line" << endl;
            cout << "4.Delete a Line" << endl;
            cout << "5.Update a Line" << endl;
            cout << "6.Read Buffer" << endl;
            cout << "7.Insert a word" << endl;
            cout << "8.delete a word" << endl;
            cout << "9.update a word" << endl;
            cout << "10.Search a word" << endl;
            cout << "11.Undo" << endl;
            cout << "12.Redo" << endl;
            cout << "13.Exit" << endl;
            cout << "Enter your choice : ";
            cin >> choice;
            switch (choice)
            {
            case 1:
                handle_saveFile();
                break;
            case 2:
                handle_moveCursor();
                break;
            case 3:
                handle_insertLine();
                break;
            case 4:
                handle_deleteLine();
                break;
            case 5:
                updateLine();
                break;
            case 6:
                readBuffer();
                break;
            case 7:
                insertWord();
                break;
            case 8:
                deleteWord();
                break;
            case 9:
                updateWord();
                break;
            case 10:
                searchWord();
                break;
            case 11:
                undo_Operation();
                break;
            case 12:
                redo_Operation();
                break;
            }
        } while (choice >= 1 && choice <= 12);
    }

    string reconstruct(buffer *node)
    {
        if (!node)
        {
            return "";
        }

        std::ostringstream oss;
        for (size_t i = 0; i < node->words.size(); ++i)
        {
            oss << node->words[i];
            if (i < node->words.size() - 1)
            {
                oss << " ";
            }
        }
        return oss.str();
    }

    void undo_Operation()
    {
        if (undo.empty())
        {
            cout << "Can't perform undo operation" << endl;
            return;
        }
        stackDataStructue temp = undo.top();
        undo.pop();
        string choice = temp.operation;
        int lineNumber = temp.linNumber;
        string line = temp.line;
        int wordNumber = temp.wordNumber;

        if (choice == "insertLine")
        {
            string str = findLineByNumber(lineNumber);
            redo.push(stackDataStructue("deleteLine", lineNumber, str, wordNumber));
            deleteAtSpecificPosition(lineNumber);
        }
        else if (choice == "deleteLine")
        {
            redo.push(stackDataStructue("insertLine", lineNumber, line, wordNumber));
            insertLine(line, lineNumber);
        }
        else if (choice == "updateLine")
        {
            string str = findLineByNumber(lineNumber);
            redo.push(stackDataStructue("updateLine", lineNumber, str, wordNumber));
            updateAtSpecificPosition(lineNumber, line);
        }
        else if (choice == "insertWord")
        {
            string str = findWordByNumber(lineNumber, wordNumber);
            redo.push(stackDataStructue("deleteWord", lineNumber, str, wordNumber));
            deleteWordAtSpecificPosition(lineNumber, wordNumber);
        }
        else if (choice == "deleteWord")
        {
            redo.push(stackDataStructue("insertWord", lineNumber, line, wordNumber));
            insertWordAtSpecificPosition(lineNumber, wordNumber, line);
        }
        else if (choice == "updateWord")
        {
            string str = findWordByNumber(lineNumber, wordNumber);
            redo.push(stackDataStructue("updateWord", lineNumber, str, wordNumber));
            updateWordAtSpecificPosition(lineNumber, wordNumber, line);
        }
        undo.pop();
    }

    void redo_Operation()
    {
        if (redo.empty())
        {
            cout << "Can't perform redo operation" << endl;
            return;
        }
        stackDataStructue temp = redo.top();
        redo.pop();
        string choice = temp.operation;
        int lineNumber = temp.linNumber;
        string line = temp.line;
        int wordNumber = temp.wordNumber;

        if (choice == "insertLine")
        {
            string str = findLineByNumber(lineNumber);
            undo.push(stackDataStructue("deleteLine", lineNumber, str, wordNumber));
            deleteAtSpecificPosition(lineNumber);
        }
        else if (choice == "deleteLine")
        {
            redo.push(stackDataStructue("insertLine", lineNumber, line, wordNumber));
            insertLine(line, lineNumber);
        }
        else if (choice == "updateLine")
        {
            string str = findLineByNumber(lineNumber);
            redo.push(stackDataStructue("updateLine", lineNumber, str, wordNumber));
            updateAtSpecificPosition(lineNumber, line);
        }
        else if (choice == "insertWord")
        {
            string str = findWordByNumber(lineNumber, wordNumber);
            undo.push(stackDataStructue("deleteWord", lineNumber, str, wordNumber));
            deleteWordAtSpecificPosition(lineNumber, wordNumber);
        }
        else if (choice == "deleteWord")
        {
            redo.push(stackDataStructue("insertWord", lineNumber, line, wordNumber));
            insertWordAtSpecificPosition(lineNumber, wordNumber, line);
        }
        else if (choice == "updateWord")
        {
            string str = findWordByNumber(lineNumber, wordNumber);
            undo.push(stackDataStructue("updateWord", lineNumber, str, wordNumber));
            updateWordAtSpecificPosition(lineNumber, wordNumber, line);
        }
        undo.pop();
    }

    void append(string line)
    {
        buffer *n = new buffer(line);
        if (head == nullptr)
        {
            head = n;
            tail = n;
            cursorAddress = head;
            cursorPosition = 1;
        }
        else
        {
            n->prev = tail;
            tail->next = n;
            tail = n;
        }
    }

    bool loadFile(fstream &file)
    {
        if (!file.is_open())
        {
            cout << "Error while opening the file " << endl;
            return false;
        }

        string line;
        int i = 0;
        while (getline(file, line))
        {
            i += 1;
            append(line);
        }
        cout << "File opened successfully and " << i << " lines inserted in buffer" << endl;
        file.close();
        return true;
    }

    void handle_moveCursor()
    {
        int n;
        cout << "Enter the position where you want to move the cursor: ";
        cin >> n;
        if (n > cursorPosition)
        {
            while (cursorPosition != n)
            {
                cursorAddress = cursorAddress->next;
                cursorPosition += 1;
            }
        }
        else if (n < cursorPosition)
        {
            while (cursorPosition != n)
            {
                cursorAddress = cursorAddress->prev;
                cursorPosition -= 1;
            }
        }
        if (cursorAddress == nullptr)
        {
            cursorAddress = head;
            cursorPosition = 1;
        }
        cout << "Cursor Position changed to: " << cursorPosition << endl;
        string str = reconstruct(cursorAddress);
        cout << str << endl;
    }

    bool handle_loadFile()
    {
        string fileName;
        cout << "Enter the file you want to work with: ";
        cin >> fileName;
        // loadFlie(fileName);
    }

    void saveFile(string fileName)
    {
        buffer *temp = head, *t;
        ofstream file(fileName);
        if (!file.is_open())
        {
            cout << "Error Saving file" << fileName << endl;
            return;
        }
        while (temp != nullptr)
        {
            string str = reconstruct(temp);
            file << str << endl;
            temp->words.clear();
            t = temp;
            temp = temp->next;
            free(t);
        }
        while (!undo.empty())
        {
            undo.pop();
        }
        while (!redo.empty())
        {
            redo.pop();
        }
        file.close();
    }

    void handle_saveFile()
    {
        string fileName;
        cout << "Enter the file name you want to save: ";
        cin.ignore();
        getline(cin, fileName);
        saveFile(fileName);
    }

    void insertLine(string &line, int pos)
    {
        undo.push(stackDataStructue("insertLine", pos, line, 0));
        buffer *n = new buffer(line);
        if (head == nullptr)
        {
            head = n;
            tail = n;
            cursorAddress = n;
            return;
        }
        buffer *temp = head;
        int i = 1;
        while (i != pos && temp != nullptr)
        {
            i += 1;
            temp = temp->next;
        }
        if (temp == nullptr && i == pos)
        {
            n->prev = tail;
            tail->next = n;
            tail = n;
        }
        else if (temp == head)
        {
            n->next = head;
            head = n;
            if (cursorAddress == head)
                cursorAddress = n;
        }
        else
        {
            if (cursorAddress == temp)
                cursorAddress = n;
            n->next = temp;
            n->prev = temp->prev;
            temp->prev->next = n;
            temp->prev = n;
        }
    }

    void handle_insertLine()
    {
        string line;
        int pos;
        cout << "Enter the line you want to insert: ";
        cin.ignore();
        getline(cin, line);
        cout << "Enter the position you want to insert line: ";
        cin >> pos;
        insertLine(line, pos);
    }

    void undoDelete(int pos, buffer *temp)
    {
        string s = reconstruct(temp);
        undo.push(stackDataStructue("deleteLine", pos, s, 0));
    }

    void deleteAtSpecificPosition(int pos)
    {
        int i = 1;
        buffer *temp = head;
        while (i != pos && temp != nullptr)
        {
            i += 1;
            temp = temp->next;
        }
        if (temp == head)
        {
            undoDelete(pos, temp);
            head = head->next;
            head->prev = nullptr;
            if (cursorAddress == temp)
                cursorAddress = head;
            free(temp);
        }
        else if (temp->next == NULL && i == pos)
        {
            undoDelete(pos, temp);
            temp = tail;
            tail = tail->prev;
            tail->next = nullptr;
            if (cursorAddress == temp)
                cursorAddress = tail;
            temp->words.clear();
            free(temp);
        }
        else if (i == pos)
        {
            undoDelete(pos, temp);
            if (temp == cursorAddress)
                cursorAddress = temp->next;
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
            cursorAddress = temp->next;
            temp->words.clear();
            free(temp);
        }
        else
            cout << "That line does not exist" << endl;
    }

    void deleteAtCursorPosition()
    {
        buffer *temp = head;
        if (cursorAddress == nullptr)
        {
            cout << "Buffer is empty Can't delete anything" << endl;
            return;
        }
        int i = 1;
        while (temp != cursorAddress)
        {
            temp = temp->next;
            i += 1;
        }
        undoDelete(i, temp);

        if (cursorAddress == head && cursorAddress == tail)
        {
            cursorAddress = nullptr;
            temp = head;
            head = nullptr;
            tail = nullptr;
            temp->words.clear();
            free(temp);
        }
        else if (cursorAddress == head)
        {
            temp = head;
            head = head->next;
            head->prev = nullptr;
            cursorAddress = head;
            temp->words.clear();
            free(temp);
        }
        else if (cursorAddress == tail)
        {
            temp = tail;
            tail = tail->prev;
            tail->next = nullptr;
            cursorAddress = tail;
            temp->words.clear();
            free(temp);
        }
        else
        {
            temp = cursorAddress;
            cursorAddress->prev->next = cursorAddress->next;
            cursorAddress->next->prev = cursorAddress->prev;
            cursorAddress = cursorAddress->next;
            temp->words.clear();
            free(temp);
            cout << "Cursor is moved one line ahead" << endl;
        }
    }

    void handle_deleteLine()
    {
        if (head == nullptr)
        {
            cout << "Can't perform delete operation" << endl;
            return;
        }
        int ch;
        cout << "Do you want to delete at cursor position or any specific position: " << endl;
        cout << "1. At cursor position" << endl;
        cout << "2. Want to give my own position" << endl;
        cout << "Enter your choice: ";
        cin >> ch;
        if (ch != 1 && ch != 2)
        {
            cout << "Wrong option choosed" << endl;
            return;
        }
        else if (ch == 1)
            deleteAtCursorPosition();
        else
        {
            int pos;
            cout << "Enter the position of line you want to delete: ";
            cin >> pos;
            deleteAtSpecificPosition(pos);
        }
    }

    void updateAtCursorPosition()
    {
        string line;
        buffer *temp = head;
        cout << "Enter the changed string: ";
        cin.ignore();
        getline(cin, line);
        buffer *n = new buffer(line);
        int i = 1;
        while (temp != cursorAddress)
        {
            temp = temp->next;
            i += 1;
        }
        undoUpdate(i, cursorAddress);
        if (head == tail)
        {
            temp = head;
            head = n;
            cursorAddress = n;
            free(temp);
        }
        else if (cursorAddress == head)
        {
            temp = head;
            n->next = head->next;
            head->next->prev = n;
            head = n;
            cursorAddress = n;
            free(temp);
        }
        else if (cursorAddress == tail)
        {
            temp = tail;
            n->prev = tail->prev;
            tail->prev->next = n;
            tail = n;
            cursorAddress = n;
            free(temp);
        }
        else
        {
            n->next = cursorAddress->next;
            n->prev = cursorAddress->prev;
            cursorAddress->next->prev = n;
            cursorAddress->prev->next = n;
            temp = cursorAddress;
            cursorAddress = n;
            free(temp);
        }
    }

    void undoUpdate(int pos, buffer *temp)
    {
        string s = reconstruct(temp);
        undo.push(stackDataStructue("updateLine", pos, s, 0));
    }

    void updateAtSpecificPosition(int pos, string line)
    {
        buffer *temp = head;
        int n = 1;
        while (n != pos && temp != nullptr)
        {
            n += 1;
            temp = temp->next;
        }

        if (temp == head)
        {
            undoUpdate(pos, temp);
            buffer *n = new buffer(line);
            n->next = temp->next;
            if (temp->next)
                temp->next->prev = n;
            if (cursorAddress == head)
                cursorAddress = n;
            head = n;
            free(temp);
        }
        else if (temp == tail)
        {
            undoUpdate(pos, temp);
            buffer *n = new buffer(line);
            n->prev = temp->prev;
            if (temp->prev)
                temp->prev->next = n;
            tail = n;
            free(temp);
        }
        else if (n == pos && temp != nullptr)
        {
            undoUpdate(pos, temp);
            buffer *n = new buffer(line);
            n->prev = temp->prev;
            n->next = temp->next;
            temp->prev->next = n;
            temp->next->prev = n;
            free(temp);
        }
        else
        {
            cout << "Can't update the line";
            return;
        }
        cout << "Hello";
    }

    void updateLine()
    {
        if (head == nullptr)
        {
            cout << "Can't perform delete operation" << endl;
            return;
        }
        int n;
        cout << "Do you want to change in particular position or cursor position: " << endl;
        cout << "1.Cursor Position" << endl;
        cout << "2.Particular Position" << endl;
        cout << "Enter your choice: ";
        cin >> n;
        if (n != 1 && n != 2)
        {
            cout << "Wrong option choosed" << endl;
            return;
        }
        else if (n == 1)
            updateAtCursorPosition();
        else
        {
            int pos;
            string line;
            cout << "Enter the position of the line you want to update: ";
            cin >> pos;
            cout << "Enter the updated string: ";
            cin.ignore();
            getline(cin, line);
            updateAtSpecificPosition(pos, line);
        }
    }

    void readBuffer()
    {
        int n;
        cout << "Where you want to read" << endl;
        cout << "1.At cursor position" << endl;
        cout << "2.Full Buffer" << endl;
        cout << "Enter your choice: ";
        cin >> n;
        if (n != 1 && n != 2)
        {
            cout << "Wrong option choosed" << endl;
            return;
        }
        else if (n == 1)
            readBufferAtCursorPosition();
        else
            readFullBuffer();
    }

    void readFullBuffer()
    {
        buffer *temp = head;
        int i = 1;
        if (head == nullptr)
        {
            cout << "Buffer is empty please insert some lines first" << endl;
            return;
        }
        while (temp != nullptr)
        {
            string str = reconstruct(temp);
            cout << i << ". " << str << endl;
            i += 1;
            temp = temp->next;
        }
    }

    void readBufferAtCursorPosition()
    {
        if (cursorAddress == nullptr)
        {
            cout << "CursorAddress is NULL please first move the cursor" << endl;
            return;
        }
        string str = reconstruct(cursorAddress);
        cout << str << endl;
    }

    void updateWordAtCursorPosition()
    {
        int n;
        string str;
        cout << "Enter the number of word you want to update: ";
        cin >> n;
        buffer *temp = head;
        int i = 1;
        while (temp != cursorAddress)
        {
            temp = temp->next;
            i += 1;
        }
        cout << "Enter the updated string: ";
        cin.ignore();
        getline(cin, str);
        if ((n - 1) < cursorAddress->words.size())
        {
            undoUpdateWord(i, n, cursorAddress->words[n - 1]);
            cursorAddress->words[n - 1] = str;
        }
        else
            cout << "the word number is not present in the string";
    }

    void updateWordAtSpecificPosition(int lineNumber, int wordNumber, string str)
    {

        int i = 1;
        buffer *temp = head;
        while (i != lineNumber && temp != nullptr)
        {
            i += 1;
            temp = temp->next;
        }
        if (temp == nullptr && i != lineNumber)
        {
            cout << "The line does not exist" << endl;
            return;
        }

        if (temp != nullptr && wordNumber > temp->words.size())
        {
            cout << "This Word does not exist in line" << endl;
            return;
        }
        undoUpdateWord(lineNumber, wordNumber, temp->words[wordNumber - 1]);
        temp->words[wordNumber - 1] = str;
    }

    void insertWordAtCursorPosition()
    {
        int n;
        cout << "Enter the position where you want to insert a word: ";
        cin >> n;
        if (n > cursorAddress->words.size() + 1)
        {
            cout << "Can't insert word at that position" << endl;
            return;
        }
        buffer *temp = head;
        int i = 1;
        while (temp != cursorAddress)
        {
            i += 1;
            temp = temp->next;
        }
        undoInsertWord(i, n);
        string word;
        cout << "Enter the new word you want to add: ";
        cin.ignore();
        getline(cin, word);
        cursorAddress->words.insert(cursorAddress->words.begin() + n - 1, word);
    }

    void undoInsertWord(int lineNumber, int wordNumber)
    {
        undo.push(stackDataStructue("insertWord", lineNumber, "", wordNumber));
    }

    void insertWordAtSpecificPosition(int lineNumber, int wordNumber, string word)
    {
        buffer *temp = head;
        int i = 1;
        while (i != lineNumber && temp != nullptr)
        {
            i += 1;
            temp = temp->next;
        }
        if (temp == nullptr)
        {
            undoInsertWord(lineNumber, wordNumber);
            buffer *n = new buffer(word);
            n->prev = tail;
            tail->next = n;
            tail = n;
            return;
        }
        else if (i == lineNumber)
        {
            undoInsertWord(lineNumber, wordNumber);
            temp->words.insert(temp->words.begin() + wordNumber - 1, word);
        }
        else
        {
            cout << "Can't insert the word" << endl;
        }
    }

    void insertWord()
    {
        string a;
        cout << "Do You want to insert in cursor point or specific line:" << endl;
        cout << "1.At cursor position" << endl;
        cout << "2.At specific position" << endl;
        cout << "Enter your choice: ";
        cin >> a;
        if (int(a[0]) != 49 && int(a[0]) != 50)
        {
            cout << "Wrong option choosed" << endl;
            return;
        }
        else if (int(a[0]) == 1)
            insertWordAtCursorPosition();
        else
        {
            int lineNumber, wordNumber;
            string word;
            cout << "Enter the line number where you want to insert a word: ";
            cin >> lineNumber;
            cout << "Enter the word number where you want to insert a word: ";
            cin >> wordNumber;
            cout << "Enter the word: ";
            cin.ignore();
            getline(cin, word);
            insertWordAtSpecificPosition(lineNumber, wordNumber, word);
        }
    }

    void deleteWord()
    {
        if (head == nullptr)
        {
            cout << "Can't perform delete operation" << endl;
            return;
        }
        int n;
        cout << "Do You want to delete in cursor point or specific line:" << endl;
        cout << "1.At cursor position" << endl;
        cout << "2.At specific position" << endl;
        cout << "Enter your choice: ";
        cin >> n;
        if (n != 1 && n != 2)
        {
            cout << "Wrong option choosed" << endl;
            return;
        }
        else if (n == 1)
            deleteWordAtCursorPosition();
        else
        {
            int lineNumber, wordNumber;
            cout << "Enter the line number where you want to delete a word: ";
            cin >> lineNumber;
            cout << "Enter the word number which you want to delete: ";
            cin >> wordNumber;
            deleteWordAtSpecificPosition(lineNumber, wordNumber);
        }
    }

    void deleteWordAtCursorPosition()
    {
        int n;
        cout << "Enter the word number you want to delete: ";
        cin >> n;
        if (cursorAddress->words.size() < n)
        {
            cout << "Cannot delete the word" << endl;
            return;
        }
        buffer *temp = head;
        int i = 1;
        while (temp != cursorAddress)
        {
            i += 1;
            temp = temp->next;
        }
        undoDeleteWord(i, n, temp->words[n - 1]);
        cursorAddress->words.erase(cursorAddress->words.begin() + n - 1);
        cout << "Element is deleted" << endl;
    }

    void undoDeleteWord(int lineNumber, int wordNumber, string word)
    {
        undo.push(stackDataStructue("deleteWord", lineNumber, word, wordNumber));
    }

    void deleteWordAtSpecificPosition(int lineNumber, int wordNumber)
    {

        buffer *temp = head;
        int i = 1;
        while (i != lineNumber && temp != nullptr)
        {
            i += 1;
            temp = temp->next;
        }
        if (temp == nullptr && i != lineNumber)
        {
            cout << "Line Number does not exist";
            return;
        }
        if (temp->words.size() < wordNumber)
        {
            cout << "Can't delete the word" << endl;
            return;
        }
        undoDeleteWord(lineNumber, wordNumber, temp->words[wordNumber - 1]);
        temp->words.erase(temp->words.begin() + wordNumber - 1);
        cout << "Word Deleted successfully";
    }

    void undoUpdateWord(int lineNumber, int wordNumber, string str)
    {
        undo.push(stackDataStructue("updateWord", lineNumber, str, wordNumber));
    }

    void updateWord()
    {
        if (head == nullptr)
        {
            cout << "Can't perform delete operation" << endl;
            return;
        }
        int n;
        cout << "Do you want to update in cursor point or another specific line:" << endl;
        cout << "1.At cursor position" << endl;
        cout << "2.At specific position" << endl;
        cout << "Enter your choice: ";
        cin >> n;
        if (n != 1 && n != 2)
        {
            cout << "Wrong option choosed" << endl;
            return;
        }
        else if (n == 1)
            updateWordAtCursorPosition();
        else
        {
            int lineNumber, wordNumber;
            cout << "Enter the line number in which you want to change word: ";
            cin >> lineNumber;
            cout << "Enter the word number you want to change: ";
            cin >> wordNumber;
            string str;
            cout << "Enter the updated word: ";
            cin.ignore();
            getline(cin, str);
            updateWordAtSpecificPosition(lineNumber, wordNumber, str);
        }
    }

    void searchWord()
    {
        string word;
        cout << "Enter the word you want to search in file: ";
        cin.ignore();
        getline(cin, word);
        buffer *temp = head;
        int i = 1;
        while (temp != nullptr)
        {
            auto it = find(temp->words.begin(), temp->words.end(), word);
            if (it != temp->words.end())
            {
                cout << "Word Fount in line number: " << i << " and word number: " << distance(temp->words.begin(), it) + 1 << endl;
                break;
            }
            i += 1;
            temp = temp->next;
        }
    }

    bool exist(string &path)
    {
        DIR *dir = opendir(path.c_str());
        if (dir)
        {
            closedir(dir); // Directory exists
            return true;
        }
        else
        {
            return false; // Directory does not exist
        }
    }

    void handleArgument(int argc, char *argv[])
    {
        if (argc == 1)
        {
            fstream file;
            file.open("file.txt", ios::in | ios::out | ios::app);
            loadFile(file);
        }
        else if (argc == 2)
        {
            string fileName = argv[1];
            fstream file;
            file.open(fileName, ios::in | ios::out);
            if (!file.is_open())
            {
                cout << "Unable to open file";
                return;
            }
            loadFile(file);
        }
        else if (argc == 3)
        {
            string fileName = argv[1];
            string directoryName = argv[2];
            string filePath = directoryName + "/" + fileName;
            if (!exist(directoryName))
            {
                cout << "Directory does not exist" << endl;
                return;
            }
            fstream file;
            file.open(filePath, ios::in | ios::out | ios::app);
            if (!file.is_open())
            {
                cout << "Can't open file" << endl;
                return;
            }
            loadFile(file);
        }
        else
        {
            cout << "To many arguments" << endl;
            return;
        }
        runMenu();
    }
};

int main(int argc, char *argv[])
{
    line_editor editor;
    editor.handleArgument(argc, argv);
    return 0;
}