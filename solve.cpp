// Открытая индексация, robin hood hashing

#pragma once

#include <vector>
#include <stdexcept>
#include <list>
#include <iostream>

template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>>
class HashMap {
public:
    typedef typename std::list<std::pair<const KeyType, ValueType>> MyList;
    typedef typename MyList::const_iterator const_iterator;
    typedef typename MyList::iterator iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    HashMap(Hash hasher = Hash(), size_t capacity = 1);
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> lst, Hash hasher = Hash());
    template<typename iterator>
    HashMap(const iterator &begin, const iterator &end, Hash hasher = Hash()) : hasher_(hasher){
        capacity_ = 1;
        update_size();
        for (auto it = begin; it != end; ++it) {
            insert(*it);
        }
    }

    size_t size() const;
    bool empty() const;
    Hash hash_function() const;
    void insert(const std::pair<KeyType, ValueType> val);
    void erase(KeyType ind);
    ValueType& operator[](const KeyType ind);
    const ValueType& at(const KeyType ind) const;
    void clear();
    iterator find(const KeyType ind);
    const_iterator find(const KeyType ind) const;
    HashMap operator=(const HashMap& other);

private:
    const float LOAD_FACTOR = 0.57;
    size_t capacity_ = 1;
    Hash hasher_ = Hash();
    std::vector<iterator> link_element_;
    std::vector<size_t> dist_element_;
    std::vector<bool> used_element_;
    MyList list_element_;

    size_t get_need_index(const KeyType ind) const;
    void update_size();
    void rebuild();
};

template<typename KeyType, typename ValueType, typename Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::begin() {
    return list_element_.begin();
}

template<typename KeyType, typename ValueType, typename Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::end() {
    return list_element_.end();
}

template<typename KeyType, typename ValueType, typename Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::begin() const {
    return list_element_.begin();
}

template<typename KeyType, typename ValueType, typename Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::end() const {
    return list_element_.end();
}

template<typename KeyType, typename ValueType, typename Hash>
size_t HashMap<KeyType, ValueType, Hash>::get_need_index(const KeyType ind) const {
    size_t current_ind = hasher_(ind) % capacity_;
    while (used_element_[current_ind]) {
        if (link_element_[current_ind]->first == ind) {
            break;
        }
        ++current_ind;
        if (current_ind == capacity_)
            current_ind = 0;
    }
    return current_ind;
}

template<typename KeyType, typename ValueType, typename Hash>
size_t HashMap<KeyType, ValueType, Hash>::size() const {
    return list_element_.size();
}

template<typename KeyType, typename ValueType, typename Hash>
bool HashMap<KeyType, ValueType, Hash>::empty() const {
    return list_element_.empty();
}

template<typename KeyType, typename ValueType, typename Hash>
Hash HashMap<KeyType, ValueType, Hash>::hash_function() const {
    return hasher_;
}

template<typename KeyType, typename ValueType, typename Hash>
void HashMap<KeyType, ValueType, Hash>::insert(const std::pair<KeyType, ValueType> val) {
    auto current_ind = hasher_(val.first) % capacity_;
    size_t current_step = 1;
    list_element_.emplace_back(val);
    auto current_it = std::prev(end());

    while (true) {
        if (dist_element_[current_ind] < current_step) {
            std::swap(dist_element_[current_ind], current_step);
            std::swap(link_element_[current_ind], current_it);
            used_element_[current_ind] = true;
            if (!current_step) {
                break;
            }
        }
        ++current_step;
        ++current_ind;
        if (current_ind == capacity_) {
            current_ind = 0;
        }
    }
    if (list_element_.size() >= LOAD_FACTOR * capacity_) {
        rebuild();
    }
}

template<typename KeyType, typename ValueType, typename Hash>
ValueType &HashMap<KeyType, ValueType, Hash>::operator[](const KeyType ind) {
    auto current_ind = get_need_index(ind);
    if (used_element_[current_ind] != 1) {
        insert(std::make_pair(ind, ValueType()));
    }
    return link_element_[get_need_index(ind)]->second;
}

