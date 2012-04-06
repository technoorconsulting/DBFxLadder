#include <map>
template<typename KeyType, typename ValueType, int N>
class mapmaker
{
    std::pair<KeyType, ValueType> (&table)[N];
    const KeyType (&keys)[N];
    const ValueType (&vals)[N];

    template<int pos> void fill_pair()
    {
        table[pos].first = keys[pos];
        table[pos].second = vals[pos];
        fill_pair<pos-1>();
    }

    template<> void fill_pair<0>()
    {
        table[0].first = keys[0];
        table[0].second = vals[0];
    }
public:
    mapmaker( std::pair<KeyType, ValueType> (&t)[N], const KeyType (&k)[N], const ValueType (&v)[N] )
        : table(t), keys(k), vals(v)
    {
        fill_pair<N-1>();
    }
};

template<typename KeyType, typename ValueType, int N>
std::map<KeyType,ValueType> make_map(const KeyType (&keys)[N], const ValueType (&vals)[N])
{
    std::pair<KeyType, ValueType> table[N];
    mapmaker<KeyType, ValueType, N>( table, keys, vals );
    return std::map<KeyType, ValueType>(table, table+N);
}
