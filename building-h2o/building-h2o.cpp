#include <iostream>
#include <mutex>
#include <functional>
#include <condition_variable>

// [url](https://leetcode.com/problems/building-h2o/)

class H2O {

public:
    H2O() {

    }

    void hydrogen(std::function<void()> releaseHydrogen)
    {
        bool need_clearing = _wait_for_atom_to_be_released( atom_type::hydrogen );

        std::scoped_lock _{ mtx4_ };
        {
            // releaseHydrogen() outputs "H". Do not change or remove this line.
            releaseHydrogen();
        }

        if( need_clearing ) {
            _clear_fields();
        }
    }

    void oxygen(std::function<void()> releaseOxygen) {

        bool need_clearing = _wait_for_atom_to_be_released( atom_type::oxygen );

        std::scoped_lock _{ mtx4_ };
        {
            // releaseOxygen() outputs "O". Do not change or remove this line.
            releaseOxygen();
        }

        if( need_clearing ) {
            _clear_fields();
        }
    }


private:

    enum class atom_type {
        oxygen, hydrogen
    };

    // There are three stages in each atom life cycle:
    // 1. atom waits for the barrier access to construct a molecule
    // 2. constructing the molecule
    // 3. releasing the molecule (three nested stages)
    //
    // Note. Classic synchronization instruments are used
    //
    // returns true if we need to clear all the fields representing atom positions
    bool _wait_for_atom_to_be_released( atom_type type )
    {
        bool we_are_the_last_to_build_h2o{ false };

        // Check if we can access the barrier. If we can, then occupy vacant position, else wait
        {
            // 1. Wait for the barrier access
            std::unique_lock _lck1{ mtx1_ };
            cv_access_to_barrier_opened_.wait( _lck1, [&] {

                if( type == atom_type::hydrogen ) {
                    return hydrogen_slots_ < 2;
                } else {
                    return oxygen_slots_ < 1;
                }

            });

            // Occupy
            if( type == atom_type::hydrogen ) {
                ++hydrogen_slots_;

            } else {
                ++oxygen_slots_;
            }
            {
                std::scoped_lock _{ mtx2_ };
                ++atoms_waiting_;
                if( 3 == atoms_waiting_ ) {
                    we_are_the_last_to_build_h2o = true;
                }
            }
        }

        if( we_are_the_last_to_build_h2o ) {

            // If we are the third atom - trigger molecule release procedure
            std::scoped_lock _{ mtx2_ };
            cv_start_release.notify_all();
        } else {

            // First two atoms are waiting for the last one to build h2o
            std::unique_lock lck{  mtx2_ };
            cv_start_release.wait( lck, [&] { return atoms_waiting_ == 3; } );
        }

        // Perform release
        {
            std::scoped_lock _{ mtx3_ };
            ++molecule_release_counter_;

            // If we are the last atom passed the barrier, clear all positions and give way to other atoms in queue
            return molecule_release_counter_ == 3;
        }
    }

    void _clear_fields( void )
    {
        std::scoped_lock _{ mtx1_ };

        hydrogen_slots_ = 0;
        oxygen_slots_ = 0;
        atoms_waiting_ = 0;
        molecule_release_counter_ = 0;

        cv_access_to_barrier_opened_.notify_all();
    }

private:

    std::mutex mtx1_;
    int hydrogen_slots_{ 0 };
    int oxygen_slots_{ 0 };
    std::condition_variable cv_access_to_barrier_opened_;

    std::mutex mtx2_;
    int atoms_waiting_{ 0 };
    std::condition_variable cv_start_release;


    std::mutex mtx3_;
    int molecule_release_counter_{ 0 };

    // I want all external `releaseXxxx` methods to be called synchronously
    std::mutex mtx4_;
};


int main( void )
{
    H2O h20;

    std::string str_atoms{ "HOOHOHHHH" }; //OHHHOHOHOOHOHHOHOOHOHHHHHOOOOOHOHOHOHOHOOHO" };

    std::vector< std::thread > threads;

    for( auto ch : str_atoms ) {

        threads.emplace_back(
            [&h20] ( char ch ) {

                if( ch == 'H' ) {
                    h20.hydrogen( []{ std::cout << 'H'; } );
                } else {
                    h20.oxygen( []{ std::cout << 'o'; } );
                }
            }, ch
        );
    }

    // Join threads one by one
    for( auto& th : threads ) {
        th.join();
    }

    std::cout << std::flush;
    return 0;
}
