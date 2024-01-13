#include<bits/stdc++.h>
using namespace std;
int main(){
    string username, password;
    int attemp_left=5;
    while(attemp_left--){
    cout << "\nattemp_left "<<attemp_left<<endl;;
    cout << "Enter username: "<<endl;
    cin >> username;
    cout <<"\nEnter password: "<<endl;
    cin >> password;
    int incryption=1;
    for(int i=0;i<username.length();i++){
         incryption=incryption+int(username[i])*(i+1);
        
    }
    for(int i=0;i<password.length();i++){
         incryption=incryption+int(password[i])*(i+1);
    }
    int reading;
    ifstream tp("initial_detail.txt");
    while(tp>>reading){
    if(reading==incryption){
        cout<<"granted_access";
        return 0;
    }}
    cout<<"\nwrong_credentials";
    }
    cout<<"\nID_locked for next 24 hours :(";
    return 0;
}
