#include "bot.h"

Bot::Bot()
    : position_(parameters::random() % parameters::map_size,
            parameters::random() % parameters::map_size)
    , is_private_altruist_(parameters::random() % parameters::bots_amount <
            parameters::altruists_amount)
    , is_public_altruist_(is_private_altruist_) {
    calibrate();
}

Bot::Bot(const Bot* mother, const Bot* father)
    : position_(mother->position_)
    , health_((mother->children_health_ + mother->health_ +
        father->children_health_ + father->health_) / 4)
    , militancy_ ((mother->militancy_ + father->militancy_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , intelligence_ ((mother->intelligence_ + father->intelligence_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , children_amount_ ((mother->children_amount_ + father->children_amount_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2)
    , children_health_ ((mother->children_health_ + father->children_health_) / 2 +
        parameters::random() % parameters::mutation - parameters::mutation / 2) {
    calibrate();
    if (mother->is_public_altruist_ && father->is_public_altruist_) {
        is_public_altruist_ = parameters::random() % 100 > parameters::mutation;
    } else if (mother->is_public_altruist_ || father->is_public_altruist_) {
        is_public_altruist_ = parameters::random() % 2;
    } else {
        is_public_altruist_ = parameters::random() % 2 ||
            parameters::random() % 100 < parameters::mutation;
    }
    if (mother->is_private_altruist_ && father->is_private_altruist_) {
        is_private_altruist_ *= parameters::random() % 100 > parameters::mutation;
    } else if (mother->is_private_altruist_ || father->is_private_altruist_) {
        is_private_altruist_ = parameters::random() % 2 ||
            parameters::random() % 100 < parameters::mutation;
    } else {
        is_private_altruist_ = parameters::random() % 100 < parameters::mutation;
    }
}

void Bot::calibrate() {
    double coefficient = 50. * genes_amount_ /
               (militancy_ + intelligence_ +
                children_amount_ + children_health_);
    
    militancy_ = std::max(0, std::min(99, static_cast<int>(militancy_ * coefficient)));
    intelligence_ = std::max(0, std::min(99, static_cast<int>(intelligence_ * coefficient)));
    children_amount_ = std::max(0, std::min(9, static_cast<int>(children_amount_ * coefficient)));
    children_health_ = std::max(0, std::min(99, static_cast<int>(children_health_ * coefficient)));
}

bool operator<(const Bot& first, const Bot& second) {
    return first.militancy_ +
        first.intelligence_ +
        first.children_amount_ +
        first.children_health_
        <
        second.militancy_ +
        second.intelligence_ +
        second.children_amount_ +
        second.children_health_;
}
