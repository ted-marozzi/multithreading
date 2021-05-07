#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <future>
using namespace std;

using namespace std::chrono;
typedef int (*fn)(int arg);

vector<vector<int>> chunks(vector<int> &array, int numThreads)
{
    int chunkSize = array.size() / numThreads;
    int lastChunkSize = array.size() - chunkSize * numThreads;
    vector<vector<int>> list;
    for (size_t i = 0; i < array.size(); i += chunkSize)
    {
        int last = min(array.size(), i + chunkSize);
        list.emplace_back(array.begin() + i, array.begin() + last);
    }

    list.emplace_back(array.end() - lastChunkSize, array.end());

    return list;
}

void printVec(vector<int> &v)
{
    cout << "{ ";
    int i;
    for (i = 0; i < v.size() - 1; i++)
    {
        cout << v[i] << ", ";
    }
    cout << v[i] << "}" << endl;
}

void printVecOfVec(vector<vector<int>> v)
{

    int i;
    cout << "{ " << endl;
    for (i = 0; i < v.size() - 1; i++)
    {
        cout << "\t";
        printVec(v[i]);
    }
    cout << "}" << endl;
}

vector<int> mapVec(vector<int> &v, fn func)
{
    vector<int> ans;
    for (int i = 0; i < v.size(); i++)
    {
        ans.push_back(func(v[i]));
    }
    return ans;
}

vector<int> mapThreader(fn func, vector<int> &v, int numThreads)
{
    vector<vector<int>> vecOfVec = chunks(v, numThreads), temp;

    vector<future<vector<int>>> futures;

    vector<int> ans;
    for (int i = 0; i < vecOfVec.size(); i++)
    {
        // For each sub array, start a thread
        // when all threads are done, join the array back up

        futures.push_back(async(launch::async, mapVec, ref(vecOfVec[i]), func));
    }

    for (int i = 0; i < futures.size(); i++)
    {
        // For each sub array, start a thread
        // when all threads are done, join the array back up
        temp.push_back(futures[i].get());
    }

    for (int i = 0; i < vecOfVec.size(); i++)
    {
        for (int j = 0; j < vecOfVec[i].size(); j++)
        {
            ans.push_back(temp[i][j]);
        }
    }

    return ans;
}

int doubleInt(int i)
{
    return i * 2;
}

int slowDouble(int i)
{
    this_thread::sleep_for(milliseconds(1));
    return i * 2;
}

int main()
{

    vector<int> v(1000, 2);
    cout << "Created vector of 1000 integers of value 2" << endl;
    /*************************************************************************/
    // 500 thread sim
    /*************************************************************************/
    auto start1 = high_resolution_clock::now();
    vector<int> doubledThreaded = mapThreader(slowDouble, v, 500);
    auto stop1 = high_resolution_clock::now();

    auto time1 = duration_cast<milliseconds>(stop1 - start1).count();
    cout << "Slow doubled vector in: " 
        << time1
        << "ms, values are now: " << doubledThreaded[0] << endl;

    /*************************************************************************/
    // 1 Thread sim
    /*************************************************************************/
    auto start = high_resolution_clock::now();
    vector<int> doubled = mapThreader(slowDouble, v, 1);
    auto stop = high_resolution_clock::now();

    auto time = duration_cast<milliseconds>(stop - start).count();
    cout << "Slow doubled vector in: " 
        << time
        << "ms, values are now: " << doubled[0] << endl;
    /*************************************************************************/

    cout << "Threaded map is approx " << (float)time/(float)time1 
        << " times faster." << endl;

    return 0;
}