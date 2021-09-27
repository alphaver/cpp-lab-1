#ifndef _LAB1_EXTERNAL_SET_H_
#define _LAB1_EXTERNAL_SET_H_

#include <bits/stdc++.h>

namespace word_counter 
{

class external_set 
{
private:
    struct node
    {
        std::uint64_t key;
        std::int64_t list_start_offset;
        std::int32_t priority;
        std::int64_t left_offset;
        std::int64_t right_offset;
    };
    struct list_node
    {
        std::int64_t val_offset;
        std::int64_t next_offset;
    };
    
    static thread_local std::random_device dev;
    static thread_local std::uniform_int_distribution<std::int32_t> distrib;    
    std::FILE *node_file, *list_file;
    std::uint32_t tree_size, lists_size;
    std::int64_t root_offset;

    void release();
    void read_node(std::int64_t offset, node &n) const;
    void write_node(std::int64_t offset, const node &n);
    void read_list_node(std::int64_t list_offset, list_node &n) const;
    void write_list_node(std::int64_t list_offset, const list_node &n); 
    void add_to_node_list(std::int64_t node_offset, std::int64_t val_offset);

    void set_split(
        std::int64_t root, std::uint64_t key,
        std::int64_t &left, std::int64_t &right
    );
    std::int64_t set_merge(std::int64_t left, std::int64_t right);
    std::int64_t set_find(std::uint64_t key) const;

public:
    static const std::int64_t null_offset = -1;

    external_set();
    ~external_set();

    void insert(std::uint64_t key, std::int64_t val_offset);
    std::int32_t find_all(
        std::uint64_t key, std::vector<std::int64_t> &val_offsets
    ) const;
    std::int32_t size() const;
};

class external_set_exception: public std::runtime_error
{
public:
    external_set_exception(const std::string &msg)
        : std::runtime_error(msg) {}
};

class counting_exception: public std::runtime_error
{
public:
    counting_exception(const std::string &msg)
        : std::runtime_error(msg) {}
};

} // namespace word_counter 

#endif // _LAB1_EXTERNAL_SET_H_
