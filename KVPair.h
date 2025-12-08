#ifndef KVPAIR_H
#define KVPAIR_H

template <typename T>
class KVPair {
private: 
    T Val;
    int Key;
public:
    KVPair(int key, T val) : Key(key), Val(val) {}
    int getKey() const { return Key; }
    T getVal() const { return Val; }

    void setVal(T val) { Val = val; }
    void setKey(int key) { Key = key;}

    bool operator<(const KVPair<T>& other) const {
        return Key < other.Key;}

    bool operator>(const KVPair<T>& other) const {
        return Key > other.Key;}
    bool operator==(const KVPair<T>& other) const {
        return Key == other.Key;}
    bool operator!=(const KVPair<T>& other) const {
        return Key != other.Key;
    }
    bool operator<=(const KVPair<T>& other) const {
        return Key <= other.Key;
    }
    bool operator>=(const KVPair<T>& other) const {
        return Key >= other.Key;
    }
};

#endif // KVPAIR_Hd