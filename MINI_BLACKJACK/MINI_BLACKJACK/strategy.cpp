//
//  strategy.cpp
//  MINI_BLACKJACK
//
//  Created by Jordan Katz on 4/4/20.
//  Copyright © 2020 Jordan Katz. All rights reserved.
//

#include "strategy.hpp"

double evaluate_game_over(const GameState& state) {
//    assert(state.game_over());
    
    if (state.human.has_busted()) {
        return  -1;
    }
    else if (state.dealer.has_busted()) {
        return 1;
    }
    else {
        // no player has busted --> both players have stopped hitting
        if (state.human.get_card_sum() > state.dealer.get_card_sum()) {
            return  1;
        }
        else if (state.human.get_card_sum() < state.dealer.get_card_sum()) {
            return -1;
        }
        return 0; // tie
    }
}

double optimal_move_value(GameState& state, bool is_human) {
    if (state.game_over()) {
        return evaluate_game_over(state);
    }
    
    if (is_human) {
        if (state.human.is_done_hitting()) {
            return optimal_move_value(state, !is_human);
        }
        
        // human can either 'hit' or 'stay'
    
        // hit:
        state.stay(is_human);
        double EV_stay = optimal_move_value(state, !is_human);
        state.undo_stay(is_human);
        
        // stay:
        double EV_hit = 0;
        for (int i = 1; i < 3; i++) {
            state.hit(is_human, i);
            double EV_hit_i = optimal_move_value(state, !is_human);
            state.undo_hit(is_human, i);
            EV_hit += state.prob_flip_card(i) * EV_hit_i;
        }
        
        // return the action with the higher EV
        return std::max(EV_hit, EV_stay);
    }
    
    else { // dealer:
        if (state.dealer.is_done_hitting()) {
            return optimal_move_value(state, !is_human);
        }
        
        // dealer stays
        if (dealer_move(state) == 's') {
            state.stay(is_human);
            double move_value = optimal_move_value(state, !is_human);
            state.undo_stay(is_human);
            return move_value;
        }
        
        // dealer hits
        else {
            double move_value = 0;
            
            for (int i = 1; i < 3; i++) {
                state.hit(is_human, i);
                double move_value_i = optimal_move_value(state, !is_human);
                state.undo_hit(is_human, i);
                move_value += state.prob_flip_card(i) * move_value_i;
            }
            return move_value;
        }
    }
}


char optimal_human_move(GameState& state, bool verbose) {
    assert(!state.game_over());
    assert(!state.human.is_done_hitting());
    
    // returns either 'h' for hit or 's' or stay
    
    if (!verbose) {
        int human_sum = state.human.get_card_sum();
        
        if (human_sum == 3) {
            return 's';
        }
        else if (human_sum == 1) {
            return 'h';
        }
    }
    
    // hit:
    state.stay(true);
    double EV_stay = optimal_move_value(state, false);
    state.undo_stay(true);
    
    // stay:
    double EV_hit = 0;
    for (int i = 1; i < 3; i++) {
        state.hit(true, i);
        double EV_hit_i = optimal_move_value(state, false);
        state.undo_hit(true, i);
        EV_hit += state.prob_flip_card(i) * EV_hit_i;
    }
    
    if (verbose) {
        std::cout << "EV stay: " << EV_stay << '\n';
        std::cout << "EV hit: " << EV_hit << '\n';
    }
    
    return EV_stay >= EV_hit ? 's' : 'h';
}



char dealer_move(const GameState& state) {
    assert(!state.game_over());
    assert(!state.dealer.is_done_hitting());
    
    /*
     returns either 'h' for hit or 's' or stay
     
     Dealer Beth has to take another card if she is behind (11-1, 12-1, 2.-1, 12-1, 111-11, 12.-11, 12-2, 12-2).
     
     Otherwise she gets a card if both have one card, a 2 each, and Ann has stopped (2.-2).
     
     The dealer stops in the remaining cases of a draw (11.-11, 2.-11, 11-2).s
    */
    
    int dealer_sum = state.dealer.get_card_sum();
    int human_sum = state.human.get_card_sum();
    
    if (dealer_sum == 1) {
        return 'h';
    }
    else if (dealer_sum == 3) {
        return 's';
    }
    else { // dealer_sum == 2
        if (human_sum == 1) {
            // ==> player must have stopped hitting ==> dealer is ahead
            return 's';
        }
        else if (human_sum == 3) {
            // dealer is behind
            return 'h';
        }
        else { // human_sum == 2
            if (state.human.is_done_hitting()) {
                if (state.human.n_cards() == 1) {
                    if (state.dealer.n_cards() == 1) {
                        return 'h';
                    }
                }
            }
            return 's';
        }
    }
}
