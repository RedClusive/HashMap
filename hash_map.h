#include <algorithm>
#include <functional>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    typedef typename std::list<std::pair<const KeyType, ValueType>>::iterator iterator;
    typedef typename std::list<std::pair<const KeyType, ValueType>>::const_iterator const_iterator;

private:
    const iterator null_it = iterator();

    size_t n = 0;
    size_t cp = 1;
    Hash hasher;
    std::vector<std::pair<iterator, size_t>> FirstPos{{null_it, 0}};
    std::list<std::pair<const KeyType, ValueType>> all;

public:
    iterator begin() {
        return all.begin();
    }

    iterator end() {
        return all.end();
    }

    const_iterator begin() const {
        return all.cbegin();
    }

    const_iterator end() const {
        return all.cend();
    }

    HashMap(const Hash& h = Hash()) : hasher(h) {
    }

    template <class Iter>
    HashMap(Iter first, Iter last, const Hash& h = Hash()) : hasher(h) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init, const Hash& h = Hash()) : hasher(h) {
        for (const auto& it : init) {
            insert(it);
        }
    }

    HashMap(const HashMap& other) {
        *this = other;
    }

    size_t size() const {
        return n;
    }

    bool empty() const {
        return (n == 0);
    }

    Hash hash_function() const {
        return hasher;
    }

    iterator find(const KeyType& key) {
        size_t id = hasher(key) % cp;
        iterator it = FirstPos[id].first;
        for (size_t i = 0; i < FirstPos[id].second; ++i, ++it) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t id = hasher(key) % cp;
        const_iterator it = FirstPos[id].first;
        for (size_t i = 0; i < FirstPos[id].second; ++i, ++it) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    void rebuild(size_t NewCp) {
        cp = NewCp;
        std::list<std::pair<const KeyType, ValueType>> allCopy = all;
        FirstPos.clear();
        all.clear();
        n = 0;
        FirstPos.resize(cp, {null_it, 0});
        for (iterator it = allCopy.begin(); it != allCopy.end(); ++it) {
            insert(*it);
        }
    }

    iterator insert(const std::pair<const KeyType, ValueType>& p) {
        iterator it = find(p.first);
        if (it != end()) {
            return it;
        }
        if (n + 1 >= cp * 2) {
            rebuild(cp * 2);
        }
        size_t id = hasher(p.first) % cp;
        if (FirstPos[id].second) {
            it = all.insert(FirstPos[id].first, p);
        } else {
            all.push_back(p);
            it = (--end());
        }
        FirstPos[id].second++;
        FirstPos[id].first = it;
        n++;
        return it;
    }

    void erase(const KeyType& key) {
        iterator it = find(key);
        if (it == end()) return;
        size_t id = hasher(key) % cp;
        if (FirstPos[id].second) {
            if (it == FirstPos[id].first) ++FirstPos[id].first;
        } else {
            FirstPos[id].first = null_it;
        }
        FirstPos[id].second--;
        all.erase(it);
        n--;
    }

    ValueType& operator[](const KeyType& key) {
        return insert({key, ValueType()})->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator it = find(key);
        if (it == end()) {
            throw std::out_of_range("epic fail");
        }
        return it->second;
    }

    void clear() {
        n = 0;
        cp = 1;
        all.clear();
        FirstPos.resize(1);
        FirstPos[0] = {null_it, 0};
    }

    HashMap& operator=(const HashMap& other) {
        if (begin() == other.begin()) return *this;
        hasher = other.hasher;
        clear();
        for (const auto& it : other) {
            insert(it);
        }
        return *this;
    }
};
