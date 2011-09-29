#include <vector>
#include <iostream>
#include <cstdlib>
#include <tr1/unordered_set>

#include <boost/dynamic_bitset.hpp>
struct circular_equals : std::binary_function<boost::dynamic_bitset<>,boost::dynamic_bitset<>,std::size_t>
{
    bool  operator()(const boost::dynamic_bitset<>& x,const boost::dynamic_bitset<>& y) const
    {
        for(unsigned int count=0; count<x.size(); count++) {
            bool conflict=false;
            unsigned int index=0;
            for(unsigned int i=count; i<x.size(); i++) {
                if(x[i]!=y[index++]) {
                    conflict=true;
                }
            }
            for(unsigned int i=0; i<count; i++) {
                if(x[i]!=y[index++]) {
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


struct circular_hash : std::unary_function<boost::dynamic_bitset<>,std::size_t>
{

    std::size_t  operator()(const boost::dynamic_bitset<>& x) const
    {
        // std::cout<<"hash!" <<std::endl;
        std::size_t result=0;
        for(unsigned int count=0; count<x.size(); count++) {
            std::size_t local_result=0;
            for(unsigned int i=count; i<x.size(); i++) {
                local_result=31*local_result+x[i];
            }
            for(unsigned int i=0; i<count; i++) {
                local_result=31*local_result+x[i];
            }
            result^=local_result;
        }
        //std::cout << std::endl << result <<std::endl;
        return result;
    }
};
int main(int argc, char *argv[])
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
    std::tr1::unordered_set<boost::dynamic_bitset<>,circular_hash,circular_equals> lines;
    boost::dynamic_bitset<> firstline(cellsize);
    firstline[0]=true;
    boost::dynamic_bitset<> lastline(cellsize);
    lines.insert(firstline);
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
        if(lines.count(lastline)) {
            std::cout << "In " << count <<std::endl;
            return 0;
        }
        lines.insert(lastline);
        lastline.swap(firstline);
        count++;
    }
}
