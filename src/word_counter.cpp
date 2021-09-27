#include "word_counter.h"
#include "external_set.h"

namespace word_counter
{

static const int block_size = 4096;
static const std::uint64_t hash_modulus = (1ULL << 61) - 1;
static const std::int64_t null_offset = -1; 

struct process_state 
{
    std::istream &str;
    std::int64_t curr_offset;
    std::int64_t curr_word_offset;
    std::uint64_t curr_word_hash;
    std::uint64_t hash_base;    
    word_counter::external_set set;
};

static int
compare_words(
    process_state &state, std::int64_t first_offset, std::int64_t second_offset
)
{
    std::streampos prev_offset = state.str.tellg();
    state.str.clear();
    bool first_space = false, second_space = false, equal = true;
    char first, second;
    while (!first_space && !second_space && equal) {
        state.str.seekg(first_offset, std::ios::beg);
        state.str.read(&first, 1);
        first_space = std::isspace(first) || state.str.eof();
        if (state.str.bad())
            throw counting_exception("comparison failed");
        state.str.seekg(second_offset, std::ios::beg);
        state.str.read(&second, 1);
        second_space = std::isspace(second) || state.str.eof();
        if (state.str.bad())
            throw counting_exception("comparison failed");
        
        equal = first == second;
        ++first_offset, ++second_offset;
    }
    state.str.seekg(prev_offset, std::ios::beg);
    
    if (first_space && second_space)
        return 0;
    if (first_space)
        first = '\0';
    if (second_space)
        second = '\0';
    if (first < second)
        return -1;
    else
        return 1;
}

static void
insert_word_if_unique(process_state &state)
{
    std::vector<std::int64_t> offsets;
    if (0 == state.set.find_all(state.curr_word_hash, offsets)) {
        state.set.insert(state.curr_word_hash, state.curr_word_offset);
        return;
    }
    for (auto offset : offsets) {
        if (0 == compare_words(state, offset, state.curr_word_offset))
            return;
    }
    state.set.insert(state.curr_word_hash, state.curr_word_offset);
}

static process_state
process_state_init(std::istream &str)
{
    static std::random_device dev;
    static std::uniform_int_distribution<std::uint64_t> 
        distrib(1, hash_modulus - 1);

    process_state state = {
        str,
        0,
        null_offset,
        0,
        distrib(dev)
    };

    return state;
}

static void
process_state_finish(process_state &state)
{
    if (null_offset != state.curr_word_offset)
        insert_word_if_unique(state); 
}

static void
process_block(process_state &state, char *block, std::size_t block_size)
{
    for (char *pos = block; pos - block != block_size; ++pos) {
        if (null_offset == state.curr_word_offset && !std::isspace(*pos)) {
            state.curr_word_offset = state.curr_offset;
        } else if (!std::isspace(*pos)) {
            state.curr_word_hash = 
                (((unsigned __int128)state.curr_word_hash * state.hash_base) +
                *pos) % hash_modulus;
        } else if (null_offset != state.curr_word_offset) {
            insert_word_if_unique(state);
            state.curr_word_offset = null_offset;
            state.curr_word_hash = 0;   
        }
        ++state.curr_offset;
    }
}

static std::uint32_t
unique_words_stream(std::istream &str)
{
    process_state state = process_state_init(str);

    char block[block_size];
    while (str.read(block, block_size) || str.gcount() > 0) {
        std::size_t chars_read = std::min(str.gcount(), std::streamsize(block_size));
        process_block(state, block, chars_read);
    }
    if (str.bad())
        throw counting_exception("reading from file failed");
     
    return state.set.size();
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
