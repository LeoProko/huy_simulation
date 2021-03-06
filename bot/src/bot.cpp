#include "bot.h"

Bot::Bot()
    : position_(parameters::random() % parameters::map_size,
            parameters::random() % parameters::map_size)
    , is_altruist_(parameters::random() % parameters::bots_amount <
            parameters::altruists_amount)
    , is_greenbeared_(is_altruist_) {
    calibrate();
}

Bot::Bot(const Bot* mother, const Bot* father)
    : position_(mother->position_)
    , health_(
        (
            mother->health_ + father->health_ +
            2 * mother->children_health_ + 2 * father->children_health_
        ) / (
            6 + std::max(0, (mother->children_amount_ + father->children_amount_) / 20)
        )
    )
    , collect_((mother->collect_ + father->collect_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , militancy_((mother->militancy_ + father->militancy_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , intelligence_((mother->intelligence_ + father->intelligence_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , vision_((mother->vision_ + father->vision_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , children_amount_((mother->children_amount_ + father->children_amount_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , children_health_((mother->children_health_ + father->children_health_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2) {
    calibrate();
    if (mother->is_greenbeared_ && father->is_greenbeared_) {
        is_greenbeared_ = parameters::random() % 100 > parameters::mutation;
    } else if (mother->is_greenbeared_ || father->is_greenbeared_) {
        is_greenbeared_ = parameters::random() % 2 ||
            parameters::random() % 100 < parameters::mutation;
    } else {
        is_greenbeared_ = parameters::random() % 100 < parameters::mutation;
    }
    if (mother->is_altruist_ && father->is_altruist_) {
        is_altruist_ = parameters::random() % 100 > parameters::mutation;
    } else if (mother->is_altruist_ || father->is_altruist_) {
        is_altruist_ = parameters::random() % 2 ||
            parameters::random() % 100 < parameters::mutation;
    } else {
        is_altruist_ = parameters::random() % 100 < parameters::mutation;
    }
}

void Bot::calibrate() {
    double coefficient = 50. * genes_amount_ /
               (collect_ + militancy_ + intelligence_ +
                vision_ + children_amount_ + children_health_);
    
    collect_ = std::max(0, std::min(99, static_cast<int>(collect_ * coefficient)));
    militancy_ = std::max(0, std::min(99, static_cast<int>(militancy_ * coefficient)));
    intelligence_ = std::max(0, std::min(99, static_cast<int>(intelligence_ * coefficient)));
    children_amount_ = std::max(0, std::min(99, static_cast<int>(children_amount_ * coefficient)));
    children_health_ = std::max(0, std::min(99, static_cast<int>(children_health_ * coefficient)));
    vision_ = std::max(0, std::min(99, static_cast<int>(vision_ * coefficient)));
}

bool operator<(const Bot& first, const Bot& second) {
    return 
        first.collect_ + 
        first.militancy_ + 
        first.intelligence_ + 
        first.health_ -
        3 * first.lifetime_
        <
        second.collect_ + 
        second.militancy_ + 
        second.intelligence_ + 
        second.health_ -
        3 * second.lifetime_;
}
