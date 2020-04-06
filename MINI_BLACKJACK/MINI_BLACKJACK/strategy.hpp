//
//  strategy.hpp
//  MINI_BLACKJACK
//
//  Created by Jordan Katz on 4/4/20.
//  Copyright © 2020 Jordan Katz. All rights reserved.
//

#ifndef strategy_hpp
#define strategy_hpp

#include "objects.hpp"


double evaluate_game_over(const GameState& state);
double optimal_move_value(GameState& state, bool is_human);
char optimal_human_move(GameState& state, bool verbose);
char dealer_move(const GameState& state);

#endif /* strategy_hpp */
