#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#define SHOW_MSG_LENGTH 8
#define SHOW_MSG_COMMAND_LENGTH 10
#define MSG_BEGIN_INDEX 2
#define SERNDER_ID_INDEX 0
#define RECEIVER_ID_INDEX 2
#define MSG_INDEX 4
#define SHOW_MSG_ID_INDEX 9
#define ZERO_ASCII_CODE 48

using namespace std;

struct Message{
    int sender_id;
    string message;
};

struct Person{
    int id;
    vector<int> friends;
    vector<Message> msgs;
};

void read_connections_from_file(vector<Person>& people, string file_name){
    string line;
    ifstream my_file (file_name);
    if(my_file){
        while ( getline (my_file,line) ){
            Person person;
            person.id = line[0] - ZERO_ASCII_CODE;
            for(int j=2; j<line.size(); j+=2)
                person.friends.push_back(line[j]- ZERO_ASCII_CODE);
            people.push_back(person);
        }
        my_file.close();
    }
}

void read_messages_from_inbox(vector<Person>& people){
    for(int i = 0; i < people.size(); i++){
        string line;
        string file_name = "Person" + to_string(people[i].id) + ".txt";
        ifstream my_file(file_name);
        if (my_file){
            while ( getline (my_file,line) ){
                Message msg;
                msg.sender_id = line[0] - ZERO_ASCII_CODE;
                msg.message = line.substr(MSG_BEGIN_INDEX);
                people[i].msgs.push_back(msg);
            }
            my_file.close();
        }
    }
}

void read_informations(vector<Person>& people, string file_name){
    read_connections_from_file(people, file_name);
    read_messages_from_inbox(people);
}

bool is_there_id(vector<Person>& people, int id){
    int counter = 0;

    for(int i = 0; i < people.size(); i++)
        if(people[i].id == id)
            counter++;

    if(counter != 0)
        return true;
    else 
        return false;
}

bool is_there_friend(vector<Person>& people, int id, int friend_id){
    int counter = 0;

    for(int i = 0; i < people[id].friends.size(); i++)
        if(people[id].friends[i] == friend_id)
            counter++;
    
    if(counter != 0)
        return true;
    else
        return false;
}

bool is_valid(vector<Person>& people, int sender_id, int receiver_id){
    if( (is_there_id(people, sender_id) ) && 
    (is_there_id(people, receiver_id) ) && 
     (is_there_friend(people,sender_id, receiver_id) ) )
        return true;
    else
        return false;
}

void set_new_friend(vector<Person>& people,int id, int new_friend_id){
    for(int i=0; i< people.size(); i++)
        if(people[i].id == id)
            people[i].friends.push_back(new_friend_id);
}

void set_friends(vector<Person>& people){
    for(int i=0; i < people.size(); i++)
        for(int j=0; j < people[i].friends.size(); j++)
            if(!is_there_friend(people,people[i].friends[j],people[i].id))
                set_new_friend(people,people[i].friends[j],people[i].id);
}

void remove_extra_spaces(string & msg){
    int extra_spaces = 0;
    for(int i = msg.size() - 1; i > 0; i--) { 
        if(msg[i] == ' ')
            extra_spaces++;
        else
            break;
    }
    msg = msg.substr(0, msg.size() - extra_spaces); 
}  

void write_message_to_file(int sender_id, int receiver_id, string message){
     ofstream my_file;
     string file_name = "Person" + to_string(receiver_id) + ".txt";
     my_file.open(file_name, ofstream::app);
     my_file << sender_id << ' ' << message << endl;
     my_file.close();
}

void save_new_message(vector<Person>& people, int sender_id, 
 int receiver_id, string message){
    Message msg;
    msg.sender_id = sender_id;
    msg.message = message;

    for(int i = 0; i < people.size(); i++)
        if(people[i].id == receiver_id)
            people[i].msgs.push_back(msg);

    write_message_to_file(sender_id, receiver_id, message);
}

void send_message(vector<Person>& people, int sender_id,
 int receiver_id, string & message){
    if( is_valid(people, sender_id, receiver_id) ){
        remove_extra_spaces(message);
        save_new_message(people, sender_id, receiver_id, message);
        cout << "sent" << endl;
    }
    else
        cout << "failed" << endl;
}

void print_messages(vector<Person>& people, int id){
    for(int i = 0; i < people.size(); i++)
        if(people[i].id == id)
            for(int j = 0; j < people[i].msgs.size(); j++){
                cout << people[i].msgs[j].sender_id << ' ';
                cout << people[i].msgs[j].message << endl;
            }
}

void show_message(vector<Person>& people, string& _command){
    int id = _command[SHOW_MSG_ID_INDEX] - ZERO_ASCII_CODE;
    if( is_there_id(people, id) ){
       print_messages(people, id);
    }
    else
        cout << "failed" << endl;
}

void check_input(vector<Person>& people, string& _command){
    if( (_command.length() == SHOW_MSG_COMMAND_LENGTH)  &&
     (_command.substr(SERNDER_ID_INDEX,SHOW_MSG_LENGTH) == "show_msg") )
        show_message(people, _command);
    else{
        int sender_id = _command[SERNDER_ID_INDEX] - ZERO_ASCII_CODE;
        int receiver_id = _command[RECEIVER_ID_INDEX] - ZERO_ASCII_CODE;
        string message = _command.substr(MSG_INDEX);

        send_message(people, sender_id, receiver_id, message);
    }
}

int main(){
    vector<Person> people;
    string command;

    read_informations(people,"people.txt");
    set_friends(people);

    while(getline(cin,command))
        check_input(people,command);

    return 0;
}