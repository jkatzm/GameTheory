//
//  objects.hpp
//  MINI_BLACKJACK
//
//  Created by Jordan Katz on 4/4/20.
//  Copyright © 2020 Jordan Katz. All rights reserved.
//

#ifndef objects_hpp
#define objects_hpp


#include <vector>
#include <string>
#include <unordered_map>
//#include <numeric> // accumulate
#include <iostream> // cout
#include <cassert> // assert
#include <random> // shuffle


class StaticDeck {
    std::vector<int> cards;
    int next_card_idx;

public:
    StaticDeck(int n1, int n2) {
        for (int i = 0; i < n1; i++) cards.push_back(1);
        for (int i = 0; i < n2; i++) cards.push_back(2);
        reset();
    }
    void reset() {
        next_card_idx = 0;
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(cards.begin(), cards.end(), std::mt19937(seed));
//        std::shuffle(cards.begin(), cards.end(), std::mt19937(std::random_device{}()));
    }
    int pick_card() {
        return cards[next_card_idx++];
    }
    int remaining_cards() const {
        return int(cards.size()) - next_card_idx;
    }
    int n_remaining(int card_value) const {
        int n = 0;
        for (int i = next_card_idx; i < cards.size(); i++) {
            if (cards.at(i) == card_value) {
                n++;
            }
        }
        return n;
    }
};




class Player {
    const std::string name;
//    std::vector<int> cards;
    bool stopped_hitting;
    std::unordered_map<int, int> card_counts;
    
public:
    Player(std::string name) : name(name), stopped_hitting(false) {
        card_counts[1] = 0;
        card_counts[2] = 0;
    }

    // functions which DO modify 'this'
    void done_hitting(bool value) {
        stopped_hitting = value;
    }
    void add_card(int card_value) {
//        cards.push_back(card_value);
        card_counts[card_value]++;
    }
    void remove_card(int card_value) {
//        cards.pop_back();
        card_counts[card_value]--;
    }
    
    // functions which DON'T modify 'this'
    bool is_done_hitting() const {
        return stopped_hitting;
    }
    int get_card_sum() const {
//        return std::accumulate(cards.begin(), cards.end(), 0);
        return card_counts.at(1) + 2 * card_counts.at(2);
    }
    size_t n_cards() const {
//        return cards.size();
        return card_counts.at(1) + card_counts.at(2);
    }
    bool has_busted() const {
        return get_card_sum() > 3;
    }
    void print_player() const {
        std::cout << name << " [ ";
        
        for (auto c : card_counts) {
            for (int n = 0; n < c.second; n++) {
                std::cout << c.first << "  ";
            }
        }
        
        std::cout << "]";
        
        if (stopped_hitting) {
            std::cout << " (done)";
        }
        
        std::cout << '\n';
    }
};



class GameState {
    std::unordered_map<int, int> n_remaining;

public:
    Player human = Player("You");
    Player dealer = Player("Dealer");

    GameState(int n1, int n2) {
        n_remaining[1] = n1;
        n_remaining[2] = n2;
    }
    
    // functions which DO modify 'this'
    void deal_top_card(bool is_human, int card_value) {
        hit(is_human, card_value);
    }
    
    void stay(bool is_human) {
        is_human ? human.done_hitting(true) : dealer.done_hitting(true);
    }
    
    void undo_stay(bool is_human) {
        is_human ? human.done_hitting(false) : dealer.done_hitting(false);
    }
    
    void hit(bool is_human, int card_value) {
        n_remaining[card_value]--;
        is_human ? human.add_card(card_value) : dealer.add_card(card_value);
    }
    
    void undo_hit(bool is_human, int card_value) {
        n_remaining[card_value]++;
        is_human ? human.remove_card(card_value) : dealer.remove_card(card_value);
    }
    
    // function which DON'T modify 'this'
    bool game_over() const {
        // if someone has busted ==> game over
        if (human.has_busted() || dealer.has_busted()) {
            return true;
        }
        // otherwise, return true iff both players are done hitting
        return human.is_done_hitting() && dealer.is_done_hitting();
    }
    
    double prob_flip_card(int card_value) const {
        int n1 = n_remaining.at(1);
        int n2 = n_remaining.at(2);
        
        if (card_value == 1 && n1 > 0) {
            return double(n1) / double(n1 + n2);
        }

        else if (card_value == 2 && n2 > 0) {
            return double(n2) / double(n1 + n2);
        }
        
        return 0;
    }
    
    void print_state() const {
        std::cout << "\n";
        human.print_player();
        dealer.print_player();
        std::cout << "# 1s remaining: " << n_remaining.at(1) << '\n';
        std::cout << "# 2s remaining: " << n_remaining.at(2) << '\n';
        std::cout << "\n\n";
    }
};

#endif /* objects_hpp */
