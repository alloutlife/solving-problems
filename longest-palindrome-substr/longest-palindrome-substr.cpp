#include <iostream>
#include <string>
#include <map>
#include <set>

// [url](https://leetcode.com/problems/longest-palindromic-substring/)

class Solution {
private:
    using index_t = std::string::size_type;
    static bool _check_bounds( index_t l, index_t r, std::string::size_type size )
    {
        if( l < 0 || r >= size ) {
            return false;
        }
        return true;
    }

public:
    std::string longestPalindrome( std::string s ) {

        // Degenerated case
        if( s.size() < 2 ) {
            return s;
        }

        // Note: palindrome can be of even or odd size: "abba", "racecar".

        // Current result
        std::string result;

        // Define a container to store all the results. Might be useful for further optimizations
        // std::set< std::pair< index_t, index_t > > intermediate_palindromes;


        // Iterate through the string in search of palindrome centers
        index_t pos = 0;
        const auto string_size = s.size();
        bool b_odd_step{ true };
        for( ; pos < ( string_size - 1 ); ( b_odd_step ? b_odd_step = false : ( ++pos, b_odd_step = true ) ) ) {

            // Prepare iterators to traverse to the left and to the right
            index_t pos_l = pos;
            index_t pos_r = b_odd_step ? pos : pos + 1;

            if( !b_odd_step && ( s.at( pos_l ) != s.at( pos_r ) ) ) {
                // initial check for even-sized palindrome
                continue;
            }

            // Expand the length of a palindrome
            for( ; pos_l != 0; ) {              // `index_t` is unsigned, so we shouldn't fall into the loop with pos_l=0

                // Probe another step
                --pos_l; ++pos_r;
                if(    !_check_bounds( pos_l, pos_r, string_size )
                    || ( s.at( pos_l ) != s.at( pos_r ) )
                ) {
                    // rollback
                    ++pos_l; --pos_r;
                    break;
                }
            }

            auto another_palindrome_size = pos_r - pos_l + 1;
            if( ( another_palindrome_size ) >= 1 ) {
                // intermediate_palindromes.insert( std::pair< index_t, index_t >{ pos_l, pos_r } );
                if( another_palindrome_size > result.size() ) [[unlikely]] {
                    result = s.substr( pos_l, another_palindrome_size );
                }
            }
        }

        return result;
    }
};

int main( void )
{
    Solution s;
   // std::cout << s.longestPalindrome( "asfkfsajasdfgfdsaadda" ) << std::endl;

    std::cout << s.longestPalindrome( "babad" ) << std::endl;

    return 0;
}

