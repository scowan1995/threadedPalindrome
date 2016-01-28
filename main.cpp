#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>

// Just makes the code clearer.
using Lines = std::vector<std::string>;

struct Result
{
    Result(int lineNumber_, int firstChar_, int length_)
            : lineNumber(lineNumber_), firstChar(firstChar_), length(length_)
    {}

    // This allows you to compare results with the < (less then) operator, i.e. r1 < r2
    bool
    operator<(Result const& o)
    {
        // Line number can't be equal
        return length < o.length ||
               (length == o.length && lineNumber <  o.lineNumber) ||
               (length == o.length && lineNumber == o.lineNumber  && firstChar < o.firstChar);
    }

    int lineNumber, firstChar, length;
};

// Removes all non letter characters from the input file and stores the result in a Lines container
Lines
strip(std::ifstream& file)
{
    Lines result;
    result.reserve(50000); // If reading is too slow try increasing this value

    std::string workString;

    while(std::getline(file,workString))
    {
        //Strip non alpha characters
        workString.erase(std::remove_if(workString.begin(), workString.end(),
                                        [] (char c) { return !std::isalpha(c); }
        ), workString.end());
        result.push_back(workString);
        workString.clear();
    }
    return result;
}

// CHANGE This Code (you can add more functions)-----------------------------------------------------------------------------

// PART A

int sliceVal;
int chunkNum;
std::mutex SVMutex;
std::mutex tell;
std::mutex fullLock;

Result SearchFromCentre(std::string param, int ID, int *linesCounter){
    fullLock.lock();
    std::stringstream ss(param);
    std::string line;
    int bestLen = 0;
    int bestStart = 0;
    int lineNumber = 0;
    for (int lines = 0; std::getline(ss, line, '\n'); lines++)
    {//loop through each line
        //std::cout<<"Looping through line number "<< lines<<"with id: "<< ID<<std::endl;
       // std::cout<<"line = "<< line<< std::endl;
       // std::cout<<"lines"<< lines<<std::endl;
        for(int centre= 0 ; centre < line.length(); centre++)
        {//pick a centre to expand from
         //   std::cout<< "centre = "<< centre<< std::endl;
            //Searches for an odd length Palidrome
            //std::cout<< centre<< std::endl;
            int i = centre -1;
            int j = centre +1;
            bool stillPal = true;
            while ((i>0&&j<line.length())&&stillPal)
            {
               // std::cout << "odd going through line i, j "<< i << "  "<< j<<std::endl;
                if (line[i]==line[j])
                {
                    i--;
                    j++;
                    std::cout<< "odd extend pal i: j:"<< i<< " "<< j<< " "<<line<<std::endl;

                }
                else if (j-i>bestLen)
                {
                   // std::cout << "j-i vs bestLen"<< j-i<< " "<< bestLen;
                    lineNumber = lines;
                    bestStart = i;
                    bestLen = j-i;
                    stillPal = false;
                  //  std::cout<< "i and j: "<< i<< " "<< j<< std::endl;
                  //  std::cout<< "just assigned start, len, lineNumber "<<i<<" "<<j-i<<" "<<lines<<std::endl;
                }
                else stillPal = false;
            }
            if (stillPal)
            {
                int x = (j<line.length()) ? j-i: line.length()-i;
                int y = (i>0) ? i : 0;
                if (x-y>bestLen) {
                    lineNumber = lines;
                    bestStart = (i > 0) ? i : 0;
                  //  std::cout<< "bestLen = "<< bestLen<< std::endl;
                    bestLen = (j < line.length()) ? j - i : line.length() - i;
                    stillPal = false;
                  //  std::cout << "i and j: " << i << " " << j << std::endl;
                  //  std::cout << "odd just assigned still pal start, len, lineNumber " << bestStart << " " << bestLen <<
                    //" " << lines << std::endl;
                }
            }
            //Searchs for an even length palindrom
            if (centre< line.length()-1)
            if (line[centre]==line[centre+1])
            {
                i = centre -1;
                j = centre + 2;
                stillPal = true;
                while (i>0&&j<line.length()&&stillPal)
                {
                   // std::cout << "even going through line i, j "<< i << "  "<< j<<std::endl;
                    if (line[i]==line[j])
                    {
                        i--;
                        j++;
                        std::cout<< "even extend pal i: j:"<< i<< " "<< j<< " "<<line<<std::endl;
                    }
                    else if (j-i>bestLen)
                    {
                      //  std::cout << "j-i vs bestLen"<< j-i<< " "<< bestLen;
                        lineNumber = lines;
                        bestStart = i;
                        bestLen = j-i;
                        stillPal = false;
                       // std::cout<< "i and j: "<< i<< " "<< j<< std::endl;
                       // std::cout<< "just assigned start, len, lineNumber "<<bestStart<<" "<<bestLen<<" "<<lines<<std::endl;
                    }
                    else stillPal = false;

                }
                if (stillPal)
                {
                    int x = (j<line.length()) ? j-i: line.length()-i;
                    int y = (i>0) ? i : 0;
                    if (x-y>bestLen)  {
                        lineNumber = lines;
                        bestStart = (i > 0) ? i : 0;
                       // std::cout<< "bestLen = "<< bestLen<< std::endl;
                        bestLen = (j < line.length()) ? j - i : line.length() - i;
                        stillPal = false;
                       // std::cout << "i and j: " << i << " " << j << std::endl;
                       // std::cout << "even just assigned still pal start, len, lineNumber " << bestStart << " " << bestLen <<
                      //  " " << lines << std::endl;
                    }
                }
            }

        }

    }
   // std::cout<<"finishing"<<std::endl;
    Result res = {0,0,0};
    res.lineNumber = lineNumber;
    res.firstChar = bestStart;
    res.length = bestLen;
    tell.lock();
    std::cout << "\nres in func len, lineNum, start: "<< res.length<< " "<< res.lineNumber<< "  "<< res.firstChar<<std::endl;
    tell.unlock();
    fullLock.unlock();
    return res;

}

