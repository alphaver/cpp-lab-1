#include "word_counter.h"

namespace word_counter
{

static std::uint32_t
unique_words_stream(std::istream &str)
{
    std::unordered_set<std::string> words;
    std::string word;
    while (str >> word)
        words.insert(word);
    return words.size();
}

std::uint32_t
unique_words_string(const std::string &text)
{
    std::istringstream text_str(text);
    return unique_words_stream(text_str);
}

std::uint32_t
unique_words_file(const std::string &file_name)
{
    std::ifstream file_str(file_name, std::ios::binary);
    return unique_words_stream(file_str);
}

} // namespace word_counter
