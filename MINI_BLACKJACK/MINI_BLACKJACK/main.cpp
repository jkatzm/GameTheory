//
//  main.cpp
//  MINI_BLACKJACK
//
//  Created by Jordan Katz on 4/4/20.
//  Copyright © 2020 Jordan Katz. All rights reserved.
//


#include "strategy.hpp"
#include <unistd.h> // sleep


double EV_subgame(int n1, int n2) {
    // returns the EV of a game with 'n1' 1s and 'n2' 2s remaining in the deck
    // assuming we play optimally and the dealer plays according to their fixed strategy
    
    StaticDeck deck = StaticDeck(n1, n2);
    
    const double n_samples = 1000;
    int n_wins = 0;
    int n_losses = 0;
    
    for (int s = 0; s < n_samples; s++) {
        deck.reset();
        
        GameState G = GameState(n1, n2);
        G.deal_top_card(true, deck.pick_card());
        G.deal_top_card(false, deck.pick_card());
        
        bool human_turn = true;
        
        while (!G.game_over()) {
            char next_move = human_turn ? optimal_human_move(G, false) : dealer_move(G);
            next_move == 's' ? G.stay(human_turn) : G.hit(human_turn, deck.pick_card());
            
            // move to the next player
            if (human_turn) {
                if (!G.dealer.is_done_hitting()) {
                    human_turn = false;
                }
            }
            else {
                if (!G.human.is_done_hitting()) {
                    human_turn = true;
                }
            }
        }
        
        double game_result = evaluate_game_over(G);
        if (game_result == 1) n_wins++;
        else if (game_result == -1) n_losses++;
    }
    
    return (n_wins-n_losses)/n_samples;
}


double prob_subgame(int i, int j) {
    // returns the probability that 'i' 1s and 'j' 2s remaining in the deck,
    // given it started with 16 of each and no reshuffling has taken place
    // (effectively hypergeometric probabilities)
    
    if (i > 16 || j > 16) {
        return 0;
    }
    
    if (i==16 && j==16) {
        return 1;
    }
        
    return (j+1)/double(i+j+1) * prob_subgame(i, j+1) + (i+1)/double(i+j+1) * prob_subgame(i+1, j);
}


double EV_game(int max_bet) {
    // assumes deck is reschuffled once deck size <= 5
    // assumes we bet "max_bet" each time we enter a positive-EV subgame (and $1 otherwise)

    double EV = 0;
    
    for (int i = 0; i < 17; i++) {
        for (int j = 0; j < 17; j++) {
            if (i + j >= 6) {
                double ev_sub = EV_subgame(i, j);
                if (ev_sub > 0) ev_sub *= 10;
                EV += ev_sub * prob_subgame(i, j);
            }
        }
    }

    EV *= 1000/27.0; // adjusts for the missing entries for when deck size <= 5
    
    return EV;
}



void UI_game(int n1, int n2, int max_bet) {
    std::cout << "\nStarting a new game of MINI-BLACKJACK\n";
    std::cout << "(as described in Erich Prisner's \"Game Theory Through Examples\")\n\n";
    std::cout << "Deck has " << n1 << " 1s and " << n2 << " 2s\n";
    std::cout << "The max bet size is " << max_bet << "\n";
    std::cout << "The dealer will reshuffle once the deck has <= 5 cards remaining\n\n\n";
    
    StaticDeck deck = StaticDeck(n1, n2);
    
    int net_winnings = 0;
    
    while (deck.remaining_cards() >= 6) {
        int bet;
        std::cout << "Place bet to play\n";
        
        double subgame_EV = EV_subgame(deck.n_remaining(1), deck.n_remaining(2));
        std::cout << "(optimal bet size: " << (subgame_EV > 0 ? max_bet : 1) << ")\n\n";
        
        std::cout << "enter number in range [1, " << max_bet << "]: ";
        std::cin >> bet;
        assert(1 <= bet && bet <= max_bet);
        
        
        
        
        GameState G = GameState(deck.n_remaining(1), deck.n_remaining(2));
        G.deal_top_card(true, deck.pick_card());
        G.deal_top_card(false, deck.pick_card());
        
        bool human_turn = true;
        
        while (!G.game_over()) {
            G.print_state();
            sleep(2);
            

            if (human_turn) {
                std::cout << "\nYour turn– hit or stay?\n";
                sleep(2);
                
                char opt_move = optimal_human_move(G, false);
                std::cout << "(optimal move: " << (opt_move == 'h' ? "hit)\n\n" : "stay)\n\n");
                
                char move;
                std::cout << "enter move ['h' or 's']: ";
                std::cin >> move;
                std::cout << '\n';
                assert(move == 'h' || move == 's');
                move == 's' ? G.stay(human_turn) : G.hit(human_turn, deck.pick_card());
                
                std::cout << "You've decided to " << (move == 'h' ? "hit\n" : "stay\n");
                sleep(5);
                
                // move to the next player
                if (!G.dealer.is_done_hitting()) {
                    human_turn = false;
                }
                
            }
            
            else {
                std::cout << "\nDealer's turn...\n\n";
                sleep(2);
                
                char move = dealer_move(G);
                move == 's' ? G.stay(human_turn) : G.hit(human_turn, deck.pick_card());
                
                std::cout << "Dealer " << (move == 'h' ? "hits\n" : "stays\n");
                sleep(5);
                
                // move to the next player
                if (!G.human.is_done_hitting()) {
                    human_turn = true;
                }
            }
            
            
        }
        
        G.print_state();
        std::cout << "\n\n";
        double game_result = evaluate_game_over(G);
        
        if (game_result == 1) {
            std::cout << "game over.\nyou earned " << bet << '\n';
            net_winnings += bet;
        }
        else if (game_result == -1) {
            std::cout << "game over.\nyou lost " << bet << '\n';
            net_winnings -= bet;
        }
        else {
            std::cout << "game over.\nyour bet has been returned to you\n";
        }
        std::cout << "net winnings: " << net_winnings << "\n\n\n";
        sleep(5);
    }
}






int main(int argc, const char * argv[]) {
    std::time_t start, finish;
    std::time(&start);

//    std::cout << EV_game(10) << '\n';
    
    const int N1s = 16;
    const int N2s = 16;
    const int MAX_BET = 5;
    assert(N1s + N2s >= 6);

    UI_game(N1s, N2s, MAX_BET);

    std::time(&finish);
    std::cout << "time: " << std::difftime(finish, start) << " sec\n";
    return 0;
}


