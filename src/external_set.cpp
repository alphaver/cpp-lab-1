#include "external_set.h"

namespace word_counter
{

thread_local std::random_device external_set::dev;
thread_local std::uniform_int_distribution<std::int32_t> external_set::distrib;

void
external_set::release()
{
    std::fclose(node_file);
    std::fclose(list_file);
}

static void
read_block(FILE *file, std::int64_t offset, std::int32_t size, void *block)
{
    if (external_set::null_offset == offset)
        return;
    std::fseek(file, offset, SEEK_SET);
    if (size != std::fread(block, 1, size, file)) 
        throw external_set_exception("can't get info from the set");
}

static void
write_block(FILE *file, std::int64_t offset, std::int32_t size, const void *block)
{
    if (external_set::null_offset == offset)
        return;
    std::fseek(file, offset, SEEK_SET);
    if (size != std::fwrite(block, 1, size, file))
        throw external_set_exception("can't update the set");
}

void
external_set::read_node(std::int64_t offset, node &n) const
{
    read_block(node_file, offset, sizeof(node), &n);
}

void
external_set::write_node(std::int64_t offset, const node &n)
{
    write_block(node_file, offset, sizeof(node), &n);
}

void
external_set::read_list_node(std::int64_t list_offset, list_node &n) const
{
    read_block(list_file, list_offset, sizeof(list_node), &n);
}

void
external_set::write_list_node(std::int64_t list_offset, const list_node &n)
{
    write_block(list_file, list_offset, sizeof(list_node), &n);
}

void
external_set::add_to_node_list(std::int64_t node_offset, std::int64_t val_offset)
{
    node n;
    read_node(node_offset, n);
    list_node ln { val_offset, n.list_start_offset };

    std::int64_t new_node_offset = std::int64_t(lists_size) * sizeof(list_node);
    write_list_node(new_node_offset, ln);
    n.list_start_offset = new_node_offset;
    write_node(node_offset, n);
    ++lists_size;
}

void
external_set::set_split(
    std::int64_t root, std::uint64_t key, 
    std::int64_t &left, std::int64_t &right
)
{
    node n;
    std::int64_t res_left = null_offset, res_right = null_offset;
  
    read_node(root, n);
    if (null_offset == root)
        goto out;
    if (n.key > key) {
        set_split(n.left_offset, key, res_left, res_right);
        n.left_offset = res_right;
        res_right = root;
    } else {
        set_split(n.right_offset, key, res_left, res_right);
        n.right_offset = res_left;
        res_left = root;
    }
    write_node(root, n);

out:
    left = res_left;
    right = res_right;
}

std::int64_t
external_set::set_merge(std::int64_t left, std::int64_t right)
{
    if (null_offset == left)
        return right;
    if (null_offset == right)
        return left;
    node left_node, right_node;
    read_node(left, left_node);
    read_node(right, right_node);
    if (left_node.priority > right_node.priority) {
        left_node.right_offset = set_merge(left_node.right_offset, right);
        write_node(left, left_node);
        return left;
    } else {
        right_node.left_offset = set_merge(left, right_node.left_offset);
        write_node(right, right_node);
        return right;
    }
}

std::int64_t
external_set::set_find(std::uint64_t key) const
{
    std::int64_t curr_offset = root_offset;
    node n;
    read_node(root_offset, n);
    while (null_offset != curr_offset) {
        if (n.key > key)
            curr_offset = n.left_offset;
        else if (n.key < key)
            curr_offset = n.right_offset;
        else
            return curr_offset;
        read_node(curr_offset, n);
    }
    return null_offset;
}

external_set::external_set()
    : node_file(std::tmpfile())
    , list_file(std::tmpfile())  
    , tree_size(0)
    , lists_size(0)
    , root_offset(null_offset)
{
    if (!node_file || !list_file) {
        release();
        throw external_set_exception("can't create the table");
    }
}

external_set::~external_set()
{
    release();
}

void
external_set::insert(std::uint64_t key, std::int64_t val_offset)
{
    std::int64_t new_offset = tree_size * sizeof(node), left, right;
    std::int64_t node_offset = set_find(key);
    if (null_offset == node_offset) {
        node new_node { key, null_offset, distrib(dev), null_offset, null_offset };
        write_node(new_offset, new_node);
        set_split(root_offset, key, left, right);
        root_offset = set_merge(left, new_offset);
        root_offset = set_merge(root_offset, right); 
        node_offset = new_offset;
    } 
    add_to_node_list(node_offset, val_offset);
    ++tree_size;
}

std::int32_t
external_set::find_all(
    std::uint64_t key, std::vector<std::int64_t> &val_offsets
) const
{
    std::int64_t node_offset = set_find(key);
    if (null_offset == node_offset)
        return 0;
    
    node n;
    list_node ln;
    read_node(node_offset, n);
    std::int64_t list_offset = n.list_start_offset;
    std::int32_t elems_number = 0;
    while (null_offset != list_offset) {
        read_list_node(list_offset, ln);
        val_offsets.push_back(ln.val_offset);
        ++elems_number;
        list_offset = ln.next_offset;
    }
    return elems_number;
}

std::int32_t
external_set::size() const
{
    return lists_size;
}

} // namespace word_counter
