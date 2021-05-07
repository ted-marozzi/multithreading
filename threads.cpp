#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
using namespace std;

using namespace std::chrono;
typedef void (*fn)(int &arg);



vector<vector<int>> chunks(vector<int> &array, int numThreads)  {
    int chunkSize = array.size() / numThreads;
    int lastChunkSize = array.size() - chunkSize * numThreads;
    vector<vector<int>> list;
    for(size_t i = 0; i < array.size(); i += chunkSize) {
        int last = min(array.size(), i + chunkSize);
        list.emplace_back(array.begin() + i, array.begin() + last);
    }


    list.emplace_back(array.end() - lastChunkSize, array.end());

    return list;

}

void printVec(vector<int> &v)   {
    cout << "{ ";
    int i;
    for(i=0; i<v.size() - 1; i++)   {
        cout << v[i] << ", ";
    }
    cout << v[i] << "}" << endl;
}  

void printVecOfVec(vector<vector<int>> v)   {
    
    int i;
    cout << "{ "<< endl;
    for(i=0; i<v.size() - 1; i++)   {
        cout << "\t";
        printVec(v[i]);
   
    }
    cout << "}" << endl;
}

void mapVec(vector<int> &v, fn func)    {
    for(int i = 0; i<v.size(); i++) {
        func(v[i]);
    }
}

vector<int> mapThreader(fn func, vector<int> &v, int numThreads) {
    vector<vector<int>> vecOfVec = chunks(v, numThreads);
    vector<thread> threads;

    vector<int> ans;
    for(int i = 0; i<vecOfVec.size(); i++) {
        // For each sub array, start a thread 
        // when all threads are done, join the array back up
        
        threads.push_back(thread(mapVec, ref(vecOfVec[i]), func));
    }


    for(int i = 0; i<threads.size(); i++) {
        // For each sub array, start a thread 
        // when all threads are done, join the array back up
        threads[i].join();
    }

    for(int i =0; i< vecOfVec.size(); i++)    {
        for(int j = 0; j< vecOfVec[i].size(); j++)  {
            ans.push_back(vecOfVec[i][j]);
        }

    }

    return ans;

}


void doubleInt(int& i)  {
    i = i*2;
}

void slowDouble(int& i)  {
    i = i*1;
    this_thread::sleep_for(milliseconds(1));

}


 
int main()
{

    vector<int> v(1000, 2);


    auto start1 = high_resolution_clock::now();
    vector<int> doubledThreaded = mapThreader(slowDouble, v, 500);
    auto stop1 = high_resolution_clock::now();
    cout << "Done threaded in: " << duration_cast<microseconds>(stop1 - start1).count() << endl;



    auto start = high_resolution_clock::now();
    vector<int> doubled = mapThreader(slowDouble, v, 1);
    auto stop = high_resolution_clock::now();
    cout << "Done squared in: " << duration_cast<microseconds>(stop - start).count() << endl;
   
    
    return 0;
}