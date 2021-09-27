#include "word_counter.h"
#include <bits/stdc++.h>

using word_counter::unique_words_string;

std::string text = "Hello, I'm D. and I'm a fucking pervert ! ! !";

int
main(int argc, char **argv)
{
    std::cout << word_counter::unique_words_string(text) << '\n' 
        << word_counter::unique_words_file(argv[1]) << '\n';       
    return 0;
}