template<typename KeyType, typename ValueType, typename Hash>
const ValueType& HashMap<KeyType, ValueType,Hash>::at(const KeyType ind) const {
    auto current_ind = get_need_index(ind);
    if (used_element_[current_ind] != 1) {
        throw std::out_of_range("Aboba");
    }
    return link_element_[current_ind]->second;
}

template<typename KeyType, typename ValueType, typename Hash>
void HashMap<KeyType, ValueType, Hash>::erase(KeyType ind) {
    auto current_ind = get_need_index(ind);
    if (used_element_[current_ind]) {
        list_element_.erase(link_element_[current_ind]);
        dist_element_[current_ind] = 0;
        size_t pos = current_ind + 1;
        used_element_[current_ind] = false;
        if (pos == capacity_)
            pos = 0;
        while (dist_element_[pos] > 1) {
            size_t last_pos = pos;
            if (last_pos != 0) {
                --last_pos;
            } else {
                last_pos = capacity_ - 1;
            }
            used_element_[pos] = false;
            used_element_[last_pos] = true;
            link_element_[last_pos] = link_element_[pos];
            dist_element_[last_pos] = dist_element_[pos] - 1;
            dist_element_[pos] = 0;
            ++pos;
            if (pos == capacity_) {
                pos = 0;
            }
        }
    }
}

template<typename KeyType, typename ValueType, typename Hash>
typename HashMap<KeyType, ValueType, Hash>::iterator HashMap<KeyType, ValueType, Hash>::find(const KeyType ind) {
    auto current_ind = get_need_index(ind);
    if (used_element_[current_ind]) {
        return link_element_[current_ind];
    } else {
        return end();
    }
}

template<typename KeyType, typename ValueType, typename Hash>
typename HashMap<KeyType, ValueType, Hash>::const_iterator HashMap<KeyType, ValueType, Hash>::find(const KeyType ind) const {
    auto current_ind = get_need_index(ind);
    if (used_element_[current_ind]) {
        return link_element_[current_ind];
    } else {
        return end();
    }
}

template<typename KeyType, typename ValueType, typename Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash hasher, size_t capacity) : capacity_(capacity), hasher_(hasher) {
    ++capacity_;
    update_size();
}

template<typename KeyType, typename ValueType, typename Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(std::initializer_list<std::pair<KeyType, ValueType>> lst, Hash hasher) : hasher_(hasher){
    capacity_ = lst.size() * 2 + 1;
    update_size();
    for (auto i : lst) {
        insert(i);
    }
}

template<typename KeyType, typename ValueType, typename Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    list_element_.clear();
    fill(link_element_.begin(), link_element_.end(), iterator());
    fill(used_element_.begin(), used_element_.end(), 0);
    fill(dist_element_.begin(), dist_element_.end(), 0);
}

template<typename KeyType, typename ValueType, typename Hash>
void HashMap<KeyType, ValueType, Hash>::rebuild() {
    capacity_ = list_element_.size() * 3 + 1;
    auto old_value = list_element_;
    update_size();
    fill(link_element_.begin(), link_element_.end(), iterator());
    fill(used_element_.begin(), used_element_.end(), 0);
    fill(dist_element_.begin(), dist_element_.end(), 0);
    for (auto i : old_value) {
        insert(i);
    }
}

template<typename KeyType, typename ValueType, typename Hash>
void HashMap<KeyType, ValueType, Hash>::update_size() {
    link_element_.resize(capacity_);
    used_element_.resize(capacity_);
    dist_element_.resize(capacity_, 0);
    list_element_.clear();
}

template<typename KeyType, typename ValueType, typename Hash>
HashMap<KeyType, ValueType, Hash> HashMap<KeyType, ValueType, Hash>::operator=(const HashMap &other) {
    clear();
    hasher_ = other.hasher_;
    capacity_ = other.capacity_;
    update_size();
    for (auto i : other.list_element_) {
        insert(i);
    }
    return *this;
}
