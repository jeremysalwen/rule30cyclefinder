#include <vector>
#include <iostream>
#include <cstdlib>
#include <tr1/unordered_set>

#include <boost/dynamic_bitset.hpp>
struct circular_equals : std::binary_function<boost::dynamic_bitset<>,boost::dynamic_bitset<>,std::size_t>
{
    bool  operator()(const std::pair<boost::dynamic_bitset<>,int>& x,const std::pair<boost::dynamic_bitset<>,int>& y) const
    {
        for(unsigned int count=0; count<x.first.size(); count++) {
            bool conflict=false;
            unsigned int index=0;
            for(unsigned int i=count; i<x.first.size(); i++) {
                if(x.first[i]!=y.first[index++]) {
                    conflict=true;
                }
            }
            for(unsigned int i=0; i<count; i++) {
                if(x.first[i]!=y.first[index++]) {
                    conflict=true;
                }
            }
            if(!conflict) {
                return true;
            }
        }
        return false;
    }
};


struct circular_hash : std::unary_function<std::pair<boost::dynamic_bitset<>,int>,std::size_t>
{

    std::size_t  operator()(const std::pair<boost::dynamic_bitset<>, int>& x) const
    {
        std::size_t result=0;
        for(unsigned int count=0; count<x.first.size(); count++) {
            std::size_t local_result=0;
            for(unsigned int i=count; i<x.first.size(); i++) {
                local_result=31*local_result+x.first[i];
            }
            for(unsigned int i=0; i<count; i++) {
                local_result=31*local_result+x.first[i];
            }
            result^=local_result;
        }
        return result;
    }
};

int old_main(int argc, char *argv[])
{
    if(argc<2) {
        std::cerr <<"error, need a numerical argument to determine the number of cells" <<std::endl;
        return 1;
    }
    int cellsize=atoi(argv[1]);
    if(cellsize<=1) {
        if(cellsize ==1) {
            std::cout << "Optimizations make size one simulations not work, but the answer is 2"<<std::endl;
            return 0;
        }
        std::cerr<<"Bad argument given: " << cellsize << " Must be >1"<<std::endl;
        return 2;
    }
    std::tr1::unordered_set<std::pair<boost::dynamic_bitset<>,int>,circular_hash,circular_equals> lines;
    boost::dynamic_bitset<> firstline(cellsize);
    firstline[0]=true;
    boost::dynamic_bitset<> lastline(cellsize);
    lines.insert(std::pair<boost::dynamic_bitset<>,int>(firstline,0));
    int count=1;
    while(true) {
        lastline[0]=(firstline[0]|firstline[1])^firstline[cellsize-1];
        //std::cout << lastline[0];
        for(int i=1; i<cellsize-1; i++) {
            bool l=firstline[i-1];
            bool m=firstline[i];
            bool r=firstline[i+1];
            lastline[i]= (m | r) ^ l;
            // std::cout << lastline[i];
        }
        lastline[cellsize-1]=(firstline[cellsize-1]| firstline[0]) ^ firstline[cellsize-2];
        //std::cout << lastline[cellsize-1];
        // std::cout<<std::endl;
        std::pair<boost::dynamic_bitset<>,int> newp(lastline,count);
        if(lines.count(newp)) {
            std::cout << "In " << count <<" from cycle started at " << lines.find(newp)->second <<std::endl;
            return 0;
        }
        lines.insert(newp);
        lastline.swap(firstline);
        count++;
    }
}

void updateState(std::vector<char>& olds, std::vector<char>& news) {
    unsigned int size=olds.size();
    news[0]=(olds[0]|olds[1])^olds[size-1];
    for(unsigned int i=1; i<size-1; i++) {
        bool l=olds[i-1];
        bool m=olds[i];
        bool r=olds[i+1];
        news[i]= (m | r) ^ l;
    }
    news[size-1]=(olds[size-1]|olds[0])^olds[size-2];
}

unsigned long stateoffset(std::vector<char>& a, std::vector<char>& b) {
    unsigned int sum1=0;
    unsigned int sum2=0;
    for(unsigned int i=0; i<a.size(); i++) {
        sum1+=a[i];
        sum2+=b[i];
    }
    if(sum1!=sum2) {
        return 0;
    }

    for(unsigned int count=0; count<a.size(); count++) {
        unsigned int index=0;
        for(unsigned int i=count; i<a.size(); i++) {
            if(a[index++]!=b[i]) {
                goto end_loop;
            }
        }
        for(unsigned int i=0; i<count; i++) {
            if(a[index++]!=b[i]) {
                goto end_loop;
            }
        }
        return count+1;
end_loop: ;
    }
    return 0;
}

