#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>

// [url](https://leetcode.com/problems/longest-substring-without-repeating-characters/)

class Solution {
public:
    int lengthOfLongestSubstring( std::string s )
    {
        std::string::difference_type max_length{ 0 };

        // Working set of chars appearing in the sequence.
        // The easiest and most handy storage type is `std::set` or `std::unordered_set`
        //
        // Although in order to speed up the code, we use a raw array of bools for each of
        // 256 possible chars. `true` means the char occurs in the sequence.
        bool char_already_used[ 256 ];
        std::fill( std::begin( char_already_used ), std::end( char_already_used ), false );

        auto it = s.cbegin();
        auto it_begin = s.cbegin();
        auto it_first = s.cbegin();             // First char in the sequence
        for( ; it != s.cend(); ) {

            const char ch = *it;

            if( !char_already_used[ static_cast< uint8_t >( ch ) ] ) [[likely]] {
                char_already_used[ static_cast< uint8_t >( ch ) ] = true;
                ++it;
                continue;
            }

            // Another unique substring is found.

            // Update `max_length` if the substring is bigger
            max_length = std::max( max_length, std::distance( it_first, it ) );

            // The brute force algorithm would return back to a char next to the first char of the substring being processed
            // (which is `it_first + 1`) and start from beginning. But this is better to skip first chars up to the first occurrence
            // of the duplicated char.
            // So we restart from the char next to the first occurrence of `ch`.
            // That means we need to "truncate" the left part of the substring.
            //
            // Example. Consider we have ' b a c d e a f g h a b c'
            //                               -       -
            // Once we've reached the second 'a' --- ^, we truncate the result of processing the left part of the substring which is 'b a'
            // And then continue processing  from 'c': 'c d e a f g h '

            // The first occurrence:
            auto rit_first_occurrence = std::find( std::make_reverse_iterator( it ), std::make_reverse_iterator( it_first ), ch );
            auto next_to_first_occurrence = rit_first_occurrence.base();        // remember that .base() returns reverse_it + 1;

            // Guardian. To be commented in order to optimize the runtime
            if( std::distance( it_begin, next_to_first_occurrence ) <= std::distance( it_begin, it_first ) ) {
                // Software error. Fallback to greedy algorithm
                std::cerr << "software error with ch = " << ch << " in position: " << std::distance( it_begin, it ) << std::endl;
                ++it_first;
                continue;
            }

            // Remove chars from truncated substring in `char_already_used`
            for( auto _it2 = it_first; _it2 != next_to_first_occurrence; ++_it2 ) {
                char_already_used[ static_cast< uint8_t >( *_it2 ) ] = false;
            }

            // Move forward the first char iterator
            it_first = next_to_first_occurrence;

            // Don't move 'it' since current char was treated as duplication and needs to be processed once again
        }

        // Reached the end of the string. Recalculate max_length since we may have something bigger than previously stored
        max_length = std::max( max_length, std::distance( it_first, it ) );

        return static_cast< int >( max_length );
    }
};

int main( void )
{
    Solution s;
    std::cout << "The result is: " << s.lengthOfLongestSubstring( "bcadeafghabcdef" ) << std::endl;

    return 0;
}
