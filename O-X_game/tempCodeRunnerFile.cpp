#include<bits/stdc++.h>
using namespace std;
int main(){
    int t,p;
    cin>>t>>p;
    int l;
    if(t<p){cout<<-1;return 0;}
    if(t%2==0){l=t/2;}
    else{l=(t/2)+1;}
    int ans=(l)+(p-(l%p));
    cout<<ans;
}