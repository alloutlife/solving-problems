#include <iostream>
#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <optional>
#include <type_traits>


// [url](https://www.youtube.com/watch?v=oBt53YbR9Kk&ab_channel=freeCodeCamp.org)
/*
 * Write a function that takes in a targetSum and an array of numbers as arguments.
 *
 * The function should return an array containing the *shortest* combination of numbers
 * that add up to exactly the targetSum.
 *
 * Numbers can be use multiple times.
*/

template< class T >
class Solution {
private:

    // Memoization
    using memo_t = std::unordered_map< T, std::vector< T > >;
    memo_t memo_;

    // Nembers set is always the same, keep the pointer on it in order to
    // avoid passing it each time recursively
    const std::unordered_set< T >* p_numbers_{ nullptr };

public:

    std::optional< std::vector< T > > bestSum( T target, const std::unordered_set< T >& numbers )
    {
        static_assert( std::is_integral_v<T> );
        static_assert( std::is_signed_v<T> );

        // Reduce recursive method arguments list
        memo_.clear();
        p_numbers_ = &numbers;

        return _bestSum_impl( target );
    }

private:

    std::optional< std::vector< T > > _bestSum_impl( T target )
    {
        auto it_already = memo_.find( target );
        if( it_already != memo_.cend() ) {
            return it_already->second;
        }

        if( target == 0 ) {
            return std::vector< T >{};
        }
        if( target < 0 ) {
            return {};
        }

        std::optional< std::vector< T > > best_result;

        for( const T val : *p_numbers_ ) {

            auto result = _bestSum_impl( target - val );

            if( !result.has_value() ) [[unlikely]] {
                continue;
            }

            // Add good value to the result
            result->push_back( val );

            if( !best_result.has_value() ) {
                best_result = result;
            } else if( result->size() < best_result->size() ) {
                best_result = std::move( result );
            }
        }

        if( best_result.has_value() ) {
            memo_.insert( typename memo_t::value_type{ target, best_result.value() } );
        }

        return best_result;
    }

public:

    template< class StreamT >
    static void Print( StreamT& str, std::optional< std::vector< T > > result )
    {
        if( !result.has_value() ) {
            str << "unresolved" << std::endl;
            return;
        }

        bool first = true;
        std::for_each( result->cbegin(), result->cend(),
            [&str, &first]( auto val )
            {
                if( first ) { first = false; } else { str << ", "; }
                str << val;
            }
        );
        str << std::endl;
    }

};

int main( void )
{
    Solution< int > s;

    auto result = s.bestSum( 7, { 2, 3, 4, 7 } );
    s.Print( std::cout, result);

    result = s.bestSum( 17, { 2, 3, 4, 5 } );
    s.Print( std::cout, result);

    // TODO: note that stack size is limited to 10000 calls for gcc on Linux
    result = s.bestSum( 10000, { 1, 2 } );
    s.Print( std::cout, result);

    return 0;
}