void nthstate(std::vector<char>& state, unsigned long n, std::vector<char>& tmp) {
    unsigned long m=n/2;
    for(unsigned int i=0; i<m; i++) {
        updateState(state,tmp);
        updateState(tmp,state);
    }
    if(n&1) {
        updateState(state,tmp);
        state=tmp;
    }
}

unsigned long calc_real_loop_size(unsigned long offset, unsigned long loopsize, unsigned long cellsize) {
    unsigned long offsetpowers=offset;
    unsigned long result=loopsize;
    while(offsetpowers) {
        offsetpowers+=offset;
        if(offsetpowers>=cellsize) {
            offsetpowers-=cellsize;
        }
        result+=loopsize;
    }
    return result;
}

int main(int argc,char* argv[]) {
    if(argc<2) {
        std::cerr <<"error, need a numerical argument to determine the number of cells" <<std::endl;
        return 1;
    }
    int cellsize=atoi(argv[1]);
    if(cellsize<=1) {
        if(cellsize ==1) {
            std::cout << "Optimizations make size one simulations not work, but the answer is 2"<<std::endl;
            return 0;
        }
        std::cerr<<"Bad argument given: " << cellsize << " Must be >1"<<std::endl;
        return 2;
    }
    std::vector<char> slow(cellsize);
    std::vector<char> fast1(cellsize);
    std::vector<char> fast2(cellsize);

    slow[0]=1;
    fast1[0]=1;
    fast2[0]=1;
    unsigned long loopoffset=0;
    unsigned long stateschecked=0;
    unsigned long loopsize=0;
    //Really a factor of 2 over this.
    unsigned long jumpsize=1;
    while(true) {
        for(unsigned long i=0; i<jumpsize; i++) {
            updateState(fast1,fast2);
            loopoffset=stateoffset(fast2,slow);
            if(loopoffset) {
                loopoffset--; //loopoffset returns 1+the actual offset, so 0 can represent false.
                loopsize=i*2+1;
                goto done_while;
            }
            updateState(fast2,fast1);
            loopoffset=stateoffset(fast1,slow);
            if(loopoffset) {
                loopoffset--;
                loopsize=i*2+2;
                goto done_while;
            }
        }
        slow=fast1;
        stateschecked+=jumpsize*2;
        jumpsize*=2;
        std::cout<<"Now checking loop size up to "<<jumpsize*2 <<std::endl;
    }
done_while:
    std::cout << "Found loop after checking " <<stateschecked <<" states.  Loop size (ignoring rotations) is "<< loopsize <<"." <<std::endl;
    unsigned long realloopsize=calc_real_loop_size(loopoffset,loopsize,cellsize);
    std::cout << "Calculating corrected loop size.  Offset is "<<loopoffset<<", full loop size is "<< realloopsize<< "."<<std::endl;
    std::vector<char> lastbadstate(cellsize);
    unsigned long lastBad;
    if(stateschecked<jumpsize*2) {
        lastBad=0;
    }else {
        lastBad=stateschecked-jumpsize*2;
    }
    unsigned long interval=jumpsize*2;
    lastbadstate[0]=1;
    for(int i=1; i<cellsize; i++) {
        lastbadstate[i]=0;
    }
    nthstate(lastbadstate,lastBad,fast1);
    std::vector<char> test(cellsize);
    std::vector<char> test2(cellsize);
    while(interval>1) {
        unsigned long teststate=interval/2;
        test=lastbadstate;
        nthstate(test,teststate,fast1);
        test2=test;
        nthstate(test2,loopsize,fast1);
        if(stateoffset(test,test2)) {
            interval=teststate;
        } else {
            lastbadstate=test;
            lastBad+=teststate;
            interval=interval-teststate;
        }
        std::cout<<"Narrowed down interval to be between " << lastBad << " and "<< (lastBad+interval) << "." <<std::endl;
    }
    std::cout << "RESULTS: The cycle begins at position " << (lastBad+1) << " and is of length "<<realloopsize << "." <<std::endl;
}