void static_thread_function(std::string * str, Result *result, int Tid, int *linesCounter)
{
    SVMutex.lock();
    std::cout<< Tid<< std::endl;
    std::cout<< "here the string for id " << Tid<< " "<< str[Tid] << std::endl;
   /* Result x = SearchFromCentre(*str, Tid, linesCounter);
    if (*result<x){
        *result = x;
        (*result).lineNumber += (Tid*(*linesCounter));
    }*/
    SVMutex.unlock();

}

/*
void dynamic_thread_function(std::string str, Result *result)
{

    SVMutex.lock();
    if (sliceVal<chunkNum)
    {
        sliceVal++;
    }
    SVMutex.unlock();
    Result x = SearchFromCentre(str);
    if (*result<x) *result = x;

}
*/


Result FindPalindromeStatic(Lines const& lines, int numThreads)
{
    std::string *s;
    for (int i = 0; i< lines.size(); i++){
        s[i] = lines[i];
    }
    std::vector<std::thread> threadArray;
    std::vector<Result> results;
    int lineCounter = 0;

    for (int i = 0; i<numThreads; i++)
    {
        results.push_back({0,0,0});
    }
    sliceVal =0;
    int countLines = 0;
    for(int i = 0; i < numThreads; i++)
    {
       /* if (i == 0){
            std::stringstream ss(lines[i]);
            std::string line;
            while (std::getline(ss, line, '\n')) lineCounter++;
        }*/
        std::string l = lines[i];
        //dl;std::cout<< l<<std::en
        threadArray.push_back(std::thread(static_thread_function, &s[i], &(results[i]), i, &lineCounter));
    }



   for (auto &var: threadArray)
        var.join();
    Result returnVal = results[0];
    int count = 0;
    for (int i = 0; i<results.size(); i++) {
      //  std::cout << "\nvar: " << results.at(i).length << " " << results.at(i).lineNumber << "  " << results.at(i).firstChar << std::endl;
        if (returnVal < results.at(i)) {
            returnVal = results.at(i);
            count = i;
        }
    }
   // returnVal.lineNumber += ((*lineCounter)*count);
    std::cout<<"count lines" << countLines<<std::endl;
    std::cout << "\nresults final len, lineNum, start: "<< returnVal.length<< " "<< returnVal.lineNumber<< "  "<< returnVal.firstChar<<std::endl;
    return returnVal;

}



// PART B
Result
FindPalindromeDynamic(Lines const& lines, int numThreads, int chunkSize)
{
    int maxSize = lines[0].size() - numThreads*chunkSize; // DELETE this (its just so the compiler wont whine
    return {0,0,maxSize}; // You could also call the constructor of Result
}

// DONT CHANGE THIS -----------------------------------------------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
    if(argc != 4)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <filename> <numThreads> <chunkSize>" << std::endl;
        return 0;
    }

    std::ifstream theFile(argv[1]);
    if(!theFile.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return 0;
    }
    int numThreads = std::atoi(argv[2]);
    int chunkSize  = std::atoi(argv[3]);

    std::cout << "Process " << argv[1] << " with " << numThreads << " threads using a chunkSize of " << chunkSize << " for dynamic scheduling\n" << std::endl;

    Lines lines = strip(theFile);

    //Part A
    Result aResult = FindPalindromeStatic(lines, numThreads);
    std::cout << "PartA: " << aResult.lineNumber << " " << aResult.firstChar << " " << aResult.length << ":\t" << lines.at(aResult.lineNumber).substr(aResult.firstChar, aResult.length) << std::endl;
    //Part B
    Result bResult = FindPalindromeDynamic(lines, numThreads, chunkSize);
    std::cout << "PartB: " << bResult.lineNumber << " " << bResult.firstChar << " " << bResult.length << ":\t" << lines.at(bResult.lineNumber).substr(bResult.firstChar, bResult.length) << std::endl;

    return 0;
}


