// Read files and prints top k word by frequency

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

const size_t TOPK = 10;


using Counter = std::map<std::string, std::size_t>;

Counter freq_dict;
std::vector <std::thread> vThreads;
std::vector <std::string> vFiles;
std::vector <Counter> vCounter;

std::mutex mtx;

std::string tolower(const std::string &str);

void unit_vocs(Counter& counter1, Counter& counter2);

void count_words(std::istream& stream, Counter&);

void print_topk(std::ostream& stream, const Counter&, const size_t k);




int thFunc(const int i) {
    mtx.lock();
    std::ifstream input( vFiles.at(i));
    if (!input.is_open()) {
        std::cerr << "Failed to open file " << vFiles.at(i) << '\n';
        return EXIT_FAILURE;
    }
    count_words(input, freq_dict);
    vCounter.push_back(freq_dict);
    freq_dict.clear();
    mtx.unlock();
    return EXIT_SUCCESS;
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: topk_words [FILES...]\n";
        return EXIT_FAILURE;
    }

    vFiles.reserve( argc-1 );

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 1; i < argc; ++i) {
        vFiles.push_back( argv[i] );
        vThreads.push_back( std::thread(thFunc, i-1));
    }

    for (auto &thread : vThreads )
        thread.join();

    for ( size_t i = 1; i < vCounter.size(); ++i) {
        unit_vocs(vCounter.at(0), vCounter.at(i) );
    }

    print_topk(std::cout, vCounter.at(0), TOPK );
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Elapsed time is " << elapsed_ms.count() << " us\n";
}

std::string tolower(const std::string &str) {
    std::string lower_str;
    std::transform(std::cbegin(str), std::cend(str),
                   std::back_inserter(lower_str),
                   [](unsigned char ch) { return std::tolower(ch); });
    return lower_str;
}

void count_words(std::istream& stream, Counter& counter) {
    std::for_each(std::istream_iterator<std::string>(stream),
                  std::istream_iterator<std::string>(),
                  [&counter](const std::string &s) { ++counter[tolower(s)]; });
}

void unit_vocs(Counter& counter1, Counter& counter2) {
    //auto it = counter2.begin();
    for ( auto it = counter2.begin(); it != counter2.end(); ++ it) {
        auto mainVoc = counter1.find(it->first);
        if (mainVoc != counter1.end()) {
            size_t newSz = mainVoc->second + it->second;
            std::swap(mainVoc->second, newSz);
        }
        else {
            counter1.insert( std::pair<std::string, std::size_t>(it->first, it->second));
        }
    }
}

void print_topk(std::ostream& stream, const Counter& counter, const size_t k) {
    std::vector<Counter::const_iterator> words;
    words.reserve(counter.size());
    for (auto it = std::cbegin(counter); it != std::cend(counter); ++it) {
        words.push_back(it);
    }

    std::partial_sort(
        std::begin(words), std::begin(words) + k, std::end(words),
        [](auto lhs, auto &rhs) { return lhs->second > rhs->second; });

    std::for_each(
        std::begin(words), std::begin(words) + k,
        [&stream](const Counter::const_iterator &pair) {
            stream << std::setw(4) << pair->second << " " << pair->first
                      << '\n';
        });
}
