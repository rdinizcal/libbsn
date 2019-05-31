#ifndef MARKOV_HPP
#define MARKOV_HPP

#include <iostream>
#include <array> 
#include <random>
#include <vector>
#include <stdint.h>
#include <sstream>

#include "range/Range.hpp"

namespace bsn {
    namespace generator {

        class Markov {
            public:
                Markov();
                
                Markov(std::array<float, 25> transitions, std::array<bsn::range::Range, 5> states, int32_t initialState);

                Markov(const Markov & /*obj*/);
                Markov& operator=(const Markov & /*obj*/);

                // Contém a probabilidade de todas as transições
                std::array<float,25> transitions;
                // 0 para low 1 para medium e 2 para high
                int32_t currentState;
                // Contém os intervalos de cada estado
                std::array<bsn::range::Range, 5> states;

                const std::string toString() const;
        };

    }

}

#endif