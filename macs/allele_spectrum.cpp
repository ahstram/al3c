#include<iostream>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include<set>
#include<map>
using namespace std;

const int pop_n[] = {176,170,372};
const char * pop_labels[]={"YRI","CEU","ASN"};

struct mafbin_t{
  float lower,upper;
  mafbin_t(float lower,float upper){
    this->lower = lower;
    this->upper = upper;
  }
};

struct byMafAsc{
  bool operator()(const mafbin_t & a,const mafbin_t & b){
    return (a.lower < b.lower && a.upper < b.upper);
  }
};

typedef map<mafbin_t,int,byMafAsc> mafbin_map_t;

mafbin_map_t mafbin_map_arr[3];

int testing(){
  mafbin_t testbin(0.02,0.02);
  mafbin_map_t::iterator it = mafbin_map_arr[0].find(testbin);
  if (it!=mafbin_map_arr[0].end()){
    mafbin_map_arr[0][testbin] = it->second+1;
    //mafbin_t mafbin  = *it;
    //++mafbin.counts;
    //mafbin_set_arr[0].insert(mafbin);
  }
  for(mafbin_map_t::iterator it=mafbin_map_arr[0].begin();it!=mafbin_map_arr[0].end();it++){
    mafbin_t mafbin  = it->first;
    cerr<<"Current bin "<<mafbin.lower<<"-"<<mafbin.upper<<":"<<it->second<<endl;
  }
  cerr<<"Total bins: "<<mafbin_map_arr[0].size()<<endl;
  return 0;
}

int main(int argc,char * argv[]){
  string line;
  int offset = static_cast<int>('0');
  float cutoffs[]={0,0.01,0.01,0.05,0.05,0.1,0.1,0.2,0.2,0.5};
  int bins = sizeof(cutoffs)/sizeof(float);
  for(int pop=0;pop<3;++pop){
    for(int i=0;i<bins/2;++i){
      mafbin_t mafbin(cutoffs[i*2],cutoffs[i*2+1]);
      mafbin_map_arr[pop].insert(pair<mafbin_t,int>(mafbin,0));
    }
  }
  while(getline(cin,line)){
    string tag="";
    if (line.length()>4) tag = line.substr(0,4);
    if(tag.compare("SITE")==0){
      istringstream iss(line);
      string token;
      for(int i=0;i<5;++i) iss>>token; 
      int j=0;
      //cerr<<tag;
      for(int pop=0;pop<3;++pop){
        float af = 0;
        for(int i=0;i<pop_n[pop];++i){
          af+=static_cast<int>(token[j])-offset;
          ++j;
        }
        af/=pop_n[pop];
        if(af>=.5) af = 1.0-af;
        mafbin_t testbin(af,af);
        mafbin_map_t::iterator it = mafbin_map_arr[pop].find(testbin);
        if (it!=mafbin_map_arr[0].end()){
          mafbin_map_arr[pop][testbin] = it->second+1;
        }
        //cerr<<"\t"<<af;
      }
      //cerr<<endl;
    }
  } 
  cout<<"POP";
  for(int bin=0;bin<bins/2;++bin){
    cout<<"\t"<<cutoffs[bin*2]<<"-"<<cutoffs[bin*2+1];
  }
  cout<<endl;
  for(int pop=0;pop<3;++pop){
    cout<<pop_labels[pop];
    //cerr<<"Bins for pop "<<pop<<": "<<mafbin_map_arr[pop].size()<<endl;
    for(mafbin_map_t::iterator it=mafbin_map_arr[pop].begin();it!=mafbin_map_arr[pop].end();it++){
      cout<<"\t"<<it->second;
      //mafbin_t mafbin = it->first;
      //cout<<"\t"<<"("<<mafbin.lower<<"-"<<mafbin.upper<<")"<<it->second;
    }
    cout<<endl;
  }
}
