#include "iostream"
#include "vector"

using namespace std;

const int m1 = 79;
const int m2 = 97;

size_t str_hash(const std::string str, size_t m) {
    int hash = 0;
    for (auto a: str)
        hash = hash * m + a;
    return hash;
}

template<class T> struct Hasher {
    size_t operator() (const T* key) const {
        return key;
    }
};

template<> struct Hasher<std::string> {
    size_t operator() (const std::string& key, size_t size) const {
        return str_hash(key, m1);
    }
};

template <class T>
struct DefaultEqual {
    bool operator() (const T& l, const T& r) const {
        return l == r;
    }
};



template<class Key, class Equal = DefaultEqual<Key>, class Hash = Hasher<Key>>
class HashSet {
    static constexpr int max_load_factor = 7;
    struct Item {
        Key key;
        bool isEmpty = true;
        bool isDeleted = false;
    };

public:
    HashSet(Hash hash = Hash(), Equal equal = Equal()) :
            items(8),
            hash(hash),
            equal(equal)
    {
    }

    HashSet(const HashSet&) = delete;
    HashSet& operator=(const HashSet&) = delete;

    ~HashSet() {
        items.pop_back();
    }

    bool add(const Key& key) {
        if (has(key)) {
            return false;
        }

        if (1. * size / items.capacity() >= 0.75){
            grow();
        }

        size_t itemIdx = hash(key, m1);
        size_t stepIdx = hash(key, m2) + 1;
        int i = 0;
        size_t pos = (itemIdx + i * stepIdx) % items.capacity();
        size_t deleted = -1;

        while (!(items[pos].isEmpty)) {
            if (items[pos].isDeleted && deleted == -1) {
                deleted = pos;
            }

            pos = (itemIdx + i * stepIdx) % items.capacity();
            i++;
        }

        if (deleted != -1) {
            items[deleted].key = key;
            items[deleted].isDeleted = false;
            size += 1;

            return true;
        }

        items[pos].key = key;
        items[pos].isEmpty = false;
        if (items[pos].isDeleted)
            items[pos].isDeleted = false;
        size += 1;

        return true;
    }

    bool has(const Key& key) const {
        size_t itemIdx = hash(key, m1);
        size_t stepIdx = hash(key, m2) + 1;
        int i = 0;
        size_t pos = (itemIdx + i * stepIdx) % items.capacity();

        while (!(items[pos].isEmpty) && i < items.capacity()) {
            if (equal(items[pos].key, key) && !(items[pos].isDeleted)) {
                return true;
            }

            pos = (itemIdx + i * stepIdx) % items.capacity();
            i++;
        }

        return false;
    }

    bool del(const Key& key) {
        if (!has(key)) {
            return false;
        }

        size_t itemIdx = hash(key, m1);
        size_t stepIdx = hash(key, m2) + 1;
        int i = 0;
        size_t pos = (itemIdx + i * stepIdx) % items.capacity();

        while (!(items[pos].isEmpty) && i < items.capacity()) {
            if (equal(items[pos].key, key) && !(items[pos].isDeleted)) {
                items[pos].isDeleted = true;
                size -= 1;

                return true;
            }


            pos = (itemIdx + i * stepIdx) % items.capacity();
            i++;

        }

        return false;
    }

private:
    void grow() {
        size_t newCapacity = items.capacity() * 2;
        std::vector<Item> newHashSet(newCapacity);
        for (int i = 0; i < items.capacity(); ++i) {

            if(!(items[i].isEmpty)) {
                size_t keyIdx = hash(items[i].key, m1);
                size_t stepIdx = hash(items[i].key, m2) + 1;
                int m = 0;
                size_t pos = (keyIdx + m * stepIdx) % newHashSet.capacity();

                while (!(newHashSet[pos].isEmpty)) {
                    pos = (keyIdx + m * stepIdx) % newHashSet.capacity();
                    m++;
                }

                newHashSet[pos] = items[i];
                newHashSet[pos].isEmpty = false;
            }
        }

        items = newHashSet;
    }

    std::vector<Item> items;

    size_t size = 1;
    Hash hash;
    Equal equal;
};

//int main() {
//    HashSet<string> set;
//    std::string  key;
//    char operation = '\0';
//    while(std::cin >> operation >> key) {
//        bool res = false;
//        switch (operation) {
//            case '+':
//                res = set.add(key);
//                break;
//            case '?':
//                res = set.has(key);
//                break;
//            case '-':
//                res = set.del(key);
//                break;
//            default:
//                break;
//        }
//
//        if (res) {
//            std:: cout << "OK" << std::endl;
//        }
//        else {
//            std::cout << "FAIL" << std::endl;
//        };
//    }
//    return  0;
//}