#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

template<class Key, class Value>
class dictionary
{
public:
    virtual ~dictionary() = default;

    virtual const Value& get(const Key& key) const = 0;
    virtual void set(const Key& key, const Value& value) = 0;
    virtual bool is_set(const Key& key) const = 0;
};

template<class Key>
class not_found_exception : public std::exception
{
public:
    virtual const Key& get_key() const noexcept = 0;
};

template<class Key>
class my_not_found_exception : public not_found_exception<Key>
{
    Key key_;
public:
    my_not_found_exception(const Key& key) : key_(key) {}

    virtual const Key& get_key() const noexcept override {
        return key_;
    }
};

template<class Key, class Value>
class my_dictionary final : public dictionary<Key, Value>
{
    std::map<Key, Value> map_;
public:
    virtual ~my_dictionary() = default;

    virtual const Value& get(const Key& key) const override {
        if (!is_set(key)) { // std::map<Key, Value>::contains C++20
            throw my_not_found_exception<Key>(key);
        }
        return map_.at(key);
    }

    virtual void set(const Key& key, const Value& value) override {
        map_.emplace(key, value);
    }

    virtual bool is_set(const Key& key) const override {
        return map_.find(key) != map_.end();
    }
};

template<class Key, class Value>
class smart_dictionary {
    dictionary<Key, Value>* dict_ = nullptr;
public:
    smart_dictionary() = default;
    smart_dictionary(dictionary<Key, Value>* const dict) : dict_(dict) {}
    ~smart_dictionary() {
        if (dict_) {
            delete dict_;
        }
    }

    bool not_null() const {
        return dict_ != nullptr;
    }

    dictionary<Key, Value>& get_obj() {
        return *dict_;
    }
};

int main() {
    smart_dictionary<int, std::string> dict_ptr(new my_dictionary<int, std::string>());

    // Checking "get" with no data in dict
    try {
        const std::string& value = dict_ptr.get_obj().get(5);
    } catch (const my_not_found_exception<int>& e) {
        std::cout << "get: Key = " << e.get_key() << " is not found in my_dictionary" << std::endl;
    }

    // Setting some data in dict
    dict_ptr.get_obj().set(5, "wow");

    // Checking "get" again
    const std::string& value = dict_ptr.get_obj().get(5);
    std::cout << "get: Key = 5, Value = " + value << std::endl;

    // Checking "is_set"
    std::cout << std::boolalpha << "is_set: Key 10 exists in the dictionary, it is - "
              << dict_ptr.get_obj().is_set(10) << std::endl;
    std::cout << "is_set: Key 5 exists in the dictionary, it is - "
              << dict_ptr.get_obj().is_set(5) << std::endl;

    getchar();
}